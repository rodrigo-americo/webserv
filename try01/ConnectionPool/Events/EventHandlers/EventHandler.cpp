#include "EventHandler.hpp"

EventHandler::EventHandler(FileDescriptorType::type type): segregation::has_type<FileDescriptorType::type>(type) {};


EventHandlerFactory::EventHandlerFactory() {};
EventHandlerFactory::~EventHandlerFactory() {};

void			EventHandlerFactory::_addItem(EventHandler *handler)
{
	std::pair<iterator, bool> result = _items.insert(std::make_pair(handler->getType(), handler));
	if (!result.second)
		delete handler;
}

EventHandler	*EventHandlerFactory::_create(const ConnectionEvent &event)
{
	iterator	it = _items.find(event.file_descriptor->getType());
	if (it == _items.end())
		return NULL;
	return it->second->copy();
}
