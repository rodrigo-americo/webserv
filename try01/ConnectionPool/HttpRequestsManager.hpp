#ifndef PENDING_HTTP_REQUESTS_HPP
# define PENDING_HTTP_REQUESTS_HPP

# include <map>

# include "Server.hpp"
# include "SocketConnection.hpp"
# include "HttpRequestBuilder.hpp"
# include "ConnectionPool.hpp"

class PendingHttpRequests
{
private:
	std::map<Socket *, HttpRequestBuilder>	_pending;
	std::map<const Socket*, Server*>		_observers;
	WebServerConfig							*_global_config;

	Server	*_findServer(const Socket *listener) const
	{
		std::map<const Socket*, Server*>::const_iterator it = _observers.find(listener);
		return (it == _observers.end()) ? NULL : it->second;
	}

public:
	PendingHttpRequests(): _pending(), _observers(), _global_config(NULL) {};
	~PendingHttpRequests() {};

	void	setGlobalConfig(WebServerConfig *config)
	{
		_global_config = config;
	}

	void	observeSocket(const Socket *socket, Server *server)
	{
		_observers[socket] = server;
	}

	void	notifyRequest(HttpRequestBuilder &req_builder)
	{
		std::map<const Socket*, Server*>::iterator list_it = _observers.find(req_builder.connection->listenner());
		if (list_it == _observers.end())
		{
			std::cerr << "Error on handling request, listenner '" << req_builder.connection->listenner()->fd() << "' not found!" << std::endl;
			return;
		}
		HttpRequest		req = req_builder.build();
		HttpResponse	res(req_builder.connection);
		list_it->second->handleRequest(req, res);
	}

	void	buildRequest(HttpRequestBuilder &req_builder)
	{
		SocketConnection	*conn = req_builder.connection;
		static const size_t buffer_size = 1024;
		std::string	chunk;
		ssize_t bytes_read = conn->read(buffer_size, chunk);
		while (bytes_read > 0)
		{
			req_builder.addToBuffer(chunk);
			if (req_builder.isComplete() || req_builder.hasError()) break;
			if (bytes_read < static_cast<ssize_t>(buffer_size)) break;
			chunk.clear();
			bytes_read = conn->read(buffer_size, chunk);
		}
		if (req_builder.hasError())
		{
			_pending.erase(conn);
			req_builder.sendBadRequest(_global_config);
			return ;
		}

		if (req_builder.isComplete())
		{
			_pending.erase(conn);
			notifyRequest(req_builder);
			if (!conn->hasPendingWrite() && !_isConnectionOwnedByCgi(conn))
				ConnectionPool::removeFileDescriptor(conn);
		}
	}

	void	buildRequest(SocketConnection *conn)
	{
		Server *server = _findServer(conn->listenner());
		const WebServerConfig *config = server ? server->getConfig() : NULL;
		HttpRequestBuilder	builder(conn, config);
		_pending[conn] = builder;
		buildRequest(builder);
	}
};

#endif
