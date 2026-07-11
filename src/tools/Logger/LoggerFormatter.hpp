/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LoggerFormatter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:50:24 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 22:54:38 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_FORMATTER_HPP
# define LOGGER_FORMATTER_HPP

# include "utils.hpp"
# include "text.hpp"
# include "LoggerRecord.hpp"

struct LoggerFormatter
{
	virtual ~LoggerFormatter() {};
	virtual utils::str	format(const LoggerRecord &) = 0;
};

struct LoggerFormatterNoStyle: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		std::stringstream	ss;
		ss << "[ " + rec.time.formatLog().string() + " ]"
			<< " " << (rec.file + ":" + utils::to_string(rec.line)).string()
			<< " " << rec.msg.string() << "\n";
		return ss.str();
	}
};

struct LoggerFormatterTrace: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::color(230, 230, 230);
		std::stringstream	ss;
		ss << trace_style.apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.italic().apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

struct LoggerFormatterDebug: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::color(80, 250, 123);
		std::stringstream	ss;
		ss << text::style(trace_style).apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

struct LoggerFormatterInfo: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::color(139, 233, 253);
		std::stringstream	ss;
		ss << text::style(trace_style).apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

struct LoggerFormatterWarn: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::color(241, 250, 140);
		std::stringstream	ss;
		ss << text::style(trace_style).apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

struct LoggerFormatterError: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::color(255, 85, 85);
		std::stringstream	ss;
		ss << text::style(trace_style).apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

struct LoggerFormatterFatal: public LoggerFormatter
{
	utils::str	format(const LoggerRecord &rec)
	{
		text::style trace_style = text::bold().color(255, 30, 30);
		std::stringstream	ss;
		ss << trace_style.apply("[ " + rec.time.formatLog().string() + " ]")
			<< " " << text::style(trace_style).underline().apply((rec.file + ":" + utils::to_string(rec.line)).string())
			<< " " << trace_style.apply(rec.msg.string()) << "\n";
		return ss.str();
	}
};

#endif
