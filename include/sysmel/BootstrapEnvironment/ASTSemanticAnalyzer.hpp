#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_SEMANTIC_ANALYZER_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_SEMANTIC_ANALYZER_HPP
#pragma once

#include "ASTVisitor.hpp"
#include "TypeInferenceMode.hpp"
#include <functional>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(ASTAnalysisEnvironment);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(ResultTypeInferenceSlot);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(MacroInvocationContext);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(Error);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(CompilationError);

typedef std::function<ASTNodePtr ()> ASTNodeSemanticAnalysisBlock;

/**
 * I am the interface for all of the language independent AST nodes.
 */
class ASTSemanticAnalyzer : public SubtypeOf<ASTVisitor, ASTSemanticAnalyzer>
{
public:
    static constexpr char const __typeName__[] = "ASTSemanticAnalyzer";

    ASTNodePtr recordSemanticErrorInNode(const ASTNodePtr &errorNode, const std::string &message);
    ASTNodePtr recordCompileTimeEvaluationErrorInNode(const ASTNodePtr &errorNode, const ErrorPtr &evaluationError);

    ASTNodePtr analyzeNodeIfNeededWithTypeInference(const ASTNodePtr &node, const ResultTypeInferenceSlotPtr &typeInferenceSlot, bool concretizeEphemeralObjects = false);
    ASTNodePtr analyzeNodeIfNeededWithExpectedType(const ASTNodePtr &node, const TypePtr &expectedType, bool concretizeEphemeralObjects = false);
    ASTNodePtr analyzeNodeIfNeededWithExpectedTypeSet(const ASTNodePtr &node, const TypePtrList &expectedTypeSet);
    ASTNodePtr analyzeNodeIfNeededWithAutoType(const ASTNodePtr &node);
    ASTNodePtr analyzeNodeIfNeededWithAutoTypeInferenceMode(const ASTNodePtr &node, TypeInferenceMode mode, bool isMutable);
    ASTNodePtr analyzeNodeIfNeededWithCurrentExpectedType(const ASTNodePtr &node, bool concretizeEphemeralObjects = false);

    AnyValuePtr adaptNodeAsMacroArgumentOfType(const ASTNodePtr &node, const TypePtr &expectedType);

    ASTNodePtr withEnvironmentDoAnalysis(const ASTAnalysisEnvironmentPtr &newEnvironment, const ASTNodeSemanticAnalysisBlock &aBlock);

    MacroInvocationContextPtr makeMacroInvocationContextFor(const ASTIdentifierReferenceNodePtr &node);
    MacroInvocationContextPtr makeMacroInvocationContextFor(const ASTMessageSendNodePtr &node);

    PatternMatchingRank rankForMatchingTypeWithValueOfType(const TypePtr &expectedType, const TypePtr &valueType);
    PatternMatchingRank rankForMatchingTypeWithNode(const TypePtr &expectedType, const ASTNodePtr &node);

    ASTNodePtr analyzeDynamicCompileTimeMessageSendNode(const ASTMessageSendNodePtr &node);
    ASTNodePtr analyzeMessageSendNodeViaDNUMacro(const ASTMessageSendNodePtr &node, const AnyValuePtr &dnuMacro);
    ASTNodePtr optimizeAnalyzedMessageSend(const ASTMessageSendNodePtr &node);
    
    ASTNodePtr analyzeCallNodeByConvertingToMessageSend(const ASTCallNodePtr &node);

    AnyValuePtr evaluateNameSymbolValue(const ASTNodePtr &node);

    ASTNodePtr analyzeArgumentDefinitionNodeWithExpectedType(const ASTArgumentDefinitionNodePtr &node, const TypePtr &expectedType);
    ASTNodePtr analyzeCallNodeWithFunctionalType(const ASTCallNodePtr &node, const FunctionalTypePtr &functionType);

    virtual AnyValuePtr visitArgumentDefinitionNode(const ASTArgumentDefinitionNodePtr &node) override;
    virtual AnyValuePtr visitCleanUpScopeNode(const ASTCleanUpScopeNodePtr &node) override;
    virtual AnyValuePtr visitClosureNode(const ASTClosureNodePtr &node) override;
    virtual AnyValuePtr visitIdentifierReferenceNode(const ASTIdentifierReferenceNodePtr &node) override;
    virtual AnyValuePtr visitCallNode(const ASTCallNodePtr &node) override;
    virtual AnyValuePtr visitLexicalScopeNode(const ASTLexicalScopeNodePtr &node) override;
    virtual AnyValuePtr visitLiteralValueNode(const ASTLiteralValueNodePtr &node) override;
    virtual AnyValuePtr visitMakeAssociationNode(const ASTMakeAssociationNodePtr &node) override;
    virtual AnyValuePtr visitMakeDictionaryNode(const ASTMakeDictionaryNodePtr &node) override;
    virtual AnyValuePtr visitMakeLiteralArrayNode(const ASTMakeLiteralArrayNodePtr &node) override;
    virtual AnyValuePtr visitMakeTupleNode(const ASTMakeTupleNodePtr &node) override;
    virtual AnyValuePtr visitMessageChainNode(const ASTMessageChainNodePtr &node) override;
    virtual AnyValuePtr visitMessageSendNode(const ASTMessageSendNodePtr &node) override;
    virtual AnyValuePtr visitParseErrorNode(const ASTParseErrorNodePtr &node) override;
    virtual AnyValuePtr visitPragmaNode(const ASTPragmaNodePtr &node) override;
    virtual AnyValuePtr visitQuasiQuoteNode(const ASTQuasiQuoteNodePtr &node) override;
    virtual AnyValuePtr visitQuasiUnquoteNode(const ASTQuasiUnquoteNodePtr &node) override;
    virtual AnyValuePtr visitQuoteNode(const ASTQuoteNodePtr &node) override;
    virtual AnyValuePtr visitSequenceNode(const ASTSequenceNodePtr &node) override;
    virtual AnyValuePtr visitSpliceNode(const ASTSpliceNodePtr &node) override;

    virtual AnyValuePtr visitLocalVariableNode(const ASTLocalVariableNodePtr &node) override;
    virtual AnyValuePtr visitGlobalVariableNode(const ASTGlobalVariableNodePtr &node) override;
    virtual AnyValuePtr visitFieldVariableNode(const ASTFieldVariableNodePtr &node) override;

    virtual AnyValuePtr visitVariableAccessNode(const ASTVariableAccessNodePtr &node) override;

    virtual AnyValuePtr visitFunctionNode(const ASTFunctionNodePtr &node) override;
    virtual AnyValuePtr visitMethodNode(const ASTMethodNodePtr &node) override;

    virtual AnyValuePtr visitNamespaceNode(const ASTNamespaceNodePtr &node) override;

    virtual AnyValuePtr visitEnumNode(const ASTEnumNodePtr &node) override;
    virtual AnyValuePtr visitClassNode(const ASTClassNodePtr &node) override;
    virtual AnyValuePtr visitStructNode(const ASTStructNodePtr &node) override;
    virtual AnyValuePtr visitUnionNode(const ASTUnionNodePtr &node) override;

    ASTNodePtr addImplicitCastTo(const ASTNodePtr &node, const TypePtr &targetType);
    ASTNodePtr addImplicitCastToOneOf(const ASTNodePtr &node, const TypePtrList &expectedTypeSet);

    AnyValuePtr evaluateInCompileTime(const ASTNodePtr &node);
    ASTNodePtr evaluateLiteralExpressionInCompileTime(const ASTNodePtr &node);
    ASTNodePtr evaluateTypeExpression(const ASTNodePtr &node);

    ASTNodePtr guardCompileTimeEvaluationForNode(const ASTNodePtr &node, const ASTNodeSemanticAnalysisBlock &aBlock);

    bool isNameReserved(const AnyValuePtr &name);

    ASTAnalysisEnvironmentPtr environment;
    ResultTypeInferenceSlotPtr currentExpectedType;

    CompilationErrorPtr makeCompilationError();
    ASTErrorNodePtrList recordedErrors;

};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_SEMANTIC_ANALYZER_HPP