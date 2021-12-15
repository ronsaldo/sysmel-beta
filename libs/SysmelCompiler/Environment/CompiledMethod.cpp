#include "Environment/CompiledMethod.hpp"
#include "Environment/ASTNode.hpp"
#include "Environment/CannotEvaluateMessageInCompileTime.hpp"
#include "Environment/CannotEvaluateUndefinedMessage.hpp"
#include "Environment/BootstrapTypeRegistration.hpp"
#include "Environment/CompilationError.hpp"
#include "Environment/ASTSemanticAnalyzer.hpp"
#include "Environment/ASTCompileTimeEvaluator.hpp"
#include "Environment/ASTSSACompiler.hpp"

#include "Environment/SSAFunction.hpp"
#include "Environment/SSACodeRegion.hpp"
#include "Environment/SSACodeRegionArgument.hpp"
#include "Environment/SSACodeRegionCapture.hpp"

#include "Environment/ASTAnalysisEnvironment.hpp"
#include "Environment/CleanUpScope.hpp"
#include "Environment/LexicalScope.hpp"
#include "Environment/ReceiverMemberLookupScope.hpp"
#include "Environment/CompileTimeCleanUpScope.hpp"

#include "Environment/FunctionalType.hpp"
#include "Environment/ArgumentVariable.hpp"
#include "Environment/ArgumentCountError.hpp"
#include <assert.h>

namespace Sysmel
{
namespace Environment
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
    IdentifierLookupScopePtr parentScope = definitionEnvironment->lexicalScope;

    if(receiverArgument)
    {
        auto receiverScope = basicMakeObject<ReceiverMemberLookupScope> ();
        receiverScope->parent = parentScope;
        receiverScope->receiverType = receiverArgument->getValueType();
        receiverScope->receiverVariable = receiverArgument;
        parentScope = receiverScope;
    }

    auto lexicalScope = LexicalScope::makeWithParent(parentScope);
    result->lexicalScope = lexicalScope;
    result->returnTargetMethod = selfFromThis();
    result->localDefinitionsOwner = selfFromThis();

    // Add the receiver argument
    if(receiverArgument)
    {
        if(isMacroMethod())
        {
            if(validAnyValue(receiverArgument->getName())->isAnonymousNameSymbol())
                receiverArgument->setName(internSymbol("__macroContext"));
        }
        else
        {
            if(validAnyValue(receiverArgument->getName())->isAnonymousNameSymbol())
                receiverArgument->setName(internSymbol("self"));
        }

        lexicalScope->setSymbolBinding(receiverArgument->getName(), receiverArgument);
    }

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
    createImplicitReceiverArgumentVariableWithType(receiverType);
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

void CompiledMethod::createImplicitReceiverArgumentVariableWithType(const TypePtr &receiverType)
{
    if(receiverType->isVoidType())
        return;

    receiverArgument = basicMakeObject<ArgumentVariable> ();
    receiverArgument->isImplicit = true;
    recordChildProgramEntityDefinition(receiverArgument);
    receiverArgument->setType(receiverType);
}

void CompiledMethod::createArgumentVariablesWithTypes(const TypePtrList &argumentTypes)
{
    arguments.reserve(argumentTypes.size());
    for(const auto &type : argumentTypes)
    {
        auto argument = basicMakeObject<ArgumentVariable> ();
        recordChildProgramEntityDefinition(argument);
        argument->setType(type);
        arguments.push_back(argument);
    }
}

void CompiledMethod::setArgumentDeclarationNode(size_t index, const ASTArgumentDefinitionNodePtr &argumentNode)
{
    assert(index < arguments.size());
    arguments[index]->setArgumentDeclarationNode(argumentNode);
}

void CompiledMethod::setArgumentDefinitionNode(size_t index, const ASTArgumentDefinitionNodePtr &argumentNode)
{
    assert(index < arguments.size());
    arguments[index]->setArgumentDefinitionNode(argumentNode);
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

    {
        auto analyzer = basicMakeObject<ASTSemanticAnalyzer> ();
        analyzeDefinitionWith(analyzer);
        auto compilationError = analyzer->makeCompilationError();
        if(compilationError)
            compilationError->signal();
    }

    asSSAValueRequiredInPosition(nullptr);
}

const ArgumentVariablePtrList &CompiledMethod::getArguments() const
{
    return arguments;
}

void CompiledMethod::recordCapturedFunctionVariable(const FunctionVariablePtr &capturedVariable)
{
    if(capturedVariableSet.find(capturedVariable) != capturedVariableSet.end())
        return;

    // Make sure that my parent also captures the variable.
    auto parentEntity = getParentProgramEntity();
    auto variableParentEntity = capturedVariable->getParentProgramEntity();
    assert(variableParentEntity->isCompiledMethod());
    if(parentEntity->isCompiledMethod() && parentEntity != variableParentEntity)
        parentEntity.staticAs<CompiledMethod> ()->recordCapturedFunctionVariable(capturedVariable);

    // Record the captured variable.
    capturedVariables.push_back(capturedVariable);
    capturedVariableSet.insert(capturedVariable);
}

const FunctionVariablePtrList &CompiledMethod::getCapturedVariables() const
{
    return capturedVariables;
}

SSAValuePtr CompiledMethod::asSSAValueRequiredInPosition(const ASTSourcePositionPtr &requiredSourcePosition)
{
    (void)requiredSourcePosition;
    if(ssaCompiledFunction)
        return ssaCompiledFunction;

    // Make the function.    
    ssaCompiledFunction = basicMakeObject<SSAFunction> ();
    ssaCompiledFunction->initializeWithNameAndType(getName(), functionalType);
    ssaCompiledFunction->setIntrinsicName(getIntrinsicName());
    ssaCompiledFunction->setDeclarationPosition(declarationPosition);
    ssaCompiledFunction->setSourceProgramEntity(selfFromThis());

    ensureSemanticAnalysis();

    // Add the function onto its ssa parent.
    auto parentEntity = getParentProgramEntity();
    // FIXME: Add the top-level script methods some-where.
    if(parentEntity)
    {
        auto parentSSAValue = parentEntity->asProgramEntitySSAValue();
        assert(parentSSAValue->isSSAProgramEntity());
        parentSSAValue.staticAs<SSAProgramEntity> ()->addChild(ssaCompiledFunction);
    }

    auto mainCodeRegion = ssaCompiledFunction->getMainCodeRegion();

    // Set the argument metadata.
    size_t argumentsOffset = 0;
    auto receiverType = functionalType->getReceiverType();
    if(!receiverType->isVoidType())
    {
        mainCodeRegion->getArgument(0)->setDeclarationPosition(declarationPosition);
        ++argumentsOffset = 1;
    }

    for(size_t i = 0; i < arguments.size(); ++i)
        mainCodeRegion->getArgument(i + argumentsOffset)->setDeclarationPosition(arguments[i]->getDeclarationPosition());

    // Add the captured variables.
    for(auto &capturedVariable : capturedVariables)
    {
        auto capture = mainCodeRegion->addCaptureWithType(capturedVariable->getReferenceType());
        capture->setDeclarationPosition(capturedVariable->getDeclarationPosition());
        capture->setDefinitionPosition(capturedVariable->getDefinitionPosition());
    }

    if(analyzedBodyNode)
    {
        ssaCompiledFunction->setDefinitionPosition(definitionPosition);
        if(!receiverType->isVoidType())
            mainCodeRegion->getArgument(0)->setDefinitionPosition(declarationPosition);
        for(size_t i = 0; i < arguments.size(); ++i)
            mainCodeRegion->getArgument(i + argumentsOffset)->setDefinitionPosition(arguments[i]->getDefinitionPosition());

        auto compiler = basicMakeObject<ASTSSACompiler> ();
        compiler->compileMethodBody(selfFromThis(), ssaCompiledFunction, analyzedBodyNode);
    }

    return ssaCompiledFunction;
}

std::string CompiledMethod::fullPrintString() const
{
    return sexpressionToPrettyString(asFullDefinitionSExpression());
}

SExpression CompiledMethod::asSExpression() const
{
    return SExpressionList{{
        SExpressionIdentifier{"compiledMethod"},
        validAnyValue(name)->asSExpression(),
        functionalType->asSExpression()
    }};
}

SExpression CompiledMethod::asFullDefinitionSExpression() const
{
    SExpressionList childrenSExpr;
    childrenSExpr.elements.reserve(children.size());
    for(auto &child : children)
        childrenSExpr.elements.push_back(child->asFullDefinitionSExpression());

    SExpression bodySExpr = SExpressionVoid{};
    if(analyzedBodyNode)
        bodySExpr = analyzedBodyNode->asSExpression();

    return SExpressionList{{
        SExpressionIdentifier{"compiledMethod"},
        validAnyValue(name)->asSExpression(),
        functionalType->asSExpression(),
        childrenSExpr,
        bodySExpr
    }};
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
    return basicMakeObject<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

AnyValuePtr CompiledMethod::applyInClosureWithArguments(const AnyValuePtr &closure, const std::vector<AnyValuePtr> &arguments)
{
    assert(functionalType->isClosureType());
    assert(closure->isCompileTimeCleanUpScope());
    validateBeforeCompileTimeEvaluation();

    auto evaluationEnvironment = CompileTimeCleanUpScope::makeWithParent(staticObjectCast<CompileTimeCleanUpScope> (closure));
    setArgumentsInEvaluationEnvironment(nullptr, arguments, evaluationEnvironment);
    return basicMakeObject<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

void CompiledMethod::setArgumentsInEvaluationEnvironment(const AnyValuePtr &receiver, const AnyValuePtrList &argumentValues, const CompileTimeCleanUpScopePtr &environment)
{
    auto expectedReceiverType = functionalType->getReceiverType();
    if(!expectedReceiverType->isVoidType())
    {
        assert(receiverArgument);
        environment->setStoreBinding(receiverArgument, receiver);
    }

    // Check the argument count.
    if(arguments.size() != argumentValues.size())
    {
        auto error = basicMakeObject<ArgumentCountError> ();
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
    return basicMakeObject<ASTCompileTimeEvaluator> ()->evaluateMethodBodyNode(evaluationEnvironment, analyzedBodyNode);
}

void CompiledMethod::recordChildProgramEntityDefinition(const ProgramEntityPtr &newChild)
{
    children.push_back(newChild);
    newChild->setParentProgramEntity(selfFromThis());
}

} // End of namespace Environment
} // End of namespace Sysmel