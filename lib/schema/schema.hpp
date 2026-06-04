/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:28:43 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/02 16:29:27 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_HPP
# define SCHEMA_HPP

# include "schema_string.hpp"
# include "schema_number.hpp"
# include "schema_bool.hpp"

typedef schema::detail::schema_string schema_string;
typedef schema::detail::result<std::string> schema_result_string;

typedef schema::detail::schema_int<int> schema_int;
typedef schema::detail::result<int> schema_result_int;

typedef schema::detail::schema_int<size_t> schema_size_t;
typedef schema::detail::result<size_t> schema_result_size_t;

typedef schema::detail::schema_int<unsigned int> schema_unsigned_int;
typedef schema::detail::result<unsigned int> schema_result_unsigned_int;

typedef schema::detail::schema_double<double> schema_double;
typedef schema::detail::result<double> schema_result_double;

typedef schema::detail::schema_double<float> schema_float;
typedef schema::detail::result<float> schema_result_float;

typedef schema::detail::schema_bool schema_bool;
typedef schema::detail::result<bool> schema_result_bool;

namespace schema
{
    inline schema_string          string() { schema_string str; return str; }
    inline schema_int          integer() { schema_int _int; return _int; }
    inline schema_unsigned_int unsigned_integer() { schema_unsigned_int _unsigned_int; return _unsigned_int; }
    inline schema_size_t      sizet() { schema_size_t _size_t; return _size_t; }
    inline schema_bool  boolean() { schema_bool _bool; return _bool; }
    inline schema_double  dbl() { schema_double _double; return _double; }
    inline schema_float  flt() { schema_float _float; return _float; }
}


#endif