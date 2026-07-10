#ifndef PENDING_HTTP_REQUESTS_HPP
# define PENDING_HTTP_REQUESTS_HPP

# include <map>
# include <set>

# include "Server.hpp"
# include "SocketConnection.hpp"
# include "HttpRequestBuilder.hpp"
# include "ConnectionEvent.hpp"

class ConnectionPool;

class HttpRequestsManager
{
private:
	std::map<const Socket *, HttpRequestBuilder*>	_pending;
	std::map<const Socket*, Server*>				_observers;
	std::set<CgiProcess*>							_running_cgis;
	WebServerConfig									*_global_config;

	Server	*_findServer(const Socket *listener) const;
	bool	_isOwnedByCgi(SocketConnection *conn) const;
	void	_removePending(SocketConnection *conn);

public:
	HttpRequestsManager();
	~HttpRequestsManager();

	void		setGlobalConfig(WebServerConfig *config);
	void		observeSocket(const Socket *socket, Server *server);
	void		addCgi(CgiProcess *cgi);
	void		removeCgi(CgiProcess *cgi);
	void		notifyRequest(HttpRequestBuilder &req_builder);
	void		buildRequest(HttpRequestBuilder &req_builder);
	void		buildRequest(SocketConnection *conn);
	CgiProcess	*findCgiByConnectionEvent(const ConnectionEvent &event);
	CgiProcess	*findCgiByConnection(const SocketConnection *conn);
	HttpRequestBuilder	*findPending(SocketConnection *conn);
	void				checkTimeout();
};

#endif
