#include "EventHandlerSocketListenner.hpp"
#include "ConnectionPool.hpp"

EventHandler	*EventHandlerSocketListenner::copy()
{
	return new EventHandlerSocketListenner();
}

void			EventHandlerSocketListenner::handle(const ConnectionEvent &event)
{
	if (!event.error.empty())
	{
		std::cerr << event.error << std::endl;
		ConnectionPool::removeFileDescriptor(event.file_descriptor);
		return;
	}
	SocketConnection	*connection = new SocketConnection(static_cast<Socket*>(event.file_descriptor));
	if (!connection->isValid()) { delete connection; return; }
	ConnectionPool::addFileDescriptor(connection);
}
