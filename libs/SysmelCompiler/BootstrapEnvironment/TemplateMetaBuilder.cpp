#include "sysmel/BootstrapEnvironment/TemplateMetaBuilder.hpp"
#include "sysmel/BootstrapEnvironment/ASTCallNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTMessageSendNode.hpp"
#include "sysmel/BootstrapEnvironment/ASTTemplateNode.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<TemplateMetaBuilder> variableMetaBuilderTypeRegistration;

ASTNodePtr TemplateMetaBuilder::analyzeCallNode(const ASTCallNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    if(!hasParsedCallArguments)
    {
        argumentsNode.reserve(node->arguments.size());
        for(const auto &arg : node->arguments)
            argumentsNode.push_back(arg->parseAsArgumentNodeWith(semanticAnalyzer));
        return node->function;
    }

    return SuperType::analyzeCallNode(node, semanticAnalyzer);
}
    
ASTNodePtr TemplateMetaBuilder::analyzeMessageSendNodeWithSelector(const std::string &selector, const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer)
{
    if(node->arguments.size() == 1)
    {
        if(selector == ":=")
        {
            bodyNode = node->arguments[0];
            return concretizeMetaBuilderAndAnalyzeWith(semanticAnalyzer);
        }
    }

    return SuperType::analyzeMessageSendNodeWithSelector(selector, node, semanticAnalyzer);
}

ASTNodePtr TemplateMetaBuilder::concretizeMetaBuilder()
{
    auto result = std::make_shared<ASTTemplateNode> ();
    result->sourcePosition = instanceContext->concreteSourcePosition();
    result->visibility = instanceContext->programEntityVisibility;

    result->name = nameNode;
    result->arguments = argumentsNode;
    result->body = bodyNode;
    return result;
}
} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius