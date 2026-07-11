/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 14:54:54 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 19:25:16 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>

# include "singleton.hpp"
# include "ConfigServer.hpp"

class Config: public patterns::singleton<Config>
{
	public:
		typedef ConfigServer	Server;


	private:
		friend class patterns::singleton<Config>;
		Config() {};
		~Config() {};
		Config(const Config &);
		Config	&operator=(const Config &);

		std::vector<ConfigServer>	_servers;

	public:
};

#endif
