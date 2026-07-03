/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 15:00:29 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/02 14:55:09 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

# include <string>
# include <sstream>
# include <vector>
# include <limits>
# include <typeinfo>

# include "str.hpp"
# include "time.hpp"

namespace utils
{
	template <typename T>
	::std::string to_string(T entry)
	{
		::std::stringstream ss;
		ss << ::std::boolalpha << entry;
		return ss.str();
	}

	inline ::std::string to_string(bool entry)
	{
		return entry ? "true" : "false";
	}

	template <typename T>
	T abs(T entry)
	{
		if (entry == std::numeric_limits<T>::min())
			return std::numeric_limits<T>::max();
		return entry > 0 ? entry : -entry;
	}

	template <typename T>
	bool fequal(T value_one, T value_two, double eps = 1e-9)
	{
		return abs(value_one - value_two) <= (eps  + 1e-9);
	}

	inline bool fequal(float value_one, float value_two, double eps = 1e-4)
	{
		return fequal<float>(value_one, value_two, eps);
	}

	template <typename T>
	bool oneOf(const ::std::vector<T>& options, const T& value)
	{
		for (size_t i = 0; i < options.size(); i++)
		{
			if (options[i] == value)
				return true;
		}
		return false;
	}

	inline double	lerp(double n1, double n2, double factor) { return n1 + ((n2 - n1) * factor); }

	inline ::std::vector<double>	lerp(::std::vector<double> vec1, ::std::vector<double> vec2, double factor)
	{
		::std::vector<double>	resp;
		if (vec1.size() != vec2.size()) return resp;

		for (size_t i = 0; i < vec1.size(); i++)
			resp.push_back(lerp(vec1[i], vec2[i], factor));

		return resp;
	}
}

#endif
