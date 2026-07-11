#include "EventHandlerSocketConnection.hpp"
#include "ConnectionPool.hpp"

EventHandlerSocketConnection::EventHandlerSocketConnection(): EventHandler(FileDescriptorType::SOCKET_CONNECTION) {};

EventHandler	*EventHandlerSocketConnection::copy()
{
	return new EventHandlerSocketConnection();
}

void			EventHandlerSocketConnection::handle(const ConnectionEvent &event)
{
	SocketConnection	*conn = static_cast<SocketConnection*>(event.file_descriptor);
	if (!event.error.empty() || event.eof)
	{
		CgiProcess *cgi = ConnectionPool::findCgiByConnection(conn);
		delete cgi;
		ConnectionPool::removePending(conn);
		ConnectionPool::removeFileDescriptor(conn);
		return;
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
		return;
	}
	ConnectionPool::buildRequest(conn);
}
