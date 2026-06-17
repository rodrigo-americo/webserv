/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunofer <brunofer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 13:50:25 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/17 12:40:46 by brunofer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_TIME_HPP
# define UTILS_TIME_HPP

# include <iostream>
# include <ctime>

# include "str.hpp"

namespace utils
{

	static const char *time_month_str[] =
	{
		"JANUARY",
		"FEBRUARY",
		"MARCH",
		"APRIL",
		"MAY",
		"JUNE",
		"JULY",
		"AUGUST",
		"SEPTEMBER",
		"OCTOBER",
		"NOVEMBER",
		"DECEMBER"
	};
	// Dia do Mês
	struct time_month
	{
		enum type
		{
			JANUARY,
			FEBRUARY,
			MARCH,
			APRIL,
			MAY,
			JUNE,
			JULY,
			AUGUST,
			SEPTEMBER,
			OCTOBER,
			NOVEMBER,
			DECEMBER
		};
	};

	static const char *time_week_day_str[] =
	{
		"SUNDAY",
		"MONDAY",
		"TUESDAY",
		"WEDNESDAY",
		"THURSDAY",
		"FRIDAY",
		"SATURDAY",
	};
	// Dia da Semana
	struct time_week_day
	{
		enum type
		{
			SUNDAY,
			MONDAY,
			TUESDAY,
			WEDNESDAY,
			THURSDAY,
			FRIDAY,
			SATURDAY,
		};
	};


	class time
	{
		public:
			typedef	time_month::type	month; // Dia do Mês
			typedef	time_week_day::type	week_day; // Dia da Semana
	private:
		std::time_t	_timestamp;
		struct tm	_time;
		bool		_timestamp_dirty;
		bool		_time_dirty;
		utils::str	_format;

		void	_timestampDirty() { _timestamp_dirty = true; }
		void	_timeDirty() { _time_dirty = true; }

		void	_checkTime()
		{
			if (_time_dirty)
			{
				_time = *::std::localtime(&_timestamp);
				_time_dirty = false;
			}
		}

		void	_checkTimestamp()
		{
			if (_timestamp_dirty)
			{
				_timestamp = std::mktime(&_time);
				_timestamp_dirty = false;
			}
		}

	public:
		time()
			: _timestamp(::std::time(NULL)), _time(*::std::localtime(&_timestamp)),
			_timestamp_dirty(false), _time_dirty(false), _format("%Y-%m-%d %H:%M:%S") {}
		~time() { }

		int		sec() const { return _time.tm_sec; }													// Seconds. [0-60] (1 leap second)
		time	&sec(int sec) { _checkTime(); _time.tm_sec = sec; _timestampDirty(); _checkTimestamp(); return *this; }	// Seconds. [0-60] (1 leap second)

		int		min() const { return _time.tm_min; }													// Minutes. [0-59]
		time	&min(int min) { _checkTime(); _time.tm_min = min; _timestampDirty(); _checkTimestamp(); return *this; }	// Minutes. [0-59]

		int		hour() const { return _time.tm_hour; }														// Hours. [0-23]
		time	&hour(int hour) { _checkTime(); _time.tm_hour = hour; _timestampDirty(); _checkTimestamp(); return *this; }	// Hours. [0-23]

		int		mday() const { return _time.tm_mday; }														// Day. [1-31]
		time	&mday(int mday) { _checkTime(); _time.tm_mday = mday; _timestampDirty(); _checkTimestamp(); return *this; }	// Day. [1-31]

		month	mon() const { return month(_time.tm_mon); }														// Month. [0-11]
		time	&mon(month mon) { _checkTime(); _time.tm_mon = mon; _timestampDirty(); _checkTimestamp(); return *this; }			// Month. [0-11]

		int		year() const { return _time.tm_year + 1900; }													// Year.
		time	&year(int year) { _checkTime(); _time.tm_year = year - 1900; _timestampDirty(); _checkTimestamp(); return *this; }	// Year.

		week_day	wday() const { return week_day(_time.tm_wday); }													// Day of week. [0-6]
		// Day of week. [0-6]
		time		&wday(week_day wday)
		{

			_checkTime();
			int	delta = wday - _time.tm_wday;
			_time.tm_mday += delta;
			_timestampDirty();
			_checkTimestamp();
			return *this;
		}

		int		yday() const { return _time.tm_yday; }														// Days in year.[0-365]
		// Days in year.[0-365]
		time	&yday(int yday)
		{
			if (yday < 0 || yday > 365) return *this;
			_checkTime();
			int	delta = yday - _time.tm_yday;
			mday(mday() + delta);
			_timestampDirty();
			_checkTimestamp();
			return *this;
		}

		int		isdst() const { return _time.tm_isdst; }														// DST. [-1/0/1]
		time	&isdst(int isdst) { _checkTime(); _time.tm_isdst = isdst; _timestampDirty(); _checkTimestamp(); return *this; }	// DST. [-1/0/1]

		time	&normalize() { _checkTimestamp(); _checkTime(); return *this; } // update internal status. Must be done before any operation, like `-`, `<`, `==`, etc.

		/**
		 *  set the default format (cannot be empty)
		 */
		time	&setFormat(const utils::str &__format)
		{
			utils::str	copy = __format;
			if (copy.trim().empty()) return *this;
			_format = __format;
			return *this;
		}

		utils::str	format(const utils::str &__format = "") const //
		{
			const	unsigned short	buffer_size = 255;
			char	buff[buffer_size];
			if (__format.empty())
			{
				std::strftime(buff, buffer_size, _format.c_str(), &_time);
				return buff;
			}
			std::strftime(buff, buffer_size, __format.c_str(), &_time);
			return buff;
		}

		utils::str	formatIso() const { return format("%Y-%m-%dT%H:%M:%S"); }
		utils::str	formatLog() const { return format("%Y-%m-%d %H:%M:%S"); }
		utils::str	formatEmail() const { return format("%a, %d %b %Y %H:%M:%S"); }

		bool	operator==(const time &other) const { return _timestamp == other._timestamp; }
		bool	operator!=(const time &other) const { return !(*this == other); }
		bool	operator>(const time &other) const { return _timestamp > other._timestamp; }
		bool	operator>=(const time &other) const { return _timestamp >= other._timestamp; }
		bool	operator<(const time &other) const { return _timestamp < other._timestamp; }
		bool	operator<=(const time &other) const { return _timestamp <= other._timestamp; }
		double	operator-(const time &other) const { return std::difftime(_timestamp, other._timestamp); }
	};
}

inline std::ostream	&operator<<(std::ostream &os, utils::time_month::type value)
{
	os << utils::time_month_str[value];
	return os;
}

inline std::ostream	&operator<<(std::ostream &os, utils::time value)
{
	os << value.format();
	return os;
}

inline std::ostream	&operator<<(std::ostream &os, utils::time_week_day::type value)
{
	os << utils::time_week_day_str[value];
	return os;
}

#endif
