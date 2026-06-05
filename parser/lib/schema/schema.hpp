/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:18:42 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/03 23:15:26 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_HPP
# define SCHEMA_HPP

# include "schema_number.hpp"
# include "schema_string.hpp"
# include "schema_boolean.hpp"
# include "schema_time.hpp"
# include "schema_path.hpp"

namespace schema
{
	inline detail::time_base				time() { return detail::time_base{}; }
	inline detail::string_base				string() { return detail::string_base{}; }
	inline detail::path_base				path() { return detail::path_base{}; }
	inline detail::boolean_base				boolean() { return detail::boolean_base{}; }

	inline detail::float_base<double>		number() { return detail::float_base<double>{}; };
	inline detail::float_base<double>		dbl() { return detail::float_base<double>{}; };
	inline detail::float_base<float>		flt() { return detail::float_base<float>{}; };
	inline detail::integer_base<int>		integer() { return detail::integer_base<int>{}; };
	inline detail::integer_base<long>		int64() { return detail::integer_base<long>{}; };
	inline detail::integer_base<unsigned>	uint32() { return detail::integer_base<unsigned>{}; };
}

#endif
