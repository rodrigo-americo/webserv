/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_boolean.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:44:41 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 16:51:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATORBOOLEANSCHEMA_HPP
# define VALIDATORBOOLEANSCHEMA_HPP

# include "schema_base.hpp"

namespace schema
{
	namespace detail
	{
		class boolean_base: public schema_base<boolean_base, bool>
		{
			private:

			public:
				boolean_base() = default;
				boolean_base(const boolean_base &other) = default;
				boolean_base(boolean_base &&other) = default;
				boolean_base	&operator=(const boolean_base &other) = default;
				boolean_base	&operator=(boolean_base &&other) = default;

				result<bool>	parse(const std::string &raw) const
				{
					std::string	str = raw;
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);

					bool	val = false;
					bool	found = false;

					std::vector<std::string> true_values = { "true", "1", "yes" };
					std::vector<std::string> false_values = { "false", "0", "no" };
					for (const auto &value : true_values)
					{
						if (raw == value)
						{
							val = true;
							found = true;
						}

					}
					if (!found)
					{
						for (const auto &value : false_values)
							if (raw == value)
							{
								val = false;
								found = true;
							}
					}
					if (!found)
						return result<bool>::failure({
							this->_field_name, text::add(error_msg_style, "'" + raw + "' is not a valid boolean!").build()});
					return result<bool>::success(val);
				}
		};
	}
}

#endif
