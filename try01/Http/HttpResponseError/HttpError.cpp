/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpError.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 23:41:16 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/04 01:05:32 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpError.hpp"
#include "Router.hpp"

void	HttpError::badRequest(const utils::str &body) const
{
	HttpResponseError(_router.res, 400, "Bad Request.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::unauthorized(const utils::str &body) const
{
	HttpResponseError(_router.res, 401, "Uauthorized.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::forbiden(const utils::str &body) const
{
	HttpResponseError(_router.res, 403, "Forbiden.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::notFound(const utils::str &body) const
{
	HttpResponseError(_router.res, 404, "Not Found.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::methodNotAllowed(const utils::str &body) const
{
	HttpResponseError(_router.res, 405, "Method not Allowed.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::timeout(const utils::str &body) const
{
	HttpResponseError(_router.res, 408, "Timeot Error", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::contentLarge(const utils::str &body) const
{
	HttpResponseError(_router.res, 413, "Content too Large", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::unsupportedMediaType(const utils::str &body) const
{
	HttpResponseError(_router.res, 415, "Unsupported Media Type", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

void	HttpError::internalServerError(const utils::str &body) const
{
	HttpResponseError(_router.res, 500, "Internal Server Error.", _router.config_server).body(body).send(ResponseHTTPVersion::HTTP_1_1);
}

