#include "sysmel/BootstrapEnvironment/ASTMessageChainMessageNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTSourcePosition.hpp"
#include "sysmel/BootstrapEnvironment/ASTVisitor.hpp"
#include "sysmel/BootstrapEnvironment/ASTMessageSendNode.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapMethod.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<ASTMessageChainMessageNode> ASTMessageChainMessageNodeTypeRegistration;

bool ASTMessageChainMessageNode::isASTMessageChainMessageNode() const
{
    return true;
}

AnyValuePtr ASTMessageChainMessageNode::accept(const ASTVisitorPtr &visitor)
{
    return visitor->visitMessageChainMessageNode(shared_from_this());
}

SExpression ASTMessageChainMessageNode::asSExpression() const
{
    SExpressionList argumentsSExpression;
    argumentsSExpression.elements.reserve(arguments.size());
    for(const auto &arg : arguments )
        argumentsSExpression.elements.push_back(arg->asSExpression());

    return SExpressionList{{SExpressionIdentifier{{"message"}},
        sourcePosition->asSExpression(),
        selector->asSExpression(),
        argumentsSExpression,
    }};
}

ASTMessageSendNodePtr ASTMessageChainMessageNode::asMessageSendNodeWithReceiver(const ASTNodePtr &newReceiver)
{
    auto result = std::make_shared<ASTMessageSendNode> ();
    result->sourcePosition = sourcePosition;
    result->receiver = newReceiver;
    result->selector = selector;
    result->arguments = arguments;
    return result;
}

void ASTMessageChainMessageNode::childrenDo(const ASTIterationBlock &aBlock)
{
    SuperType::childrenDo(aBlock);
    if(selector) aBlock(selector);
    for(auto &arg : arguments)
        aBlock(arg);
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius