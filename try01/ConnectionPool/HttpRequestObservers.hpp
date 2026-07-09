#ifndef HTTP_REQUEST_OBSERVER_HPP
# define HTTP_REQUEST_OBSERVER_HPP

# include <map>

# include "Server.hpp"
# include "Socket.hpp"
# include "HttpRequestBuilder.hpp"

class HttpRequestObservers
{
	private:
		std::map<const Socket*, Server*>	_observers;

	public:
		HttpRequestObservers() {}
		~HttpRequestObservers() {}

		void	addSocketToObserver(const Socket *socket, Server *server)
		{
			_observers[socket] = server;
		}
		Server	*findServer(const Socket *listener) const
		{
			std::map<const Socket*, Server*>::const_iterator it = _observers.find(listener);
			return (it == _observers.end()) ? NULL : it->second;
		}
		bool	notifyRequest(HttpRequestBuilder &req_builder)
		{
			std::map<const Socket*, Server*>::iterator list_it = _observers.find(req_builder.connection->listenner());
			if (list_it == _observers.end())
			{
				std::cerr << "Error on handling request, listenner '" << req_builder.connection->listenner()->fd() << "' not found!" << std::endl;
				return false;
			}
			HttpRequest		req = req_builder.build();
			HttpResponse	res(req_builder.connection);
			list_it->second->handleRequest(req, res);
			return true;
		}
};

#endif
