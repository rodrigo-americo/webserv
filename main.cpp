#include <iostream>
#include "parser/parser/ParserBuilder.hpp"
#include "config/ConfigBuilderVisitor.hpp"
#include "WebServer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    ParserAst ast = ParserBuilder::defaultBuilder()
        .withFile(argv[1])
        .build();

    ast.validateStructure();
    if (ast.hasError())
    {
        ast.printErrors();
        return 1;
    }

    ConfigBuilderVisitor visitor;
    ast.applyVisitor(visitor);

    WebServerConfig* config = visitor.getResult();
    if (!config)
    {
        std::cerr << "Erro: configuração inválida" << std::endl;
        return 1;
    }

    if (config->hasErrors())
    {
        std::cerr << config->formatErrors();
        delete config;
        return 1;
    }
	WebServer webServer;
	webServer.start(config);
    delete config;
    return 0;
}
