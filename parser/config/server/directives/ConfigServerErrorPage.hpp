/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerErrorPage.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:19:39 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 19:33:19 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_SERVER_ERROR_PAGE_HPP
# define CONFIG_SERVER_ERROR_PAGE_HPP

# include <string>
# include <vector>

struct ConfigServerErrorPage {
	private:
		std::vector<int>	_codes;		// [404], [500, 502, 503]
		std::string			_uri;		// "/404.html"
};

#endif
