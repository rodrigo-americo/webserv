#include "Server.hpp"

void Server::_serveCgi(const HttpRequest &req, HttpResponse &res,
                        const LocationConfig &location)
{
    (void)location;
    _sendError(res, 501, "Not Implemented", &req);
}
