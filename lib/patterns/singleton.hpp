/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singleton.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 14:58:01 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 01:15:13 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SINGLETON_HPP
# define SINGLETON_HPP

namespace patterns
{
	template <typename Derived>
	class singleton
	{
	protected:
		singleton() {};
		~singleton() {};

		singleton(const singleton &);
		singleton	&operator=(const singleton &);

	public:

		static Derived	&getInstance()
		{
			static Derived	instance;
			return instance;
		}
	};
}

#endif
