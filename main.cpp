#include <iostream>
#include "parser/parser/ParserBuilder.hpp"
#include "config/ConfigBuilderVisitor.hpp"
#include "signal.hpp"
#include "WebServer.hpp"
#include "ConfigInheritanceResolver.hpp"
#include "Logger.hpp"
# include "LoggerDestineFile.hpp"
# include "LoggerDestineOstream.hpp"

void	settupLogger();

int main(int argc, char* argv[])
{
	settupLogger();


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

    WebServerConfig &config = WebServerConfig::getInstance();
    ConfigInheritanceResolver::resolve();

    if (config.hasErrors())
    {
        std::cerr << config.formatErrors();
        return 1;
    }
	WebServer webServer;
	webServer.start();
    webServer.clean_up();
    return 0;
}

void	settupLogger()
{
	Logger	&logger = Logger::getInstance();
    LoggerDestineFile *traces = new LoggerDestineFile("log_traces.log", LoggerLevel::TRACE, LoggerLevel::DEBUG);
	LoggerDestineFile *info = new LoggerDestineFile("log_info.log", LoggerLevel::INFO, LoggerLevel::WARN);
	LoggerDestineFile *error = new LoggerDestineFile("log_error.log", LoggerLevel::ERROR, LoggerLevel::FATAL);
    logger.addDestine(traces);
	logger.addDestine(info);
	logger.addDestine(error);
}
