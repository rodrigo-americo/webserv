/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_type.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:22:30 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/03 11:46:19 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HAS_TYPE_HPP
# define HAS_TYPE_HPP

namespace segregation
{
	template <typename Type>
	class has_type
	{
		protected:
			Type	_type;

			void	_setType(Type type) { _type = type; }
		public:
			has_type(Type type): _type(type) {};
			has_type(): _type() {};
			~has_type() = default;

			Type	getType() const { return _type; }
	};
}

#endif
