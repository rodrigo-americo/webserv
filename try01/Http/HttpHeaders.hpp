/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaders.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 21:56:02 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/05 13:54:41 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HEADERS_HPP
# define HTTP_HEADERS_HPP

# include <map>
# include <vector>
# include <string>

#include "str.hpp"

class SetCookie
{
private:
	std::vector<std::pair<std::string, std::string> >	_set_cookie;

public:
	SetCookie() {}
	~SetCookie() {}

	void	addKeyValue(const std::string &key, const std::string &value)
	{
		_set_cookie.push_back(std::make_pair(key, value));
	}

	const std::vector<std::pair<std::string, std::string> >	&getItems() const { return _set_cookie; }
};

struct CaseInsensitiveCompare
{
	bool operator()(const std::string &a, const std::string &b) const
	{
		return utils::str(a).tolower().string() < utils::str(b).tolower().string();
	}
};

class HttpHeaders
{
	public:
		typedef std::map<std::string, std::string, CaseInsensitiveCompare>	Headers;
		typedef Headers::iterator								iterator;
		typedef Headers::const_iterator								const_iterator;
	private:

		Headers					_headers;
		std::vector<SetCookie>	_set_cookies;
		const std::string		_dummy;

	public:
		HttpHeaders(): _headers(), _dummy() {};
		~HttpHeaders() {};

		iterator		begin() { return _headers.begin(); }
		const_iterator	begin() const { return _headers.begin(); }
		iterator		end() { return _headers.end(); }
		const_iterator	end() const { return _headers.end(); }

		std::string	&operator[] (const std::string &value)
		{ return _headers[value]; }
		const std::string	&operator[] (const std::string &value) const
		{
			Headers::const_iterator	it =  _headers.find(value);
			if (it == _headers.end())
				return _dummy;
			return it->second;
		}

		// Headers gerais

		void				cache_control(const std::string &value) { (*this)["Cache-Control"] = value; }
		const std::string	&cache_control() const { return (*this)["Cache-Control"]; }
		void				connection(const std::string &value) { (*this)["Connection"] = value; }
		const std::string	&connection() const { return (*this)["Connection"]; }
		void				date(const std::string &value) { (*this)["Date"] = value; }
		const std::string	&date() const { return (*this)["Date"]; }
		void				pragma(const std::string &value) { (*this)["Pragma"] = value; }
		const std::string	&pragma() const { return (*this)["Pragma"]; }
		void				trailer(const std::string &value) { (*this)["Trailer"] = value; }
		const std::string	&trailer() const { return (*this)["Trailer"]; }
		void				transfer_encoding(const std::string &value) { (*this)["Transfer-Encoding"] = value; }
		const std::string	&transfer_encoding() const { return (*this)["Transfer-Encoding"]; }
		void				upgrade(const std::string &value) { (*this)["Upgrade"] = value; }
		const std::string	&upgrade() const { return (*this)["Upgrade"]; }
		void				via(const std::string &value) { (*this)["Via"] = value; }
		const std::string	&via() const { return (*this)["Via"]; }
		void				warning(const std::string &value) { (*this)["Warning"] = value; }
		const std::string	&warning() const { return (*this)["Warning"]; }

		// Headers de roteamento e identificação

		void				host(const std::string &value) { (*this)["Host"] = value; }
		const std::string	&host() const { return (*this)["Host"]; }
		void				user_agent(const std::string &value) { (*this)["User-Agent"] = value; }
		const std::string	&user_agent() const { return (*this)["User-Agent"]; }
		void				referer(const std::string &value) { (*this)["Referer"] = value; }
		const std::string	&referer() const { return (*this)["Referer"]; }
		void				origin(const std::string &value) { (*this)["Origin"] = value; }
		const std::string	&origin() const { return (*this)["Origin"]; }
		void				from(const std::string &value) { (*this)["From"] = value; }
		const std::string	&from() const { return (*this)["From"]; }
		void				forwarded(const std::string &value) { (*this)["Forwarded"] = value; }
		const std::string	&forwarded() const { return (*this)["Forwarded"]; }
		void				x_forwarded_for(const std::string &value) { (*this)["X-Forwarded-For"] = value; }
		const std::string	&x_forwarded_for() const { return (*this)["X-Forwarded-For"]; }
		void				x_forwarded_fost(const std::string &value) { (*this)["X-Forwarded-Host"] = value; }
		const std::string	&x_forwarded_fost() const { return (*this)["X-Forwarded-Host"]; }
		void				x_forwarded_froto(const std::string &value) { (*this)["X-Forwarded-Proto"] = value; }
		const std::string	&x_forwarded_froto() const { return (*this)["X-Forwarded-Proto"]; }

		// Headers de conteúdo

		void				content_type(const std::string &value) { (*this)["Content-Type"] = value; }
		const std::string	&content_type() const { return (*this)["Content-Type"]; }
		void				content_length(const std::string &value) { (*this)["Content-Length"] = value; }
		const std::string	&content_length() const { return (*this)["Content-Length"]; }
		void				content_encoding(const std::string &value) { (*this)["Content-Encoding"] = value; }
		const std::string	&content_encoding() const { return (*this)["Content-Encoding"]; }
		void				content_language(const std::string &value) { (*this)["Content-Language"] = value; }
		const std::string	&content_language() const { return (*this)["Content-Language"]; }
		void				content_location(const std::string &value) { (*this)["Content-Location"] = value; }
		const std::string	&content_location() const { return (*this)["Content-Location"]; }
		void				content_MD5(const std::string &value) { (*this)["Content-MD5"] = value; }
		const std::string	&content_MD5() const { return (*this)["Content-MD5"]; }

		// Headers de negociação (Accept*)

		void				accept(const std::string &value) { (*this)["Accept"] = value; }
		const std::string	&accept() const { return (*this)["Accept"]; }
		void				accept_charset(const std::string &value) { (*this)["Accept-Charset"] = value; }
		const std::string	&accept_charset() const { return (*this)["Accept-Charset"]; }
		void				accept_encoding(const std::string &value) { (*this)["Accept-Encoding"] = value; }
		const std::string	&accept_encoding() const { return (*this)["Accept-Encoding"]; }
		void				accept_language(const std::string &value) { (*this)["Accept-Language"] = value; }
		const std::string	&accept_language() const { return (*this)["Accept-Language"]; }
		void				accept_datetime(const std::string &value) { (*this)["Accept-Datetime"] = value; }
		const std::string	&accept_datetime() const { return (*this)["Accept-Datetime"]; }

		// Headers de autenticação

		void				authorization(const std::string &value) { (*this)["Authorization"] = value; }
		const std::string	&authorization() const { return (*this)["Authorization"]; }
		void				proxy_authorization(const std::string &value) { (*this)["Proxy-Authorization"] = value; }
		const std::string	&proxy_authorization() const { return (*this)["Proxy-Authorization"]; }
		void				www_authenticate(const std::string &value) { (*this)["WWW-Authenticate"] = value; }
		const std::string	&www_authenticate() const { return (*this)["WWW-Authenticate"]; }
		void				proxy_authenticate(const std::string &value) { (*this)["Proxy-Authenticate"] = value; }
		const std::string	&proxy_authenticate() const { return (*this)["Proxy-Authenticate"]; }

		// Cookie

		void				cookie(const std::string &value) { (*this)["Cookie"] = value; }
		const std::string	&cookie() const { return (*this)["Cookie"]; }

		void				addSetCookie(const SetCookie &set_cookie) { _set_cookies.push_back(set_cookie); }
		const std::vector<SetCookie>	&set_cookies() const { return _set_cookies; }
};

#endif
