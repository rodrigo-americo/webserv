/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 12:51:32 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 23:00:58 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <algorithm>
# include <map>
# include <sstream>

# include "singleton.hpp"
# include "utils.hpp"
# include "text.hpp"
# include "LoggerDestine.hpp"

class Logger: public patterns::singleton<Logger>
{
	public:
		typedef LoggerLevel::type	level;

	private:
		friend class patterns::singleton<Logger>;
		typedef std::vector<LoggerDestine*>	destines;
		destines	_destines;
		level		_min_level;

		Logger() {}
		Logger(const Logger&);
		Logger	&operator=(const Logger&);


	public:
		~Logger()
		{
			for (size_t i = 0; i < _destines.size(); i++)
				delete _destines[i];
		}

		static void	levelTrace() { Logger::getInstance().setMinLevel(LoggerLevel::TRACE); }
		static void	levelDebug() { Logger::getInstance().setMinLevel(LoggerLevel::DEBUG); }
		static void	levelInfo() { Logger::getInstance().setMinLevel(LoggerLevel::INFO); }
		static void	levelWarn() { Logger::getInstance().setMinLevel(LoggerLevel::WARN); }
		static void	levelError() { Logger::getInstance().setMinLevel(LoggerLevel::ERROR); }
		static void	levelFatal() { Logger::getInstance().setMinLevel(LoggerLevel::FATAL); }

		void	setMinLevel(level lvl) { _min_level = lvl; }

		void	addDestine(LoggerDestine *destine)
		{
			if (std::binary_search(_destines.begin(), _destines.end(), destine))
				return delete destine;
			destines::iterator	it = std::lower_bound(_destines.begin(), _destines.end(), destine);
			_destines.insert(it, destine);
		}

		void log(level lvl, const utils::str &msg, const utils::str &file, size_t line, const utils::time &time)
		{
			if (lvl < _min_level) return;
			LoggerRecord	rec(lvl, msg, file, line, time);
			for (size_t i = 0; i < _destines.size(); i++)
				_destines[i]->write(rec);
		}
};

# define LOG_TRACE(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::TRACE, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)
# define LOG_DEBUG(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::DEBUG, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)
# define LOG_INFO(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::INFO, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)
# define LOG_WARN(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::WARN, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)
# define LOG_ERROR(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::ERROR, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)
# define LOG_FATAL(msg) do { std::stringstream _ss; _ss << msg; Logger::getInstance().log(LoggerLevel::FATAL, _ss.str(), __FILE__, __LINE__, utils::time()); } while(0)


#endif
