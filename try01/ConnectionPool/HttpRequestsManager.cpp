#include "HttpRequestsManager.hpp"
# include "ConnectionPool.hpp"

Server	*HttpRequestsManager::_findServer(const Socket *listener) const
{
	std::map<const Socket*, Server*>::const_iterator it = _observers.find(listener);
	return (it == _observers.end()) ? NULL : it->second;
}

bool HttpRequestsManager::_isOwnedByCgi(SocketConnection *conn) const
{
	for (std::set<CgiProcess*>::const_iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
	{
		if ((*it)->clientConn() == conn)
			return true;
	}
	return false;
}

void	HttpRequestsManager::removePending(SocketConnection *conn)
{
	std::map<const Socket *, HttpRequestBuilder*>::iterator	it = _pending.find(conn);
	if (it != _pending.end())
	{
		delete it->second;
		_pending.erase(it);
	}
}

// esvazia o que ja chegou no buffer de recepcao do kernel antes de fechar a
// conexao: fechar um socket com dados nao lidos pendentes faz o kernel
// mandar RST em vez de FIN, e o cliente ve "connection reset" no lugar da
// resposta de erro que acabamos de enfileirar.
void	HttpRequestsManager::_drainSocket(SocketConnection *conn) const
{
	static const size_t buffer_size = 4096;
	std::string discard;
	ssize_t bytes_read;
	do {
		discard.clear();
		bytes_read = conn->read(buffer_size, discard);
	} while (bytes_read > 0);
}

HttpRequestsManager::HttpRequestsManager(): _pending(), _observers(), _running_cgis() {};
HttpRequestsManager::~HttpRequestsManager()
{
	for (std::map<const Socket *, HttpRequestBuilder *>::iterator it = _pending.begin(); it != _pending.end(); ++it)
		delete it->second;
	for (std::set<CgiProcess *>::iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
		delete *it;
};

void	HttpRequestsManager::observeSocket(const Socket *socket, Server *server)
{
	_observers[socket] = server;
}

void		HttpRequestsManager::addCgi(CgiProcess *cgi)
{
	_running_cgis.insert(cgi);
}

void		HttpRequestsManager::removeCgi(CgiProcess *cgi)
{
	_running_cgis.erase(cgi);
}

void		HttpRequestsManager::removeActiveConnection(SocketConnection *conn)
{
	_active_connections.erase(conn);
}

void	HttpRequestsManager::notifyRequest(HttpRequestBuilder &req_builder)
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

void	HttpRequestsManager::buildRequest(HttpRequestBuilder &req_builder)
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
		req_builder.sendBadRequest();
		_drainSocket(conn);
		removePending(conn);
		ConnectionPool::updateWriteInterest(conn, conn->hasPendingWrite());
		return ;
	}

	if (req_builder.isComplete())
	{
		notifyRequest(req_builder);
		removePending(conn);
		if (!conn->hasPendingWrite() && !_isOwnedByCgi(conn))
			ConnectionPool::removeFileDescriptor(conn);
		else
			// enquanto o CGI ainda esta rodando, nada foi enfileirado pra esse
			// conn ainda: fica so com POLLIN (sem POLLOUT) ate a resposta
			// chegar via CgiProcess::buildAndSendResponse().
			ConnectionPool::updateWriteInterest(conn, conn->hasPendingWrite());
	}
	if (bytes_read == 0 || (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)){
		removePending(conn);
		ConnectionPool::removeFileDescriptor(conn);
	}
}

void	HttpRequestsManager::buildRequest(SocketConnection *conn)
{
	HttpRequestBuilder *existing = findPending(conn);
	if (existing)
		return buildRequest(*existing);
	_active_connections.insert(conn);
	HttpRequestBuilder	*builder = new HttpRequestBuilder(conn);
	_pending[conn] = builder;
	buildRequest(*builder);
}

CgiProcess	*HttpRequestsManager::findCgiByConnectionEvent(const ConnectionEvent &event)
{
	for (std::set<CgiProcess*>::iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
	{
		if ((*it)->stdinPipe()  == event.file_descriptor
		|| (*it)->stdoutPipe() == event.file_descriptor
		|| (*it)->clientConn() == event.file_descriptor)
			return *it;
	}
	ConnectionPool::removeFileDescriptor(event.file_descriptor);
	return NULL;
}

CgiProcess	*HttpRequestsManager::findCgiByConnection(const SocketConnection *conn)
{
	for (std::set<CgiProcess*>::iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
		{
			if ((*it)->clientConn() == conn)
				return *it;
		}
		return NULL;
}

HttpRequestBuilder	*HttpRequestsManager::findPending(SocketConnection *conn)
{
	std::map<const Socket *, HttpRequestBuilder *>::iterator it = _pending.find(conn);
	if (it != _pending.end())
		return it->second;
	return NULL;
}

void				HttpRequestsManager::checkTimeout()
{
	time_t now = time(NULL);
	std::set<CgiProcess*>::iterator cit = _running_cgis.begin();
	while (cit != _running_cgis.end())
	{
		if ((*cit)->isExpired(now, 30))
		{
			LOG_TRACE("CGI expired");
			std::set<CgiProcess*>::iterator victim = cit;
			++cit;
			(*victim)->timeoutResponse();
		}
		else
			++cit;
	}

	for (std::set<SocketConnection *>::iterator it = _active_connections.begin(); it != _active_connections.end();)
	{
		if ((*it)->expired())
		{
			HttpResponse	res(*it);
			HttpResponseError error(res, 408, "Timeout Error", NULL);
			res.headers.content_type("text/plain");
			res.send(ResponseHTTPVersion::HTTP_1_1);
			std::set<SocketConnection *>::iterator temp = it;
			++it;
			_active_connections.erase(temp);
		}
		else
			++it;
	}

}
