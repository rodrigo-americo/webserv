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
	std::set<SocketConnection *>					_active_connections;

	Server	*_findServer(const Socket *listener) const;
	bool	_isOwnedByCgi(SocketConnection *conn) const;
	void	_removePending(SocketConnection *conn);

public:
	HttpRequestsManager();
	~HttpRequestsManager();

	void		observeSocket(const Socket *socket, Server *server);
	void		addCgi(CgiProcess *cgi);
	void		removeCgi(CgiProcess *cgi);
	void		removeActiveConnection(SocketConnection *conn);
	void		notifyRequest(HttpRequestBuilder &req_builder);
	void		buildRequest(HttpRequestBuilder &req_builder);
	void		buildRequest(SocketConnection *conn);
	CgiProcess	*findCgiByConnectionEvent(const ConnectionEvent &event);
	CgiProcess	*findCgiByConnection(const SocketConnection *conn);
	HttpRequestBuilder	*findPending(SocketConnection *conn);
	void				checkTimeout();
};

#endif
