/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpError.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 21:03:33 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/04 01:04:49 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_ERROR_HPP
# define HTTP_ERROR_HPP

# include "str.hpp"
# include "HttpResponseError.hpp"

class Router;

class HttpError
{
private:
	Router	&_router;
public:
	HttpError(Router &router): _router(router) {};
	~HttpError() {};

	void	badRequest(const utils::str &body = "") const; // 400
	void	unauthorized(const utils::str &body = "") const; // 401
	void	forbiden(const utils::str &body = "") const; // 403
	void	notFound(const utils::str &body = "") const; // 404
	void	methodNotAllowed(const utils::str &body = "") const;// 405
	void	timeout(const utils::str &body = "") const; // 408
	void	contentLarge(const utils::str &body = "") const; // 413
	void	unsupportedMediaType(const utils::str &body = "") const; // 415
	void	internalServerError(const utils::str &body = "") const; // 500
};

#endif
