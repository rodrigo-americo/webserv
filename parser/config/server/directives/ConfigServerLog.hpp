/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerLog.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:20:10 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 19:33:07 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_LOG_HPP
# define CONFIG_SERVER_LOG_HPP

# include <string>

class ConfigServerLog {
	private:
		std::string _path;
		std::string _format; // "main", "combined", etc
	public:
};

#endif
