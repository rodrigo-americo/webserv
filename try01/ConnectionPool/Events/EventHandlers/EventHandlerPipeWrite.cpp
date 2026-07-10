#include "EventHandlerPipeWrite.hpp"
#include "ConnectionPool.hpp"

EventHandlerPipeWrite::EventHandlerPipeWrite(): EventHandler(FileDescriptorType::PIPE_WRITE) {};

EventHandler	*EventHandlerPipeWrite::copy()
{
	return new EventHandlerPipeWrite();
}

void			EventHandlerPipeWrite::handle(const ConnectionEvent &event)
{
	CgiProcess	*cgi = ConnectionPool::findCgiByConnectionEvent(event);
	if (cgi)
	{
		LOG_TRACE("PIPE_WRITE: " << event);
		if (event.writable)
			cgi->onStdinWritable();
		if (!cgi->isStdinClosed() && cgi->stdinWriteFinished())
		{
			LOG_TRACE("CGI write finished. Closing.");
			ConnectionPool::removeFileDescriptor(cgi->stdinPipe()); // fecha o fd -> EOF pro filho
			cgi->markStdinClosed();
		}
	}
	LOG_TRACE("CGI pipe read not found: " << event.file_descriptor->fd());
}
