/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 15:00:29 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/04 14:32:43 by ighannam         ###   ########.fr       */
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
}

#endif