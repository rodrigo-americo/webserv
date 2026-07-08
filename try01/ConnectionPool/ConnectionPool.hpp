

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <map>
# include <list>
# include <vector>
# include <signal.h>
# include <sys/wait.h>
# include <ctime>

# include "signal.hpp"
# include "singleton.hpp"
# include "IMultiplexer.hpp"
# include "Server.hpp"
# include "HttpRequestBuilder.hpp"
#include "CgiProcess.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "Logger.hpp"


class HttpRequestObservers
{
	private:
		std::map<const Socket*, Server*>	_observers;

	public:
		HttpRequestObservers() {}
		~HttpRequestObservers() {}

		void	addSocketToObserver(const Socket *socket, Server *server)
		{
			_observers[socket] = server;
		}
		Server	*findServer(const Socket *listener) const
		{
			std::map<const Socket*, Server*>::const_iterator it = _observers.find(listener);
			return (it == _observers.end()) ? NULL : it->second;
		}
		bool	notifyRequest(RequestBuilder &req_builder)
		{
			std::map<const Socket*, Server*>::iterator list_it = _observers.find(req_builder.connection->listenner());
			if (list_it == _observers.end())
			{
				std::cerr << "Error on handling request, listenner '" << req_builder.connection->listenner()->fd() << "' not found!" << std::endl;
				return false;
			}
			HttpRequest		req = req_builder.build();
			HttpResponse	res(req_builder.connection);
			list_it->second->handleRequest(req, res);
			return true;
		}
};

/**
 * ConnectionPool sera:
 *
 * - um `singleton`;
 * - um `OberverSubject` e os `Servers` serão os `Observers` dele;
 */
class ConnectionPool: public patterns::singleton<ConnectionPool>
{
	friend class patterns::singleton<ConnectionPool>;
private:
	IMultiplexer				*_multiplexer;
	HttpRequestObservers		_http_request_observers;
	std::list<RequestBuilder>	_pending_request;
	std::list<CgiProcess*>		_running_cgis;

	RequestBuilder *_findPending(SocketConnection *conn)
	{
		for (std::list<RequestBuilder>::iterator it = _pending_request.begin(); it != _pending_request.end(); ++it)
		{
			if (it->connection == conn)
				return &(*it);
		}
		return NULL;
	}

	std::list<CgiProcess*>::iterator _findCgiByPipe(FileDescriptor *pipe_socket)
	{
		for (std::list<CgiProcess*>::iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
		{
			if ((*it)->stdinPipe()  == pipe_socket
			|| (*it)->stdoutPipe() == pipe_socket)
				return it;
		}
		return _running_cgis.end();
	}

	std::list<CgiProcess*>::iterator _findCgiByClient(SocketConnection *conn)
	{
		for (std::list<CgiProcess*>::iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
		{
			if ((*it)->clientConn() == conn)
				return it;
		}
		return _running_cgis.end();
	}

	bool _isConnectionOwnedByCgi(SocketConnection *conn) const
	{
		for (std::list<CgiProcess*>::const_iterator it = _running_cgis.begin(); it != _running_cgis.end(); ++it)
		{
			if ((*it)->clientConn() == conn)
				return true;
		}
		return false;
	}

	void	_removePending(SocketConnection *conn)
	{
		for (std::list<RequestBuilder>::iterator it = _pending_request.begin(); it != _pending_request.end(); ++it)
		{
			if (it->connection == conn)
			{
				_pending_request.erase(it);
				return;
			}
		}
	}

	bool	_handleRequest(RequestBuilder &req_builder)
	{
		static const size_t buffer_size = 1024;
		std::string	chunk;
		ssize_t bytes_read = req_builder.connection->read(buffer_size, chunk);
		while (bytes_read > 0)
		{
			req_builder.addToBuffer(chunk);
			if (req_builder.isComplete() || req_builder.hasError()) break;
			if (bytes_read < static_cast<ssize_t>(buffer_size)) break;
			chunk.clear();
			bytes_read = req_builder.connection->read(buffer_size, chunk);
		}
		if (req_builder.hasError())
		{
			HttpResponse res(req_builder.connection);
			res.statusCode(req_builder.errorStatus(), req_builder.errorMessage());
			res.headers.connection("close");
			res.body(req_builder.errorMessage() + "\n");
			res.send(ResponseHTTPVersion::HTTP_1_1);
			return true;
		}

		bool	is_request_complete = req_builder.isComplete();
		if (is_request_complete)
			_http_request_observers.notifyRequest(req_builder);
		return is_request_complete;
	}

	enum CgiCleanupReason
	{
		CGI_NORMAL,
		CGI_INTERNAL_ERROR,
		CGI_CLIENT_GONE,
		CGI_TIMEOUT
	};

	void _cleanupCgi(std::list<CgiProcess*>::iterator it, CgiCleanupReason reason)
	{
		LOG_TRACE("_cleanupCgi");
		CgiProcess *cgi = *it;
		SocketConnection *conn = cgi->clientConn();
		pid_t pid = cgi->pid();

		if (reason == CGI_NORMAL)
		{
			cgi->buildAndSendResponse();
		}
		else if (reason == CGI_INTERNAL_ERROR)
		{
			HttpResponse res(conn);
			res.statusCode(502, "Bad Gateway");
			res.body("Bad Gateway\n");
			res.send(ResponseHTTPVersion::HTTP_1_1);
		}
		else if (reason == CGI_TIMEOUT)
		{
			HttpResponse res(conn);
			res.statusCode(504, "Gateway Timeout");
			res.body("Gateway Timeout\n");
			res.send(ResponseHTTPVersion::HTTP_1_1);
		}

		::kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);

		if (!cgi->isStdinClosed())
			_multiplexer->remove(cgi->stdinPipe());
		_multiplexer->remove(cgi->stdoutPipe());

		_running_cgis.erase(it);
		delete cgi;

		if (reason == CGI_CLIENT_GONE)
			return;
		_removePending(conn);
        if (!conn->hasPendingWrite())
            _multiplexer->remove(conn);
	}

	void _setMultiplexer(IMultiplexer *multiplexer)
	{
		_multiplexer = multiplexer;
	}

	IMultiplexer	*_getMultiplexer()
	{
		return _multiplexer;
	}

	bool	_addListenner(Socket *socket, Server *server)
	{
		if (!socket || !socket->isValid()) return false;
		_multiplexer->add(socket);
		_http_request_observers.addSocketToObserver(socket, server);
		return true;
	}

	bool _addFileDescriptor(FileDescriptor *file_descriptor)
	{
		if (!file_descriptor || !file_descriptor->isValid())
			return false;
		_multiplexer->add(file_descriptor);
		return true;
	}

	void _addCgi(CgiProcess *cgi)
	{
		if (!cgi) return;
		_multiplexer->add(cgi->stdinPipe());
		_multiplexer->add(cgi->stdoutPipe());
		_running_cgis.push_back(cgi);
	}

public:
	ConnectionPool(): _multiplexer(NULL) { }
	~ConnectionPool() {}

	static void	multiplexer(IMultiplexer *multiplexer)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._setMultiplexer(multiplexer);
	}

	static void	addListenner(Socket *socket, Server *server)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._addListenner(socket, server);
	}

	static void addFileDescriptor(FileDescriptor *file_descriptor)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._addFileDescriptor(file_descriptor);
	}

	static void removeFileDescriptor(FileDescriptor *file_descriptor)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._getMultiplexer()->remove(file_descriptor);
		delete file_descriptor;
	}

	static void untrackFileDescriptor(FileDescriptor *file_descriptor)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._getMultiplexer()->remove(file_descriptor);
	}

	static void addCgi(CgiProcess *cgi)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._addCgi(cgi);
	}

	// static void removeCgi(CgiProcess *cgi)
	// {
	// 	ConnectionPool	&instance = ConnectionPool::getInstance();
	// 	instance._getMultiplexer()->remove(file_descriptor);
	// 	delete file_descriptor;
	// }

	void	waitConnections()
	{
		std::cout << "Waitting for connections!\n";
		while (!g_stop)
		{
			
			time_t now = time(NULL);
			std::list<CgiProcess*>::iterator cit = _running_cgis.begin();
			while (cit != _running_cgis.end())
			{
				if ((*cit)->isExpired(now, 30))
				{
					LOG_TRACE("CGI expired");
					std::list<CgiProcess*>::iterator victim = cit;
					++cit;
					_cleanupCgi(victim, CGI_TIMEOUT);
				}
				else
					++cit;
			}
			// _processPendingRequests();
			ConnectionEventList	events;
			std::string error = _multiplexer->wait(events);
			if (!error.empty())
				std::cerr << error << std::endl;
			if (events.empty()) continue;
			for (size_t i = 0; i < events.size(); i++)
			{
				ConnectionEvent	event = events[i];

				if (event.file_descriptor->getType() == FileDescriptorType::SOCKET_LISTENNER)
				{
					if (!event.error.empty())
					{
						std::cerr << event.error << std::endl;
						_multiplexer->remove(event.file_descriptor);
						continue;
					}
					SocketConnection	*connection = new SocketConnection(static_cast<Socket*>(event.file_descriptor));
					if (!connection->isValid()) { delete connection; continue; }
					ConnectionPool::addFileDescriptor(connection);
					continue;
				}
				else if (event.file_descriptor->getType() == FileDescriptorType::SOCKET_CONNECTION)
				{
					SocketConnection	*conn = static_cast<SocketConnection*>(event.file_descriptor);
					if (!event.error.empty() || event.eof)
					{
						std::list<CgiProcess*>::iterator cit = _findCgiByClient(conn);
						if (cit != _running_cgis.end())
							_cleanupCgi(cit, CGI_CLIENT_GONE);

						_multiplexer->remove(conn);
						continue;
					}

					// ha uma resposta (headers e/ou arquivo) ainda sendo drenada pro
					// socket. so tenta escrever de novo quando o multiplexer avisar
					// que o fd esta gravavel; nao mistura com leitura de novo request
					// enquanto a resposta atual nao terminou de sair.
					if (conn->hasPendingWrite())
					{
						if (event.writable)
							conn->flushWrite();
						if (!conn->hasPendingWrite() && !_isConnectionOwnedByCgi(conn))
							_multiplexer->remove(conn);
						continue;
					}

					RequestBuilder *existing = _findPending(conn);
					if (existing)
					{
						if (_handleRequest(*existing))
						{
							_removePending(conn);
							if (!conn->hasPendingWrite() && !_isConnectionOwnedByCgi(conn))
        						_multiplexer->remove(conn);
						}
					}
					else
					{
						Server *server = _http_request_observers.findServer(conn->listenner());
						const WebServerConfig *config = server ? server->getConfig() : NULL;
						_pending_request.push_back(RequestBuilder(conn, config));
						if (_handleRequest(_pending_request.back()))
						{
							_pending_request.pop_back();
							if (!conn->hasPendingWrite() && !_isConnectionOwnedByCgi(conn))
        						_multiplexer->remove(conn);
						}
					}
				}
				else if (event.file_descriptor->getType() == FileDescriptorType::PIPE_READ)
				{
					std::list<CgiProcess*>::iterator cit = _findCgiByPipe(event.file_descriptor);
					if (cit == _running_cgis.end())
					{
						LOG_TRACE("CGI pipe read not found: " << event.file_descriptor->fd());
						_multiplexer->remove(event.file_descriptor);
						continue;
					}
					LOG_TRACE("PIPE_READ: " << event);
					if (event.readable || event.eof)
						(*cit)->onStdoutReadable();

					if ((*cit)->isDone())
						_cleanupCgi(cit, CGI_NORMAL);
				}
				else if (event.file_descriptor->getType() == FileDescriptorType::PIPE_WRITE)
				{
					std::list<CgiProcess*>::iterator cit = _findCgiByPipe(event.file_descriptor);
					if (cit == _running_cgis.end())
					{
						LOG_TRACE("CGI pipe write not found: " << event.file_descriptor->fd());
						_multiplexer->remove(event.file_descriptor);
						continue;
					}
					LOG_TRACE("PIPE_WRITE: " << event);
					if (event.writable)
						(*cit)->onStdinWritable();
					if (!(*cit)->isStdinClosed() && (*cit)->stdinWriteFinished())
					{
						LOG_TRACE("CGI write finished. Closing.");
						_multiplexer->remove((*cit)->stdinPipe()); // fecha o fd -> EOF pro filho
						(*cit)->markStdinClosed();
					}
				}
				else
				{
					std::cerr << "unknown socket type " << event.file_descriptor->getType() << "\n";
				}
			}
			_multiplexer->flushRemovals();
		}
	}
};

#endif
