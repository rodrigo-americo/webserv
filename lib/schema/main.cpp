/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:30:21 by ighannam          #+#    #+#             */
/*   Updated: 2026/05/29 19:34:27 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "schema.hpp"
#include <iostream>
#include <vector>

// int main()
// {    
//     std::string str = "Isadora";
//     schema_string name_schema =  schema::string().minLenght(20).name("NAME");
//     schema_result_string res = name_schema.parse(str);
//     std::cout << res.format();
    
//     schema_result_int res_int = schema::integer().min(10).name("NAME INT").parse("2");
//     std::cout << res_int.format();

//     schema_double double_test = schema::dbl().name("DOUBLE").min(2.35);
//     schema_result_double res_double = double_test.parse("1.8d");
//     std::cout << res_double.format();

//     schema_bool bool_test = schema::boolean().name("BOOL").add_truthy("ok").truthy();
//     schema_result_bool res_bool = bool_test.parse("False");
//     std::cout << res_bool.format();
    
//     return 0;
// }

#include "container.hpp"
#include <memory>

int main()
{
    container<std::vector, int> container_teste;
    container_teste.getTeste()
}