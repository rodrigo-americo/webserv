/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerSsl.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:20:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 19:33:00 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_SSL_HPP
# define CONFIG_SERVER_SSL_HPP

# include <string>
# include <vector>

class ConfigServerSsl {
	private:
		std::string					_certificate;					// ssl_certificate
		std::string					_certificate_key;				// ssl_certificate_key
		std::vector<std::string>	_protocols;						// ssl_protocols
		std::string					_ciphers;						// ssl_ciphers
		std::string					_session_cache;					// ssl_session_cache
		int							_session_timeout = 300;			// ssl_session_timeout
		bool						_prefer_server_ciphers = false;

	public:

};

#endif
