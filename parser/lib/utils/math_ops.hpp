/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math_ops.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:04:31 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 15:38:21 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATH_OPS_HPP
# define MATH_OPS_HPP

# include <cmath>
# include <stdexcept>

namespace utils
{
	inline bool					fequal(double v1, double v2) { return std::fabs(v1 - v2) < 1e-9; }

	inline double				lerp(double n1, double n2, double factor) { return n1 + ((n2 - n1) * factor); }

	template<template <typename...> class Container, typename... Args>
	inline Container<double, Args...>	lerp(const Container<double, Args...> &arr1, const Container<double, Args...> &arr2, double factor)
	{
		if (arr1.size() != arr2.size()) throw std::runtime_error("lerp::error -> vectors have not the same dimentions!");
		Container<double, Args...>	result;
		result.reserve(arr1.size());
		for (size_t i = 0; i < arr1.size(); i++)
			result.insert(result.end(), lerp(arr1[i], arr2[i], factor));
		return result;
	}

	template<std::size_t N>
	inline std::array<double, N> lerp( const std::array<double, N>& a, const std::array<double, N>& b, double factor)
	{
		std::array<double, N> result;
		for (std::size_t i = 0; i < N; ++i)
			result[i] = a[i] + (b[i] - a[i]) * factor;
		return result;
	}
}

// # include <vector>
// int test()
// {
// 	std::vector<double> vec1;
// 	std::vector<double> vec2;

// 	utils::lerp(vec1, vec2, 0.5);
// }

#endif
