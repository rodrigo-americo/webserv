/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_name.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 11:11:34 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 17:29:54 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HAS_NAME_HPP
# define HAS_NAME_HPP

# include <string>

namespace segregation
{
	class has_name
	{
	protected:
		std::string	_name;
	public:
		has_name(): _name() {}
		has_name(std::string name): _name(std::move(name)) {}
		~has_name() = default;

		bool	operator==(const has_name &other) const
		{
			if (this == &other) return true;
			return _name == other._name;
		}

		bool	operator==(has_name &&other) const
		{
			return _name == other._name;
		}

		const std::string	&getName() const { return _name; }
	};
}

#endif
