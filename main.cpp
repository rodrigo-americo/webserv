#include <iostream>
#include "parser/parser/ParserBuilder.hpp"
#include "config/ConfigBuilderVisitor.hpp"
#include "signal.hpp"
#include "WebServer.hpp"
#include "Logger.hpp"
# include "LoggerDestineFile.hpp"
# include "LoggerDestineOstream.hpp"

int main(int argc, char* argv[])
{
    Logger	&logger = Logger::getInstance();
    LoggerDestineFile *cgi_log = new LoggerDestineFile("cgi.log", LoggerLevel::TRACE, LoggerLevel::FATAL);
    logger.addDestine(cgi_log);

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    setup_signal_handlers();
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
    webServer.clean_up();
    return 0;
}
