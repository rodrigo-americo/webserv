/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigEvents.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 23:20:06 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/09 23:23:17 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_EVENTS_HPP
# define CONFIG_EVENTS_HPP

# include <string>

class ConfigEvents
{
	private:
		size_t		_worket_connections;
		std::string	_use;
		bool		_multi_accept;

	public:
		ConfigEvents() {}
		~ConfigEvents() {}

	size_t		worket_connections() const { return _worket_connections; }
	std::string	use() const { return _use; }
	bool		multi_accept() const { return _multi_accept; }
};

#endif
