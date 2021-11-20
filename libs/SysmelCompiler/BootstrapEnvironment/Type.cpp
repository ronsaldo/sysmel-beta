#include "sysmel/BootstrapEnvironment/Type.hpp"
#include "sysmel/BootstrapEnvironment/MethodDictionary.hpp"
#include "sysmel/BootstrapEnvironment/MessageNotUnderstood.hpp"
#include "sysmel/BootstrapEnvironment/ASTCallNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTMessageSendNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTLiteralValueNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTSemanticAnalyzer.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapMethod.hpp"
#include "sysmel/BootstrapEnvironment/StringUtilities.hpp"
#include "sysmel/BootstrapEnvironment/DeferredCompileTimeCodeFragment.hpp"
#include <algorithm>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{
static BootstrapTypeRegistration<Type> typeTypeRegistration;

MethodCategories Type::__instanceMethods__()
{
    return MethodCategories{
        {"accessing", {
            makeMethodBinding("supertype", &Type::getSupertype),
            makeMethodBinding("subtypes", &Type::getSubtypes),
        }},
    };
}

bool Type::isUndefinedType() const
{
    return isSubtypeOf(getUndefinedType());
}

bool Type::isAutoType() const
{
    return this == getAutoType().get();
}

bool Type::isVoidType() const
{
    return isSubtypeOf(getVoidType());
}

bool Type::isReturnType() const
{
    return isSubtypeOf(getReturnType());
}

bool Type::isLiteralValueType() const
{
    return isSubtypeOf(getLiteralValueType());
}

bool Type::isCompilationErrorValueType() const
{
    return isSubtypeOf(getCompilationErrorValueType());
}

bool Type::isType() const
{
    return true;
}

AnyValuePtr Type::getName() const
{
    return name;
}

void Type::setName(const AnyValuePtr &newName)
{
    name = newName;
}

std::string Type::printString() const
{
    if(name)
        return name->asString();
    return SuperType::printString();
}

TypePtr Type::getSupertype() const
{
    return supertype;
}

void Type::setSupertype(const TypePtr &newSupertype)
{
    supertype = newSupertype;
}

const TypePtrList &Type::getSubtypes()
{
    return subtypes;
}

void Type::registerSubtype(const TypePtr &subtype)
{
    if(!subtype->isMetaType())
        subtypes.push_back(subtype);
}

TypePtr Type::asReceiverType()
{
    return shared_from_this();
}

AnyValuePtr Type::lookupLocalSelector(const AnyValuePtr &selector)
{
    return methodDictionary ? methodDictionary->lookupSelector(selector) : AnyValuePtr();
}

AnyValuePtr Type::lookupLocalMacroSelector(const AnyValuePtr &selector)
{
    return macroMethodDictionary ? macroMethodDictionary->lookupSelector(selector) : AnyValuePtr();
}

AnyValuePtr Type::lookupLocalMacroFallbackSelector(const AnyValuePtr &selector)
{
    return macroFallbackMethodDictionary ? macroFallbackMethodDictionary->lookupSelector(selector) : AnyValuePtr();
}

AnyValuePtr Type::lookupMacroSelector(const AnyValuePtr &selector)
{
    auto localMethod = lookupLocalMacroSelector(selector);
    if(localMethod)
        return localMethod;

    auto superType = getSupertype();
    if(superType)
        return superType->lookupMacroSelector(selector);

    return AnyValuePtr();
}

AnyValuePtr Type::lookupSelector(const AnyValuePtr &selector)
{
    auto localMethod = lookupLocalSelector(selector);
    if(localMethod)
        return localMethod;

    auto superType = getSupertype();
    if(superType)
        return superType->lookupSelector(selector);

    return AnyValuePtr();
}

AnyValuePtr Type::lookupMacroFallbackSelector(const AnyValuePtr &selector)
{
    auto localMethod = lookupLocalMacroFallbackSelector(selector);
    if(localMethod)
        return localMethod;

    auto superType = getSupertype();
    if(superType)
        return superType->lookupMacroFallbackSelector(selector);

    return AnyValuePtr();
}

/// This method performs the semantic analysis of a call node with the specified semantic analyzer.
ASTNodePtr Type::analyzeCallNode(const ASTCallNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    assert(node->function && node->function->analyzedType);

    // In case the fuction is an error, analyze the arguments to
    // further discover more error, and then return the receiver to propagate an error node.
    if(isCompilationErrorValueType())
    {
        assert(node->function->analyzedType->isCompilationErrorValueType());
        for(const auto &arg : node->arguments)
            semanticAnalyzer->analyzeNodeIfNeededWithAutoType(arg);
        return node->function;
    }

    // Allow the actual function to analyze the call. This is typically used by metabuilders.
    if(supportsMessageAnalysisByLiteralValueReceivers() && node->function->isASTLiteralValueNode())
    {
        auto literalValue = std::static_pointer_cast<ASTLiteralValueNode> (node->function)->value;
        return literalValue->analyzeCallNode(node, semanticAnalyzer);
    }

    // By default, convert the call into a message send, and then analyze back.
    return semanticAnalyzer->analyzeCallNodeByConvertingToMessageSend(node);
}

ASTNodePtr Type::analyzeMessageSendNode(const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    assert(node->receiver && node->receiver->analyzedType);
    assert(node->selector && node->selector->analyzedType);

    // Allow the actual instance to analyze the message. This is typically used by metabuilders.
    if(supportsMessageAnalysisByLiteralValueReceivers() && node->receiver->isASTLiteralValueNode())
    {
        auto literalValue = std::static_pointer_cast<ASTLiteralValueNode> (node->receiver)->value;
        return literalValue->analyzeMessageSendNode(node, semanticAnalyzer);
    }

    return analyzeMessageSendNodeWithTypeDefinedMethods(node, semanticAnalyzer);
}

ASTNodePtr Type::analyzeMessageSendNodeWithTypeDefinedMethods(const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    assert(node->receiver && node->receiver->analyzedType);
    assert(node->selector && node->selector->analyzedType);

    // In case the receiver is an error, analyze the arguments to
    // further discover more error, and then return the receiver to propagate an error node.
    if(isCompilationErrorValueType())
    {
        assert(node->receiver->analyzedType->isCompilationErrorValueType());
        for(const auto &arg : node->arguments)
            semanticAnalyzer->analyzeNodeIfNeededWithAutoType(arg);
        return node->receiver;
    }

    AnyValuePtr directSelectorValue;
    if(node->selector->isASTLiteralValueNode())
        directSelectorValue = std::static_pointer_cast<ASTLiteralValueNode> (node->selector)->value;

    // Attempt going through the different expansion levels
    while(uint8_t(node->expansionLevel) < uint8_t(MessageSendExpansionLevel::Count))
    {
        if(directSelectorValue)
        {
            auto macroOrMethod = lookupSelectorInReceiverNodeWithExpansionLevel(directSelectorValue, node->receiver, node->expansionLevel);
            if(macroOrMethod)
                return macroOrMethod->analyzeMessageSendNode(node, semanticAnalyzer);
        }

        node->expansionLevel = MessageSendExpansionLevel(uint8_t(node->expansionLevel) + 1);
    }

    return analyzeUnboundMessageSendNode(node, semanticAnalyzer);
}

bool Type::supportsDynamicCompileTimeMessageSend() const
{
    return false;
}

bool Type::supportsMessageAnalysisByLiteralValueReceivers() const
{
    return false;
}

bool Type::isEphemeralCompileTimeObject() const
{
    return false;
}

ASTNodePtr Type::analyzeUnboundMessageSendNode(const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    if(supportsDynamicCompileTimeMessageSend())
        return semanticAnalyzer->analyzeDynamicCompileTimeMessageSendNode(node);

    // Support the does not understand macro.
    auto dnuMacro = lookupDoesNotUnderstandMacro();
    if(dnuMacro)
        return semanticAnalyzer->analyzeMessageSendNodeViaDNUMacro(node, dnuMacro);

    if(node->selector->isASTLiteralSymbolValue())
    {
        auto directSelectorValue = std::static_pointer_cast<ASTLiteralValueNode> (node->selector)->value;
        return semanticAnalyzer->recordSemanticErrorInNode(node, formatString("Failed to find matching message for selector {0} in receiver of type '{1}'.", {{directSelectorValue->printString(), printString()}}));
    }
    
    return semanticAnalyzer->recordSemanticErrorInNode(node, "");
}

ASTNodePtr Type::concretizeEphemeralCompileTimeObject(const ASTLiteralValueNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    if(supportsMessageAnalysisByLiteralValueReceivers())
        return node->value->concretizeEphemeralCompileTimeObject(node, semanticAnalyzer);
    return SuperType::concretizeEphemeralCompileTimeObject(node, semanticAnalyzer);
}

AnyValuePtr Type::lookupDoesNotUnderstandMacro()
{
    return nullptr;
}

void Type::addMacroMethodWithSelector(const AnyValuePtr &method, const AnyValuePtr &selector)
{
    if(!macroMethodDictionary)
        macroMethodDictionary = std::make_shared<MethodDictionary> ();
    macroMethodDictionary->addMethodWithSelector(method, selector);
}

void Type::addMethodWithSelector(const AnyValuePtr &method, const AnyValuePtr &selector)
{
    if(!methodDictionary)
        methodDictionary = std::make_shared<MethodDictionary> ();
    methodDictionary->addMethodWithSelector(method, selector);
}

void Type::addMacroFallbackMethodWithSelector(const AnyValuePtr &method, const AnyValuePtr &selector)
{
    if(!macroMethodDictionary)
        macroMethodDictionary = std::make_shared<MethodDictionary> ();
    macroMethodDictionary->addMethodWithSelector(method, selector);
}

TypePtr Type::getInstanceType()
{
    return shared_from_this();
}

TypePtr Type::getMetaType()
{
    return getType();
}

bool Type::isSubtypeOf(const TypePtr &otherType) const
{
    if(this == otherType.get())
        return true;
    
    auto supertype = getSupertype();
    if(supertype)
        return supertype->isSubtypeOf(otherType);

    return false;
}

PatternMatchingRank Type::rankToMatchType(const TypePtr &type)
{
    PatternMatchingRank rank = 0;
    for(auto currentType = type; currentType; currentType = currentType->getSupertype())
    {
        if(currentType.get() == this)
            return rank;

        ++rank;
    }

    return -1;
}

PatternMatchingRank Type::rankToMatchValue(const AnyValuePtr &value)
{
    return rankToMatchType(value->getType());
}

AnyValuePtr Type::runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
{
    auto method = lookupSelector(selector);
    if(method)
        return method->runWithArgumentsIn(selector, arguments, receiver);

    signalNewWithMessage<MessageNotUnderstood> ("Message " + selector->printString() + " is not understood by " + receiver->printString() + ".");
}

void Type::subtypesDo(const TypeIterationBlock &aBlock)
{
    for(auto &subtype : subtypes)
        aBlock(subtype);
}

void Type::allSubtypesDo(const TypeIterationBlock &aBlock)
{
    TypeIterationBlock recursiveBlock = [&](const TypePtr &type) {
        aBlock(type);
        type->subtypesDo(recursiveBlock);
    };

    subtypesDo(recursiveBlock);
}

void Type::withAllSubtypesDo(const TypeIterationBlock &aBlock)
{
    aBlock(shared_from_this());
    allSubtypesDo(aBlock);
}

void Type::ensureSemanticAnalysis()
{
    SuperType::ensureSemanticAnalysis();
    evaluateAllPendingCodeFragments();
}

void Type::evaluateAllPendingCodeFragments()
{
    evaluateAllPendingBodyBlockCodeFragments();
}

void Type::evaluateAllPendingBodyBlockCodeFragments()
{
    while(!pendingBodyBlockCodeFragments.empty())
    {
        auto fragmentsToProcess = pendingBodyBlockCodeFragments;
        pendingBodyBlockCodeFragments.clear();
        for(auto &fragment : fragmentsToProcess)
        {
            fragment->analyzeAndEvaluate();
        }
    }
}

void Type::enqueuePendingBodyBlockCodeFragment(const DeferredCompileTimeCodeFragmentPtr &codeFragment)
{
    pendingBodyBlockCodeFragments.push_back(codeFragment);
    enqueuePendingSemanticAnalysis();
}

void Type::recordChildProgramEntityDefinition(const ProgramEntityPtr &newChild)
{
    children.push_back(newChild);
}

void Type::bindSymbolWithVisibility(const AnyValuePtr &symbol, ProgramEntityVisibility visibility, const ProgramEntityPtr &binding)
{
    assert(symbol && !symbol->isAnonymousNameSymbol());
    if(bindings.find(symbol) != bindings.end())
        signalNewWithMessage<Error> ("Expected a new symbol binding.");

    bindings[symbol] = std::make_pair(visibility, binding);
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius