/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LoggerDestine.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:53:27 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 22:52:29 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_DESTINE_HPP
# define LOGGER_DESTINE_HPP

# include <map>

# include "LoggerFormatter.hpp"

class LoggerDestine
{
	typedef std::map<LoggerLevel::type, LoggerFormatter *>	Formatters;
	LoggerFormatter		*_default_formatter;
	Formatters			_formatters;
	LoggerLevel::type	_min_level;
	LoggerLevel::type	_max_level;

	LoggerDestine(const LoggerDestine &other);
	LoggerDestine	&operator=(const LoggerDestine &other);

	protected:
		virtual void	doWrite(const utils::str &value) const = 0;

	public:
		LoggerDestine(LoggerLevel::type min_lvl, LoggerLevel::type max_lvl)
			: _default_formatter(new LoggerFormatterNoStyle()), _formatters(), _min_level(min_lvl), _max_level(max_lvl) {}
		LoggerDestine(LoggerLevel::type lvl)
			: _default_formatter(new LoggerFormatterNoStyle()), _formatters(),_min_level(lvl), _max_level(lvl) {}

		virtual ~LoggerDestine()
		{
			delete _default_formatter;
			for (std::map<LoggerLevel::type, LoggerFormatter *>::iterator it = _formatters.begin(); it != _formatters.end(); ++it)
					delete it->second;
		}

		void	addFormatter(LoggerLevel::type level, LoggerFormatter *formatter) { _formatters[level] = formatter; }

		bool	isInRange(LoggerLevel::type	lvl) const { return lvl >= _min_level && lvl <= _max_level; }

		void	write(const LoggerRecord &rec) const
		{
			if (isInRange(rec.level))
			{
				Formatters::const_iterator it =  _formatters.find(rec.level);
				if (it != _formatters.end())
					return doWrite(it->second->format(rec));
				doWrite(_default_formatter->format(rec));
			}
		}
};

#endif
