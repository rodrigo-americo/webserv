/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseConversor.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:27:57 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/04 00:34:20 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_CONVERSOR_HPP
# define HTTP_RESPONSE_CONVERSOR_HPP

# include "str.hpp"

class HttpResponse;

class HttpResponseConversor
{
	protected:
		const HttpResponse &_res;
	public:
		HttpResponseConversor(const HttpResponse &res): _res(res) {};
		virtual ~HttpResponseConversor() {};

		virtual void	convert(utils::str &res) const = 0;
};

#endif
