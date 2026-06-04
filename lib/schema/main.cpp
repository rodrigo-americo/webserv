/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:30:21 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/04 15:07:14 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "schema.hpp"
#include <iostream>
#include <vector>
#include "schema_number.hpp"

int main()
{    
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(20).name("NAME");
    schema_result_string res = name_schema.parse(str);
    std::cout << res.format();
    
    schema_result_int res_int = schema::integer().min(10).name("NAME INT").parse("2");
    std::cout << res_int.format();

    schema_double double_test = schema::dbl().name("DOUBLE").min(2.35);
    schema_result_double res_double = double_test.parse("1.8");
    std::cout << res_double.format();

    schema_bool bool_test = schema::boolean().name("BOOL").add_truthy("ok").truthy();
    schema_result_bool res_bool = bool_test.parse("False");
    std::cout << res_bool.format();

    struct functor_refine_no_space : public schema_string::functor_refine
    {
        functor_refine_no_space() {};
        functor_refine* clone() const
        {
            return new functor_refine_no_space(*this);
        }
        bool operator()(const std::string& value)
        {
            for (size_t i = 0; i < value.size(); i++)
            {
                if (value[i] == ' ')
                    return false;
            }
            return true;
        }
    };

    schema_string no_space = schema::string().refine(new functor_refine_no_space(), "there as spaces");
    schema_result_string res_str_refine = no_space.parse("tes te");
    schema_result_string res_str_refine2 = no_space.parse("tes te");
    std::cout << res_str_refine.format();
    std::cout << res_str_refine2.format();
    
    int arr[] = {301, 302, 303, 307, 308};
    std::vector<int> options(arr, arr + 5);
    schema_int oneOf = schema::integer().oneOf(options);
    schema_result_int res_one_of = oneOf.parse("3010");
    std::cout << res_one_of.format();
    
    
    return 0;
}

// #include "container.hpp"
// #include <memory>

// int main()
// {
//     container<std::vector, int> container_teste;
//     container_teste.getTeste()
// }