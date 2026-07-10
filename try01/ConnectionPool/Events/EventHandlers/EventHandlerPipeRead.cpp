#include "EventHandlerPipeRead.hpp"
#include "ConnectionPool.hpp"

EventHandlerPipeRead::EventHandlerPipeRead(): EventHandler(FileDescriptorType::PIPE_READ) {};

EventHandler	*EventHandlerPipeRead::copy()
{
	return new EventHandlerPipeRead();
}

void			EventHandlerPipeRead::handle(const ConnectionEvent &event)
{
	CgiProcess	*cgi = ConnectionPool::findCgiByConnectionEvent(event);
	if (cgi)
	{
		if (event.readable || event.eof)
			cgi->onStdoutReadable();
		if (cgi->isDone())
			cgi->buildAndSendResponse();
	}
	LOG_TRACE("CGI pipe read not found: " << event.file_descriptor->fd());
}
