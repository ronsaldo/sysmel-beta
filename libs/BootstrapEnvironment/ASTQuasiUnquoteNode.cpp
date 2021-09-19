#include "sysmel/BootstrapEnvironment/ASTQuasiUnquoteNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTSourcePosition.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapMethod.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<ASTQuasiUnquoteNode> ASTQuasiUnquoteNodeTypeRegistration;

bool ASTQuasiUnquoteNode::isASTQuasiUnquoteNode() const
{
    return true;
}

SExpression ASTQuasiUnquoteNode::asSExpression() const
{
    return SExpressionList{{SExpressionIdentifier{{"quasiUnquote"}},
        sourcePosition->asSExpression(),
        expression->asSExpression()
    }};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius