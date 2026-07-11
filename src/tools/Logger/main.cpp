/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 21:57:38 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 23:12:48 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Logger.hpp"
# include "LoggerDestineFile.hpp"
# include "LoggerDestineOstream.hpp"

void	testAllLogs();
void	testAllLogsFatal();
void	testAllLogsInfo();
void	testAllLogsWarn();
int main()
{
	Logger	&logger = Logger::getInstance();
	logger.addDestine(new LoggerDestineFile("error_fatal.log", LoggerLevel::ERROR, LoggerLevel::FATAL));
	LoggerDestineOstream	*fatal_os = new LoggerDestineOstream(std::cerr, LoggerLevel::ERROR, LoggerLevel::FATAL);
	fatal_os->addFormatter(LoggerLevel::ERROR, new LoggerFormatterError());
	fatal_os->addFormatter(LoggerLevel::FATAL, new LoggerFormatterFatal());
	logger.addDestine(fatal_os);
	LoggerDestineOstream	*trace_os = new LoggerDestineOstream(std::cout, LoggerLevel::TRACE, LoggerLevel::WARN);
	trace_os->addFormatter(LoggerLevel::TRACE, new LoggerFormatterTrace());
	trace_os->addFormatter(LoggerLevel::DEBUG, new LoggerFormatterDebug());
	trace_os->addFormatter(LoggerLevel::INFO, new LoggerFormatterInfo());
	trace_os->addFormatter(LoggerLevel::WARN, new LoggerFormatterWarn());
	logger.addDestine(trace_os);

	testAllLogs();
	testAllLogsFatal();
	testAllLogsInfo();
	testAllLogsWarn();
}

void	testAllLogs()
{
	std::cout << "\nTESTTING ALL LOGS \n\n";
	LOG_TRACE("test de trace");
	LOG_DEBUG("debugging...");
	LOG_INFO("info...");
	LOG_WARN("warn...");
	LOG_ERROR("error...");
	LOG_FATAL("FATAL error...");
}

void	testAllLogsFatal()
{
	std::cout << "\nTESTTING ALL LOGS -> setting min_level as FATAL \n\n";

	Logger::levelFatal();

	LOG_TRACE("test de trace");
	LOG_DEBUG("debugging...");
	LOG_INFO("info...");
	LOG_WARN("warn...");
	LOG_ERROR("error...");
	LOG_FATAL("FATAL error...");
}

void	testAllLogsInfo()
{
	std::cout << "\nTESTTING ALL LOGS -> setting min_level as INFO \n\n";

	Logger::levelInfo();

	LOG_TRACE("test de trace");
	LOG_DEBUG("debugging...");
	LOG_INFO("info...");
	LOG_WARN("warn...");
	LOG_ERROR("error...");
	LOG_FATAL("FATAL error...");
}


void	testAllLogsWarn()
{
	std::cout << "\nTESTTING ALL LOGS -> setting min_level as WARN \n\n";

	Logger::levelWarn();

	LOG_TRACE("test de trace");
	LOG_DEBUG("debugging...");
	LOG_INFO("info...");
	LOG_WARN("warn...");
	LOG_ERROR("error...");
	LOG_FATAL("FATAL error...");
}
