/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 15:00:29 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/07 21:40:30 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <vector>

namespace utils
{
	template <typename T>
	std::string to_string(T entry)
	{
		std::stringstream ss;
		ss << std::boolalpha << entry;
		return ss.str();
	}

	std::string to_string(bool entry)
	{
		return entry ? "true" : "false";
	}

    template <typename T>
    T abs(T entry)
    {
        return entry > 0 ? entry : -entry;
    }

    template <typename T>
    bool fequal(T value_one, T value_two, double eps = 1e-9)
    {
        return abs(value_one - value_two) <= eps;
    }

    bool fequal(float value_one, float value_two, double eps = 1e-4)
    {
        return fequal<float>(value_one, value_two, eps);
    }

    template <typename T>
    bool oneOf(const std::vector<T>& options, const T& value)
    {
        for (size_t i = 0; i < options.size(); i++)
        {
            if (options[i] == value)
                return true;
        }
        return false;
    }

	inline double	lerp(double n1, double n2, double factor) { return n1 + ((n2 - n1) * factor); }
}

#endif
