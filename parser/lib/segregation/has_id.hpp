/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_id.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:50:32 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/22 17:06:34 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HAS_ID_HPP
# define HAS_ID_HPP

# include <string>

namespace segregation
{
	class has_id
	{
	protected:
		size_t	_id;

	public:
		has_id(size_t id): _id(id) {};
		~has_id() = default;

		size_t	getId() const { return _id; }
	};

	class has_incremental_id: public has_id
	{
	private:
		static size_t	_current_id;
	protected:

	public:
		has_incremental_id(): has_id(_current_id++) {};
		~has_incremental_id() = default;

		size_t	getId() const { return _id; }
	};
}

#endif
