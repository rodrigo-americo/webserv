#ifndef EVENT_HANDLER_SOCKET_LISTENNER_HPP
# define EVENT_HANDLER_SOCKET_LISTENNER_HPP

# include "EventHandler.hpp"

struct EventHandlerSocketListenner: public EventHandler, public patterns::factory_register<EventHandlerSocketListenner, EventHandlerFactory>
{
	EventHandlerSocketListenner();
	void			handle(const ConnectionEvent &event);
	EventHandler	*copy();
};

#endif
