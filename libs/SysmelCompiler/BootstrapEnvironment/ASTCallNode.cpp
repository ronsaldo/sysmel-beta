#include "sysmel/BootstrapEnvironment/ASTCallNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTSourcePosition.hpp"
#include "sysmel/BootstrapEnvironment/ASTVisitor.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<ASTCallNode> ASTCallNodeTypeRegistration;

bool ASTCallNode::isASTCallNode() const
{
    return true;
}

AnyValuePtr ASTCallNode::accept(const ASTVisitorPtr &visitor)
{
    return visitor->visitCallNode(shared_from_this());
}

SExpression ASTCallNode::asSExpression() const
{
    SExpressionList argumentsSExpr;
    argumentsSExpr.elements.reserve(arguments.size());
    for(auto &arg : arguments)
        argumentsSExpr.elements.push_back(arg->asSExpression());

    return SExpressionList{{SExpressionIdentifier{{"call"}},
        sourcePosition->asSExpression(),
        function->asSExpression(),
        argumentsSExpr
    }};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius