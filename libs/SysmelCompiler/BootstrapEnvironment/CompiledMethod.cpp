#include "sysmel/BootstrapEnvironment/CompiledMethod.hpp"
#include "sysmel/BootstrapEnvironment/ASTNode.hpp"
#include "sysmel/BootstrapEnvironment/CannotEvaluateMessageInCompileTime.hpp"
#include "sysmel/BootstrapEnvironment/CannotEvaluateUndefinedMessage.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"
#include "sysmel/BootstrapEnvironment/CompilationError.hpp"
#include "sysmel/BootstrapEnvironment/ASTSemanticAnalyzer.hpp"
#include "sysmel/BootstrapEnvironment/ASTCompileTimeEvaluator.hpp"

#include "sysmel/BootstrapEnvironment/ASTAnalysisEnvironment.hpp"
#include "sysmel/BootstrapEnvironment/CleanUpScope.hpp"
#include "sysmel/BootstrapEnvironment/LexicalScope.hpp"
#include "sysmel/BootstrapEnvironment/CompileTimeCleanUpScope.hpp"

#include "sysmel/BootstrapEnvironment/FunctionalType.hpp"
#include "sysmel/BootstrapEnvironment/ArgumentVariable.hpp"
#include "sysmel/BootstrapEnvironment/ArgumentCountError.hpp"
#include <assert.h>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<CompiledMethod> compiledMethodTypeRegistration;

bool CompiledMethod::isCompiledMethod() const
{
    return true;
}

bool CompiledMethod::isDefined() const
{
    return definitionEnvironment || analyzedBodyNode;
}

bool CompiledMethod::isDefinedForCompileTime() const
{
    return isDefined();
}

void CompiledMethod::setDeclaration(const ASTNodePtr &node)
{
    declarationNode = node;
    declarationPosition = node->sourcePosition;
}

void CompiledMethod::setDefinition(const ASTNodePtr &node, const ASTNodePtr &bodyNode, const ASTAnalysisEnvironmentPtr &environment)
{
    if(!declarationNode)
        setDeclaration(node);

    definitionNode = node;
    definitionEnvironment = environment;
    definitionPosition = node->sourcePosition;
    definitionBodyNode = bodyNode;
}

ASTAnalysisEnvironmentPtr CompiledMethod::createSemanticAnalysisEnvironment()
{
    auto result = definitionEnvironment->copyWithCleanUpcope(CleanUpScope::makeEmpty());
    auto lexicalScope = LexicalScope::makeWithParent(definitionEnvironment->lexicalScope);
    result->lexicalScope = lexicalScope;
    result->localDefinitionsOwner = shared_from_this();

    // Add the arguments to the lexical scope.
    for(auto &arg : arguments)
    {
        if(arg->getName())
            lexicalScope->setSymbolBinding(arg->getName(), arg);
    }

    return result;
}

void CompiledMethod::setMethodSignature(const TypePtr &receiverType, const TypePtr &resultType, const TypePtrList &argumentTypes)
{
    SuperType::setMethodSignature(receiverType, resultType, argumentTypes);
    createArgumentVariablesWithTypes(argumentTypes);
}

void CompiledMethod::setFunctionSignature(const TypePtr &resultType, const TypePtrList &argumentTypes)
{
    SuperType::setFunctionSignature(resultType, argumentTypes);
    createArgumentVariablesWithTypes(argumentTypes);
}

void CompiledMethod::setClosureSignature(const TypePtr &resultType, const TypePtrList &argumentTypes)
{
    SuperType::setClosureSignature(resultType, argumentTypes);
    createArgumentVariablesWithTypes(argumentTypes);
}

void CompiledMethod::createArgumentVariablesWithTypes(const TypePtrList &argumentTypes)
{
    arguments.reserve(argumentTypes.size());
    for(const auto &type : argumentTypes)
    {
        auto argument = std::make_shared<ArgumentVariable> ();
        recordChildProgramEntityDefinition(argument);
        argument->setType(type);
        arguments.push_back(argument);
    }
}

void CompiledMethod::setArgumentDeclarationNode(size_t index, const ASTArgumentDefinitionNodePtr &argumentNode)
{
    assert(index < arguments.size());
    arguments[index]->setDeclarationNode(argumentNode);
}

void CompiledMethod::setArgumentDefinitionNode(size_t index, const ASTArgumentDefinitionNodePtr &argumentNode)
{
    assert(index < arguments.size());
    arguments[index]->setDefinitionNode(argumentNode);
}

ASTNodePtr CompiledMethod::analyzeDefinitionWith(const ASTSemanticAnalyzerPtr &analyzer)
{
    if(analyzedBodyNode)
        return analyzedBodyNode;

    analyzedBodyNode = analyzer->withEnvironmentDoAnalysis(createSemanticAnalysisEnvironment(), [&]() {
        return analyzer->analyzeNodeIfNeededWithExpectedType(definitionBodyNode, functionalType->getResultType());
    });

    concretizeAutoResultTypeWith(analyzedBodyNode->analyzedType);
    return analyzedBodyNode;
}

void CompiledMethod::ensureSemanticAnalysis()
{
    if(analyzedBodyNode || !isDefined())
        return;

    auto analyzer = std::make_shared<ASTSemanticAnalyzer> ();
    analyzeDefinitionWith(analyzer);
    auto compilationError = analyzer->makeCompilationError();
    if(compilationError)
        compilationError->signal();
}

std::string CompiledMethod::fullPrintString() const
{
    return sexpressionToPrettyString(asFullDefinitionSExpression());
}

SExpression CompiledMethod::asFullDefinitionSExpression() const
{
    if(analyzedBodyNode)
        return analyzedBodyNode->asSExpression();

    return SExpressionVoid{};
}

void CompiledMethod::validateBeforeCompileTimeEvaluation()
{
    if(!isDefined())
        signalNew<CannotEvaluateUndefinedMessage> ();
    if(!isDefinedForCompileTime())
        signalNew<CannotEvaluateMessageInCompileTime> ();
    ensureSemanticAnalysis();
}

AnyValuePtr CompiledMethod::runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
{
    assert(!functionalType->isClosureType());
    (void)selector;
    
    validateBeforeCompileTimeEvaluation();

    auto evaluationEnvironment = CompileTimeCleanUpScope::makeEmpty();
    setArgumentsInEvaluationEnvironment(receiver, arguments, evaluationEnvironment);
    return std::make_shared<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

AnyValuePtr CompiledMethod::applyInClosureWithArguments(const AnyValuePtr &closure, const std::vector<AnyValuePtr> &arguments)
{
    assert(functionalType->isClosureType());
    assert(closure->isCompileTimeCleanUpScope());
    validateBeforeCompileTimeEvaluation();

    auto evaluationEnvironment = CompileTimeCleanUpScope::makeWithParent(std::static_pointer_cast<CompileTimeCleanUpScope> (closure));
    setArgumentsInEvaluationEnvironment(nullptr, arguments, evaluationEnvironment);
    return std::make_shared<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

void CompiledMethod::setArgumentsInEvaluationEnvironment(const AnyValuePtr &receiver, const AnyValuePtrList &argumentValues, const CompileTimeCleanUpScopePtr &environment)
{
    auto expectedReceiverType = functionalType->getReceiverType();
    if(!expectedReceiverType->isVoidType())
    {
        (void)receiver;
        assert("TODO: set compile time evaluation receiver" && false);
    }

    // Check the argument count.
    if(arguments.size() != argumentValues.size())
    {
        auto error = std::make_shared<ArgumentCountError> ();
        error->expectedCount = functionalType->getArgumentCount();
        error->callCount = arguments.size();
        error->signal();
    }

    // Set the different arguments.
    for(size_t i = 0; i < arguments.size(); ++i)
        environment->setStoreBinding(arguments[i], argumentValues[i]);
}

AnyValuePtr CompiledMethod::applyWithArguments(const std::vector<AnyValuePtr> &arguments)
{
    assert(!functionalType->isClosureType());
    validateBeforeCompileTimeEvaluation();
    auto evaluationEnvironment = CompileTimeCleanUpScope::makeEmpty();
    setArgumentsInEvaluationEnvironment(nullptr, arguments, evaluationEnvironment);
    return std::make_shared<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

void CompiledMethod::recordChildProgramEntityDefinition(const ProgramEntityPtr &newChild)
{
    children.push_back(newChild);
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius