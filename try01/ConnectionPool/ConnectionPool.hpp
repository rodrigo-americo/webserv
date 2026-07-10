

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <set>
# include <vector>
# include <signal.h>
# include <sys/wait.h>
# include <ctime>

# include "signal.hpp"
# include "singleton.hpp"
# include "IMultiplexer.hpp"
# include "Server.hpp"
# include "Router.hpp"
# include "HttpRequestBuilder.hpp"
#include "CgiProcess.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "Logger.hpp"
# include "HttpRequestsManager.hpp"


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
	HttpRequestsManager			_requests;

	ConnectionPool()
		: _multiplexer(NULL), _requests() {}

	void	_setGlobalConfig(WebServerConfig *config)
	{
		_requests.setGlobalConfig(config);
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
		_requests.observeSocket(socket, server);
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
		_requests.addCgi(cgi);
	}

	void _removeCgi(CgiProcess *cgi)
	{
		_requests.removeCgi(cgi);
	}

public:
	~ConnectionPool() {}

	HttpRequestsManager	&requests() { return _requests; }

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
		// delete file_descriptor;
	}

	static void addCgi(CgiProcess *cgi)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._addCgi(cgi);
	}

	static void removeCgi(CgiProcess *cgi)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._removeCgi(cgi);
	}

	void	waitConnections()
	{
		std::cout << "Waitting for connections!\n";
		while (!g_stop)
		{

			_requests.checkTimeout();

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
						ConnectionPool::removeFileDescriptor(event.file_descriptor);
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
						CgiProcess *cgi = _requests.findCgiByConnection(conn);
						delete cgi;
						ConnectionPool::removeFileDescriptor(conn);
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
						if (!conn->hasPendingWrite())
							ConnectionPool::removeFileDescriptor(conn);
						continue;
					}

					HttpRequestBuilder *existing = _requests.findPending(conn);
					if (existing)
						_requests.buildRequest(*existing);
					else
						_requests.buildRequest(conn);
				}
				else if (event.file_descriptor->getType() == FileDescriptorType::PIPE_READ)
				{
					CgiProcess	*cgi = _requests.findCgiByConnectionEvent(event);
					if (cgi)
					{
						if (event.readable || event.eof)
							cgi->onStdoutReadable();
						if (cgi->isDone())
							cgi->buildAndSendResponse();
					}
					LOG_TRACE("CGI pipe read not found: " << event.file_descriptor->fd());
				}
				else if (event.file_descriptor->getType() == FileDescriptorType::PIPE_WRITE)
				{
					CgiProcess	*cgi = _requests.findCgiByConnectionEvent(event);
					if (cgi)
					{
						LOG_TRACE("PIPE_WRITE: " << event);
						if (event.writable)
							cgi->onStdinWritable();
						if (!cgi->isStdinClosed() && cgi->stdinWriteFinished())
						{
							LOG_TRACE("CGI write finished. Closing.");
							ConnectionPool::removeFileDescriptor(cgi->stdinPipe());
							cgi->markStdinClosed();
						}
					}
					LOG_TRACE("CGI pipe read not found: " << event.file_descriptor->fd());
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
