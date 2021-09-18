#include "sysmel/Compiler/Sysmel/SysmelLanguageSupport.hpp"
#include "sysmel/ObjectModel/AnyValue.hpp"
#include "sysmel/ObjectModel/ASTNode.hpp"
#include "sysmel/ObjectModel/Wrappers.hpp"
#include "sysmel/ObjectModel/RuntimeContext.hpp"
#include "sysmel/ObjectModel/BootstrapModule.hpp"
#include "sysmel/ObjectModel/BootstrapTypeRegistration.hpp"
#include "sysmel/ObjectModel/Type.hpp"
#include <fstream>
#include <iostream>
/*
namespace TestEvaluator
{
using namespace SysmelMoebius::Compiler::Sysmel;
using namespace SysmelMoebius::ObjectModel;


}
*/

using namespace SysmelMoebius::ObjectModel;

void parseString(const std::string &sourceString, const std::string &sourceName)
{
    auto language = SysmelMoebius::ObjectModel::SysmelLanguageSupport::uniqueInstance();
    auto ast = language->parseSourceStringNamed(sourceString, sourceName);

    std::cout << ast->printString() << std::endl;
}

void evalString(const std::string &sourceString, const std::string &sourceName)
{
    auto language = SysmelMoebius::ObjectModel::SysmelLanguageSupport::uniqueInstance();
    auto result = language->evaluateSourceStringNamed(sourceString, sourceName);

/*
    using namespace SysmelMoebius::Compiler::Sysmel;
    using namespace SysmelMoebius::ObjectModel;
    auto ast = parseString(sourceString, sourceName);
    bool hasError = false;
    validateASTParseErrors(ast, [&](ASTParseErrorNode &parseErrorNode) {
        std::cout << parseErrorNode.sourcePosition << ": " << parseErrorNode.errorMessage << '\n';
        std::cout << parseErrorNode.sourcePosition.content() << std::endl;
        hasError = true;
    });

    if(hasError)
        return;

*/

    std::cout << result->printString() << std::endl;

}

void evalFileNamed(const std::string &fileName)
{
    auto language = SysmelMoebius::ObjectModel::SysmelLanguageSupport::uniqueInstance();
    auto result = language->evaluateFileNamed(fileName);
    std::cout << result->printString() << std::endl;
}

void dumpBootstrapEnvironment()
{
    auto bootstrapModule = RuntimeContext::getActive()->getBootstrapModule();
    AnyValue::__staticType__()->withAllSubtypesDo([&](const TypePtr &type) {
        std::cout << type->asString() << std::endl;
        auto superType = type->getSupertype();
        if(superType)
            std::cout << "    supertype: " << superType->asString() << std::endl;
    });
}

int main(int argc, const char *argv[])
{
    int exitCode = 0;

    RuntimeContext::create()->activeDuring([&](){
        for(int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if(arg == "-eval")
            {
                evalString(argv[++i], "<command line arg>");
            }
            else if(arg == "-parse-string")
            {
                parseString(argv[++i], "<command line arg>");
            }
            else if(arg == "-dump-bootstrap-env")
            {
                dumpBootstrapEnvironment();
                return;
            }
            else if(!arg.empty() && arg[0] != '-')
            {
                evalFileNamed(argv[i]);
            }
            else
            {
                std::cerr << "Unsupported command line argument " << arg << std::endl;
                exitCode = 1;
                return;
            }
        }
    });

    return exitCode;
}
