#ifndef WEBSERVERCONFIG_HPP
# define WEBSERVERCONFIG_HPP

# include <list>
# include "ConfigNode.hpp"
# include "GlobalConfig.hpp"

class WebServerConfig : public ConfigContainer {
	private:
		WebServerConfig(const WebServerConfig& other);
		WebServerConfig& operator=(const WebServerConfig& other);
		GlobalConfig* _global;
	public:
		WebServerConfig() : _global(NULL) {}
		~WebServerConfig() { delete _global; }

		const GlobalConfig* getGlobal() const { return _global; }

		const std::list<ServerConfig*>& getServers() const {
			static const std::list<ServerConfig*> empty;
			if (!_global || !_global->getHttp())
				return empty;
			return _global->getHttp()->getServers();
		}
		size_t getWorkerConnections() const {
			if (!_global || !_global->getEvents()) return 1024;
			size_t wc = _global->getEvents()->getWorkerConnections();
			return wc ? wc : 1024;
		}
		const ServerConfig* match_server(size_t port, const std::string& host_header) const;
		void addChild(ConfigNode* child);
};

#endif
