#ifndef EVENT_HANDLER_SOCKET_LISTENNER_HPP
# define EVENT_HANDLER_SOCKET_LISTENNER_HPP

# include "EventHandler.hpp"

struct EventHandlerPipeWrite: public EventHandler, public patterns::factory_register<EventHandlerPipeWrite, EventHandlerFactory>
{
	EventHandlerPipeWrite();
	void			handle(const ConnectionEvent &event);
	EventHandler	*copy();
};

#endif
