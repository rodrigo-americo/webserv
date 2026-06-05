/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 20:05:44 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/22 20:42:15 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURE_TIME_HPP
# define STRUCTURE_TIME_HPP

# include <string>
# include <cstring>
# include <iostream>
# include <sstream>
# include <iomanip>


namespace structure
{
	class time
	{
		public:
			static const size_t	npos = std::string::npos;
			static constexpr double	eps = 1e-9;

		private:

			size_t		_us;
			size_t		_ms;
			size_t		_sec;
			size_t		_min;
			size_t		_hour;

			std::string	_format;
			std::string	_parse_error;

			static std::string	_verifyFormatIntegrity(const std::string &format, const std::string &target)
			{
				if (target.size() != format.size())
					return "Target string has diferent size than format string!";
				static const std::string	acceptedFormats = "umsMH";
				for (size_t i = 0; i < format.size(); i++)
				{
					bool	is_in_acceptedFormats = std::strchr(acceptedFormats.c_str(), format[i]);
					if (is_in_acceptedFormats && !std::isdigit(target[i]))
						return "Target in position " + std::to_string(i) + " '" + target[i] + "' is not a digit!";
					else if (!is_in_acceptedFormats && format[i] != target[i])
						return "Format in position " + std::to_string(i) + " is '" + format[i] + "', but target is different: '" + target[i] + "'";
				}
				return "";
			}

			static size_t	_parseInteger(const std::string &format, const std::string &target, char target_char, std::string &error)
			{
				size_t	start_idx = format.find_first_of(target_char);
				if (start_idx == npos) return 0;
				size_t				value;
				std::stringstream	ss;
				size_t	idx = start_idx;
				while (format[idx] && format[idx] == target_char)
					ss << target[idx++];
				ss >> value;
				if (ss.fail() || ss.peek() != std::stringstream::traits_type::eof())
				{
					error = "Section '";
					idx = start_idx;
					while (format[idx] && format[idx] == target_char)
						error += target[idx++];
					error += "' does not correspond with format: '";
					idx = start_idx;
					while (format[idx] && format[idx] == target_char)
						error += format[idx++];
					return npos;
				}
				return value;
			}

			static void	_formatInteger(const std::string &format, std::string &target, char target_char, size_t value)
			{
				size_t	first_idx = format.find_first_of(target_char);
				size_t	length = 0;
				while (format[first_idx + length] && format[first_idx + length] == target_char)
					length++;
				std::stringstream	ss;
				ss << std::setfill('0') << std::setw(length) << value;
				std::string	result = ss.str();
				for (size_t i = 0; i < length; i++)
					target[first_idx + i] = result[i];
			}

		private:
			time(size_t us, size_t ms, size_t sec, size_t min, size_t hour): _us(us), _ms(ms), _sec(sec), _min(min), _hour(hour), _format(), _parse_error() {};
			time(std::string parse_error): _us(0), _ms(0), _sec(0), _min(0), _hour(0), _format(), _parse_error(std::move(parse_error)) {};

		public:
			time(): _us(0), _ms(0), _sec(0), _min(0), _hour(0), _format(), _parse_error() {};
			time(const time &other) = default;
			time(time &&other) = default;
			~time() = default;

			time	&operator=(const time &other) = default;
			time	&operator=(time &&other) = default;

			double	inSeconds() const
			{
				double	seconds = (double)_us * 1e-6;
				seconds += (double)_ms * 1e-3;
				seconds += (double)_sec;
				seconds += (double)_min * 60;
				seconds += (double)_hour * 3600;
				return seconds;
			}

			double	inMicroSeconds() const
			{
				return inSeconds() * 1e6;
			}

			double	inMilliSeconds() const
			{
				return inSeconds() * 1e3;
			}

			double	inMinutes() const
			{
				return inSeconds() / 60;
			}

			double	inHours() const
			{
				return inSeconds() / 3600;
			}

			size_t				getMicroSeconds() const { return _us; };
			size_t				getMilliSeconds() const { return _ms; };
			size_t				getSeconds() const { return _sec; };
			size_t				getMinutes() const { return _min; };
			size_t				getHours() const { return _hour; };
			const std::string	&getParseError() const { return _parse_error; }

			operator bool() const
			{
				bool	is_falsy = _us == 0
						&& _ms == 0
						&& _sec == 0
						&& _min == 0
						&& _hour == 0;
				return	!is_falsy;
			}

			bool	operator>(const time &other) const
			{
				return	(inSeconds() - other.inSeconds()) > eps;
			}

			bool	operator<(const time &other) const
			{
				return	(other.inSeconds() - inSeconds()) > eps;
			}

			bool	operator==(const time &other) const
			{
				return _us == other._us
						&& _ms == other._ms
						&& _sec == other._sec
						&& _min == other._min
						&& _hour == other._hour;
			}

			bool	operator!=(const time &other) const
			{
				return	!(*this == other);
			}

			bool	operator>=(const time &other) const
			{
				if (*this == other) return true;
				return	*this > other;
			}

			bool	operator<=(const time &other) const
			{
				if (*this == other) return true;
				return	*this < other;
			}

			/**
			 * Parse given `str` to `time` following the format:
			 * - `u`: microseconds;
			 * - `m`: milliseconds;
			 * - `s`: seconds;
			 * - `M`: minutes;
			 * - `H`: hours;
			 *
			 * ## Example:
			 *
			 * ---
			 *
			 * ### ex01:
			 *
			 * given str = `23h25.556`;
			 *
			 * corresponding format = `HHhMM.mmm`;
			 *
			 * ---
			 *
			 * ### ex02:
			 *
			 * given str = `23:35:12:256.7998`;
			 *
			 * corresponding format = `HH:MM:ss:mmm.uuuu`;
			 *
			 * ---
			 * @returns time
			 */
			static time	parse(const std::string &str, const std::string &format)
			{
				std::string	error = _verifyFormatIntegrity(format, str);
				if (error.size()) return time(error);
				size_t us = _parseInteger(format, str, 'u', error);
				if (error.size()) return time(error);
				size_t ms = _parseInteger(format, str, 'm', error);
				if (error.size()) return time(error);
				size_t sec = _parseInteger(format, str, 's', error);
				if (error.size()) return time(error);
				size_t min = _parseInteger(format, str, 'M', error);
				if (error.size()) return time(error);
				size_t hour = _parseInteger(format, str, 'H', error);
				if (error.size()) return time(error);
				return time(us, ms, sec, min, hour);
			}

			/**
			 * Format `time` following the format informed by the  given `str`:
			 * - `u`: microseconds;
			 * - `m`: milliseconds;
			 * - `s`: seconds;
			 * - `M`: minutes;
			 * - `H`: hours;
			 *
			 * ## Example:
			 *
			 * ---
			 *
			 * ### ex01:
			 *
			 * given str (format) = `HHhMM.mmm`;
			 *
			 * corresponding format = `23h25.556`;
			 *
			 *
			 * ---
			 *
			 * ### ex02:
			 *
			 * given str (format) = `HHHHhMMM.mmmm`;
			 *
			 * corresponding format = `0023h025.0556`;
			 *
			 *
			 * ---
			 *
			 * ### ex03:
			 *
			 * given str = `HH:MM:ss:mmm.uuuu`;
			 *
			 * corresponding format = `23:35:12:256.7998`;
			 *
			 *
			 * ---
			 * @returns Formatted `time`
			 */
			std::string	format(std::string _format_str = "HHhMM") const
			{
				std::string	final_format = _format.empty() ? _format_str : _format;
				std::string	result = final_format;
				_formatInteger(final_format, result, 'u', _us);
				_formatInteger(final_format, result, 'm', _ms);
				_formatInteger(final_format, result, 's', _sec);
				_formatInteger(final_format, result, 'M', _min);
				_formatInteger(final_format, result, 'H', _hour);
				return result;
			}

			void	setFormat(std::string _format_str) { _format = _format_str; }
	};

	std::ostream	&operator<<(std::ostream &os, const time &_time)
	{
		os << _time.format();
		return os;
	}
}

#endif
