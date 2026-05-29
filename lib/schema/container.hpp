/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   container.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 19:22:18 by ighannam          #+#    #+#             */
/*   Updated: 2026/05/29 19:35:21 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTAINER_HPP
# define CONTAINER_HPP

#include <memory>
#include <vector>

template <template <typename > class Container, typename T>
class container
{
private:
    Container<T> teste;
public:
    container(/* args */) : teste() {};
    ~container() {};
    Container<T,Q> getTeste() { return teste; };
};

#endif
