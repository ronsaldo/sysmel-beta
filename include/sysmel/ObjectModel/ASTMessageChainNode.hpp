#ifndef SYSMEL_COMPILER_OBJECT_MODEL_AST_MESSAGE_CHAIN_NODE_HPP
#define SYSMEL_COMPILER_OBJECT_MODEL_AST_MESSAGE_CHAIN_NODE_HPP
#pragma once

#include "ASTNode.hpp"

namespace SysmelMoebius
{
namespace ObjectModel
{

class ASTMessageChainMessageNode;
typedef std::shared_ptr<ASTMessageChainMessageNode> ASTMessageChainMessageNodePtr;
typedef std::vector<ASTMessageChainMessageNodePtr> ASTMessageChainMessageNodePtrList;

/**
 * I am represent a message chain AST node.
 */
class ASTMessageChainNode : public SubtypeOf<ASTNode, ASTMessageChainNode>
{
public:
    static constexpr char const __typeName__[] = "ASTMessageChainNode";

    virtual bool isASTMessageChainNode() const override;
    virtual AnyValuePtr encodeAsSExpression() const override;

    ASTNodePtr receiver;
    ASTMessageChainMessageNodePtrList messages;
};

} // End of namespace ObjectModel
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_OBJECT_MODEL_AST_MESSAGE_CHAIN_NODE_HPP