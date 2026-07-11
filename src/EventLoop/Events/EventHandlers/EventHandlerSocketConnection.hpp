#ifndef EVENT_HANDLER_SOCKET_LISTENNER_HPP
# define EVENT_HANDLER_SOCKET_LISTENNER_HPP

# include "EventHandler.hpp"

struct EventHandlerSocketConnection: public EventHandler, public patterns::factory_register<EventHandlerSocketConnection, EventHandlerFactory>
{
	EventHandlerSocketConnection();
	void			handle(const ConnectionEvent &event);
	EventHandler	*copy();
};

#endif
