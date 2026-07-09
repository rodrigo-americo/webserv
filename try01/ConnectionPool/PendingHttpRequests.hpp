#ifndef PENDING_HTTP_REQUESTS_HPP
# define PENDING_HTTP_REQUESTS_HPP

# include <map>

# include "Server.hpp"
# include "Socket.hpp"
# include "HttpRequestBuilder.hpp"

class PendingHttpRequests
{
private:
	std::map<Socket *, HttpRequestBuilder>	_pending;
public:
	PendingHttpRequests(): _pending() {};
	~PendingHttpRequests() {};

	bool handleRequest(HttpRequestBuilder &req_builder)
	{

	}
};

#endif
