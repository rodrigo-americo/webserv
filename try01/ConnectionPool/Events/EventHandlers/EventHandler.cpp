#include "EventHandler.hpp"


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
