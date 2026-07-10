

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
#include "EventHandler.hpp"


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

	void _setMultiplexer(IMultiplexer *multiplexer)
	{
		_multiplexer = multiplexer;
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

	void	_removeFileDescriptor(FileDescriptor *file_descriptor)
	{
		_multiplexer->remove(file_descriptor);
		if (file_descriptor->getType() == FileDescriptorType::SOCKET_CONNECTION)
			_requests.removeActiveConnection(dynamic_cast<SocketConnection*>(file_descriptor));
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

	CgiProcess * _findCgiByConnection(SocketConnection *conn)
	{
		return _requests.findCgiByConnection(conn);
	}

	CgiProcess * _findCgiByConnectionEvent(const ConnectionEvent &event)
	{
		return _requests.findCgiByConnectionEvent(event);
	}

	void _buildRequest(SocketConnection *conn)
	{
		_requests.buildRequest(conn);
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
		instance._removeFileDescriptor(file_descriptor);
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

	static CgiProcess * findCgiByConnection(SocketConnection *conn)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		return instance._findCgiByConnection(conn);
	}

	static CgiProcess * findCgiByConnectionEvent(const ConnectionEvent &event)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		return instance._findCgiByConnectionEvent(event);
	}

	static void buildRequest(SocketConnection *conn)
	{
		ConnectionPool	&instance = ConnectionPool::getInstance();
		instance._buildRequest(conn);
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
			{
				LOG_FATAL(error);
				g_stop = 1;
			}
			if (events.empty()) continue;
			for (size_t i = 0; i < events.size(); i++)
			{
				ConnectionEvent	event = events[i];
				EventHandler	*handler = EventHandlerFactory::create(event);
				if (!handler) { std::cerr << "unknown socket type " << event.file_descriptor->getType() << "\n"; continue; }
				handler->handle(event);
				delete handler;
			}
			_multiplexer->flushRemovals();
		}
	}
};

#endif
