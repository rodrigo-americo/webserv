#ifndef EVENT_HANDLER_HPP
# define EVENT_HANDLER_HPP

# include "ConnectionEvent.hpp"
# include "factory.hpp"
# include "FileDescriptor.hpp"
# include "has_type.hpp"

struct EventHandler: public segregation::has_type<FileDescriptorType::type>
{
	EventHandler(FileDescriptorType::type type);
	virtual ~EventHandler() {};
	virtual EventHandler	*copy() = 0;
	virtual void			handle(const ConnectionEvent &event) = 0;
};

class EventHandlerFactory: public patterns::factory<EventHandlerFactory, FileDescriptorType::type, EventHandler, const ConnectionEvent&>
{
	friend class patterns::singleton<EventHandlerFactory>;
	friend class patterns::factory<EventHandlerFactory, FileDescriptorType::type, EventHandler, const ConnectionEvent&>;

	EventHandlerFactory();
	~EventHandlerFactory();

	void			_addItem(EventHandler *handler);
	EventHandler	*_create(const ConnectionEvent&);

};

#endif
