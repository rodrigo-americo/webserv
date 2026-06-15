/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseConversor_1_1.hpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:35:55 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 16:19:27 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_CONVERSOR_1_1_HPP
# define HTTP_RESPONSE_CONVERSOR_1_1_HPP

# include <string>

# include "HttpResponseConversor.hpp"

class HttpResponseConversor_1_1: public HttpResponseConversor
{
private:
	void	_convert_set_cookies(std::string &res) const;

public:
	HttpResponseConversor_1_1(const HttpResponse &res): HttpResponseConversor(res) {};
	~HttpResponseConversor_1_1() {};

	void	convert(std::string &res) const;
};

#endif
