#include "sysmel/BootstrapEnvironment/ASTProgramEntityExtensionNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTSourcePosition.hpp"
#include "sysmel/BootstrapEnvironment/ASTVisitor.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapMethod.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<ASTProgramEntityExtensionNode> ASTProgramEntityExtensionNodeTypeRegistration;

bool ASTProgramEntityExtensionNode::isASTProgramEntityExtensionNode() const
{
    return true;
}

AnyValuePtr ASTProgramEntityExtensionNode::accept(const ASTVisitorPtr &visitor)
{
    return visitor->visitProgramEntityExtensionNode(shared_from_this());
}

SExpression ASTProgramEntityExtensionNode::asSExpression() const
{
    return SExpressionList{{SExpressionIdentifier{{"extension"}},
        sourcePosition->asSExpression(),
        body->asSExpression(),
    }};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius