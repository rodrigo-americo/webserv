#ifndef EVENT_HANDLER_SOCKET_LISTENNER_HPP
# define EVENT_HANDLER_SOCKET_LISTENNER_HPP

# include "EventHandler.hpp"

struct EventHandlerPipeRead: public EventHandler, public patterns::factory_register<EventHandlerPipeRead, EventHandlerFactory>
{
	EventHandlerPipeRead();
	void			handle(const ConnectionEvent &event);
	EventHandler	*copy();
};

#endif
