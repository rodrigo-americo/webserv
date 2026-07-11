/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseConversor_1_1.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:08:30 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/04 00:33:38 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HttpResponseConversor_1_1.hpp"
# include "HttpResponse.hpp"

void	HttpResponseConversor_1_1::_convert_set_cookies(utils::str &res) const
{
	const std::vector<SetCookie> &set_cookies = _res.headers.set_cookies();
	for (size_t cook = 0; cook < set_cookies.size(); cook++)
	{
		res += "Set-Cookie: ";
		const std::vector<std::pair<std::string, std::string> > &set_cookie = set_cookies[cook].getItems();
		for (size_t i = 0; i < set_cookie.size(); i++)
		{
			res += set_cookie[i].first + "=" + set_cookie[i].second;
			if (i < set_cookie.size() - 1)
				res += ';';
		}
		res += "\r\n";
	}
}

void	HttpResponseConversor_1_1::convert(utils::str &res) const
	{
		res.clear();
		res += "HTTP/1.1 ";
		res += utils::to_string(_res.status_code) + " " + _res.status_code_message + "\r\n";
		for (HttpHeaders::const_iterator it = _res.headers.begin(); it != _res.headers.end(); ++it)
			res += it->first + ": " + it->second + "\r\n";
		_convert_set_cookies(res);
		res += "\r\n";
		res += _res.body();
	}
