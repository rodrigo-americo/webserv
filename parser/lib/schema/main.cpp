/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 23:21:43 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/18 13:35:08 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include "schema.hpp"

template<typename T>
void	parseSchema(T &schema, std::string test)
{
	auto result = schema.parse(std::move(test));
	if (result)
		std::cout << "result = " << result.value << "\n";
	else
		result.dump();
}

template<typename T>
void	parseTimeSchema(T &schema, std::string test, std::string print_format = "HHhMM")
{
	auto result = schema.parse(std::move(test));
	if (result)
	{
		result.value.setFormat(print_format);
		std::cout << "result = " << result.value << "\n";
	}
	else
		result.dump();
}

int main()
{
	auto	my_bool_schema = schema::boolean();
	parseSchema(my_bool_schema, "true1");
	auto	my_double_schema = schema::dbl().name("my_double").negative().max(-10).min(-88);
	parseSchema(my_double_schema, "-99.5");
	auto	my_time_schema = schema::time().name("my_time").betweenHours(5, 18).minSeconds(10).setFormat("HHhMM:ss");
	parseTimeSchema(my_time_schema, "19h26:05", "HHHhMM:ssss.mm");

	return 0;
}
