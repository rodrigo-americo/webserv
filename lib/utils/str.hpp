/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 19:09:08 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 17:02:24 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STR_HPP
# define STR_HPP

# include <sstream>
# include <string>
# include <cstring>
# include <vector>

namespace utils
{
	template <typename T>
	bool	str_to(const std::string &str, T &item)
	{
		::std::stringstream	ss;
		ss << str;
		ss >> item;
		return !(ss.fail() || ss.peek() != ::std::stringstream::traits_type::eof());
	}

	class str
	{
	protected:
		::std::string _str;

	public:
		typedef std::string::iterator		iterator;
		typedef std::string::const_iterator	const_iterator;

		static const size_t npos = std::string::npos;

		str(): _str() {};
		str(const std::string &__str): _str(__str) {};
		str(const char *__str): _str(__str) {};
		str(size_t n, const char __str): _str(n, __str) {};
		str(const str &__str): _str(__str._str) {};
		~str() {};

		const std::string	&string() const { return _str; }
		::std::string	&string() { return _str; }

		iterator		begin() { return _str.begin(); }
		const_iterator	begin() const { return _str.begin(); }
		iterator		end() { return _str.end(); }
		const_iterator	end() const { return _str.end(); }

		size_t			size() const { return _str.size(); }
		bool			empty() const { return _str.empty(); }
		void			clear() { _str.clear(); }
		const char		*c_str() const { return _str.c_str(); }
		char			*data() { return &_str[0]; }
		iterator		erase(iterator start) { return _str.erase(start); }
		iterator		erase(iterator start, iterator _end) { return _str.erase(start, _end); }
		int				compare(const str &__str) const { return _str.compare(__str.string()); }
		int				compare(const char *__str) const { return _str.compare(__str); }
		// Attempt to preallocate enough memory for specified number of characters.
		str				&reserve(size_t size) { _str.reserve(size); return *this; }
		// ### Get a substring.
		//
		// Construct and return a new string using the `__n` characters starting at `__pos`. If the string is too short, use the remainder of the characters. If `__pos` is beyond the end of the string, `out_of_range` is thrown
		str				substr(size_t pos, size_t n = npos) const { str new_str = _str.substr(pos, n); return new_str; }

		/**
		 * clear all the `whitespaces` in the begin and in the end of de `str`.
		 * @attention modifies the current `str`
		 */
		str				&trim()
		{
			size_t	first = 0;
			size_t	last = size();
			while (_str[first] && ::std::isspace(_str[first]))
				++first;
			if (first == last)
			{
				clear();
				return *this;
			}
			while (--last > first && ::std::isspace(_str[last]))
				;
			_str = _str.substr(first, last - first + 1);
			return *this;
		}
		/**
		 * clear all the `whitespaces` in the begin and in the end of de `str`.
		 * @attention makes a copy of the current `str` for the `trim`.
		 */
		str				trim_const() const
		{
			str	string = *this;
			string.trim();
			return string;
		}

		/**
		 * make all the characters `lowercase`.
		 * @attention modifies the current `str`
		 */
		str				&tolower()
		{
			for (size_t i = 0; i < size(); i++)
				_str[i] = std::tolower(_str[i]);
			return *this;
		}
		/**
		 * make all the characters `lowercase`.
		 * @attention makes a copy of the current `str`.
		 */
		str				tolower_const() const
		{
			str	string = *this;
			string.tolower();
			return string;
		}

		/**
		 * make all the characters `uppercase`.
		 * @attention modifies the current `str`
		 */
		str				&toupper()
		{
			for (size_t i = 0; i < size(); i++)
				_str[i] = std::toupper(_str[i]);
			return *this;
		}
		/**
		 * make all the characters `uppercase`.
		 * @attention makes a copy of the current `str`.
		 */
		str				toupper_const() const
		{
			str	string = *this;
			string.toupper();
			return string;
		}

		::std::vector<str> split(char delim) const
		{
			return split(::std::string(1, delim));
		}

		::std::vector<str> split(const std::string &delim) const
		{
			::std::vector<str>	result;
			size_t	first = _str.find_first_not_of(delim, 0);
			size_t	last = _str.find_first_of(delim, first);
			result.push_back(_str.substr(first, last - first));
			while (last != npos || last < size())
			{
				first = _str.find_first_not_of(delim, last + 1);
				if (first == npos) break;
				last = _str.find_first_of(delim, first);
				result.push_back(_str.substr(first, last - first));
			}
			return result;
		}

		static str join(const std::vector<str> &parts, const str &sep = "")
		{
			str	_string;
			for (size_t i = 0; i < parts.size(); i++)
			{
				_string += parts[i];
				if (i < parts.size() - 1)
					_string += sep;
			}
			return	_string;
		}

		str &join_with(const std::vector<str> &parts, const str &sep = "")
		{
			_str += join(parts, sep).string();
			return *this;
		}

		str join_with_const(const std::vector<str> &parts, const str &sep = "") const
		{
			str	_string(*this);
			return _string.join_with(parts, sep);
		}

		str &replace_all(const str &from, const str &to)
		{
			size_t	first = _str.find(from.string());
			if (first == npos) return *this;

			if (from.size() == to.size())
			{
				while (first != npos)
				{
					::std::strcpy(data() + first, to.c_str());
					first += to.size();
					first = _str.find(from.string(), first);
				}
				return *this;
			}
			size_t	second = 0;
			str	_string = _str.substr(0, first);
			_string += to;
			while (first != npos)
			{
				second = _str.find(from.string(), first + from.size());
				if (second != npos)
					_string += to;
				_string += _str.substr(first, second - first);
				first = _str.find(from.string(), second + from.size());
				if (first != npos)
					_string += to;
			}
			_str = _string.string();
			return *this;
		}

		str replace_all_const(const str &from, const str &to)
		{
			str	_string(*this);
			return _string.replace_all(from, to);
		}

		str &replace_first(const str &from, const str &to)
		{
			size_t	first = _str.find(from.string());
			if (first == npos) return *this;

			if (from.size() == to.size())
			{
				for (size_t i = 0; i < from.size(); i++)
					_str[i + first] = to[i];
				return *this;
			}
			_str = _str.substr(0, first);
			_str += to.string();
			return *this;
		}

		str replace_first_const(const str &from, const str &to) const
		{
			str	_string(*this);
			return _string.replace_first(from, to);
		}

		str &remove(char c)
		{
			size_t	idx = _str.find(c);
			if (idx != npos)
				_str.erase(idx, 1);
			return *this;
		}

		str &remove(const str &sub)
		{
			size_t	idx = _str.find(sub.string());
			if (idx != npos)
				_str.erase(idx, sub.size());
			return *this;
		}

		str &insert(size_t pos, const str &s)
		{
			_str.insert(pos, s.string());
			return *this;
		}

		bool			starts_with(const str &__str) const
		{
			return find_first_of(__str) == 0;
		}

		bool			ends_with(const str &__str) const
		{
			size_t	idx = rfind(__str);
			if (idx == npos) return false;
			return idx + __str.size() == size();
		}

		/**
		 * try to convert `str` to `item type` ant put it on `item`. The not convertable part will be put on `rest`
		 */
		template <typename T>
		void			to(T &item, std::string &rest) const
		{
			::std::stringstream	ss;
			ss << _str;
			ss >> item;
			ss >> rest;
		}
		/**
		 * try to convert `str` to `item type` ant put it on `item`.
		 * @returns `true` if the full `str` was able to be converted, otherwise returns `false`.
		 */
		template <typename T>
		bool			to(T &item) const { return str_to(_str, item); }


		bool contains(const str &s) const
		{
			return _str.find_first_of(s.string()) != npos;
		}

		bool contains(char c) const
		{
			return _str.find_first_of(c) != npos;
		}

		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		size_t			find(const str &__str, size_t pos = 0) const
		{
			return	_str.find(__str.string(), pos);
		}
		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		template<typename T>
		size_t			find(const T &__str, size_t pos = 0) const
		{
			return	_str.find(__str, pos);
		}

		/**
		 *
		 * @returns Starting from __pos, searches backward for value of __str within this string. If found, returns the index where it begins. If not found, returns npos.
		 */
		size_t			rfind(const str &__str, size_t pos = npos) const
		{
			return	_str.rfind(__str.string(), pos);
		}
		/**
		 *
		 * @returns Starting from __pos, searches backward for value of __str within this string. If found, returns the index where it begins. If not found, returns npos.
		 */
		template<typename T>
		size_t			rfind(const T &__str, size_t pos = npos) const
		{
			return	_str.rfind(__str, pos);
		}

		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		size_t			find_first_of(const str &__str, size_t pos = 0) const
		{
			return	_str.find_first_of(__str.string(), pos);
		}
		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		template<typename T>
		size_t			find_first_of(const T &__str, size_t pos = 0) const
		{
			return	_str.find_first_of(__str, pos);
		}

		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		size_t			find_first_not_of(const str &__str, size_t pos = 0) const
		{
			return	_str.find_first_not_of(__str.string(), pos);
		}
		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		template<typename T>
		size_t			find_first_not_of(const T &__str, size_t pos = 0) const
		{
			return	_str.find_first_not_of(__str, pos);
		}

		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		size_t			find_last_of(const str &__str, size_t pos = npos) const
		{
			return	_str.find_last_of(__str.string(), pos);
		}
		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		template<typename T>
		size_t			find_last_of(const T &__str, size_t pos = npos) const
		{
			return	_str.find_last_of(__str, pos);
		}

		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		size_t			find_last_not_of(const str &__str, size_t pos = npos) const
		{
			return	_str.find_last_not_of(__str.string(), pos);
		}
		/**
		 *
		 * @returns the position of the string, or `npos` if not fond
		 */
		template<typename T>
		size_t			find_last_not_of(const T &__str, size_t pos = npos) const
		{
			return	_str.find_last_not_of(__str, pos);
		}

		char			&operator[](size_t idx) { return _str[idx]; }
		const char		&operator[](size_t idx) const { return _str[idx]; }

		bool			operator>(const str &other) const {  return _str > other._str; }
		bool			operator>=(const str &other) const {  return _str >= other._str; }
		bool			operator<(const str &other) const {  return _str < other._str; }
		bool			operator<=(const str &other) const {  return _str <= other._str; }
		bool			operator==(const str &other) const {  return _str == other._str; }
		bool			operator!=(const str &other) const {  return _str != other._str; }

		bool			operator>(const std::string &other) const { return _str > other; }
		bool			operator>=(const std::string &other) const { return _str >= other; }
		bool			operator<(const std::string &other) const { return _str < other; }
		bool			operator<=(const std::string &other) const { return _str <= other; }
		bool			operator==(const std::string &other) const { return _str == other; }
		bool			operator!=(const std::string &other) const { return _str != other; }

		bool			operator>(const char *other) const { return _str > other; }
		bool			operator>=(const char *other) const { return _str >= other; }
		bool			operator<(const char *other) const { return _str < other; }
		bool			operator<=(const char *other) const { return _str <= other; }
		bool			operator==(const char *other) const { return _str == other; }
		bool			operator!=(const char *other) const { return _str != other; }

		str				&operator=(const str &other) { if (this == &other) return *this; _str = other._str; return *this; }
		str				&operator=(const std::string &other) { _str = other; return *this; }
		str				&operator=(const char *other) { _str = other; return *this; }
		str				&operator=(const char other) { _str = other; return *this; }

		str				&operator+=(const str &other) { _str += other._str; return *this; }
		str				&operator+=(const std::string &other) { _str += other; return *this; }
		str				&operator+=(const char *other) { _str += other; return *this; }
		str				&operator+=(const char other) { _str += other; return *this; }

		str				operator+(const str &other) const { str copy(*this); return copy += other; }
		str				operator+(const std::string &other) const { str copy(*this); return copy += other; }
		str				operator+(const char *other) const { str copy(*this); return copy += other; }
		str				operator+(const char other) const { str copy(*this); return copy += other; }
	};

}

inline std::ostream	&operator<<(std::ostream &os, const utils::str &_str)
{
	os << _str.string();
	return os;
}

inline std::istream	&operator>>(std::istream &is, utils::str &_str)
{
	is >> _str.string();
	return is;
}

inline std::string		operator+(const std::string &string, const utils::str &_str) { std::string copy(string); return copy += _str.string(); }

#endif
