/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerSsl.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:20:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 00:15:33 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_SSL_HPP
# define CONFIG_SERVER_SSL_HPP

# include <string>
# include <vector>

class ConfigServerSsl {
	private:
		std::string					certificate;				// ssl_certificate
		std::string					certificate_key;			// ssl_certificate_key
		std::vector<std::string>	protocols;					// ssl_protocols
		std::string					ciphers;					// ssl_ciphers
		std::string					session_cache;				// ssl_session_cache
		int							session_timeout;			// ssl_session_timeout
		bool						prefer_server_ciphers;

	public:
		ConfigServerSsl()
			: certificate(), certificate_key(), protocols(), ciphers(),
			session_cache(), session_timeout(300), prefer_server_ciphers(false) {(void)session_timeout;
				 (void)prefer_server_ciphers;}
};

#endif
