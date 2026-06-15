/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:08:48 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/10 10:48:32 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <iostream>

template <typename T>
int assert(const T& expected, const T& actual, const std::string& test_name)
{
    if (expected != actual)
    {
        std::cerr << "FAILED: " << test_name
            << "  expected:  " << expected
            << "  got:  " << actual;
        return 1;
    }
    std::cout << "PASSED: " << test_name << "\n";
    return 0;    
}



#endif
