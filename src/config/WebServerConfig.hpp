#ifndef WEBSERVERCONFIG_HPP
# define WEBSERVERCONFIG_HPP

# include <list>
# include "ConfigNode.hpp"
# include "GlobalConfig.hpp"
# include "singleton.hpp"

class HttpRequest;

class WebServerConfig : public ConfigContainer, public patterns::singleton<WebServerConfig>
{
	friend class patterns::singleton<WebServerConfig>;
	private:
		WebServerConfig(const WebServerConfig& other);
		WebServerConfig& operator=(const WebServerConfig& other);
		GlobalConfig* _global;

		WebServerConfig() : _global(NULL) {}
		~WebServerConfig() { delete _global; }

		const GlobalConfig* _getGlobal() const { return _global; }

		const std::list<ServerConfig*>& _getServers() const
		{
			static const std::list<ServerConfig*> empty;
			if (!_global || !_global->getHttp())
				return empty;
			return _global->getHttp()->getServers();
		}

		size_t _getWorkerConnections() const
		{
			if (!_global || !_global->getEvents()) return 1024;
			size_t wc = _global->getEvents()->getWorkerConnections();
			return wc ? wc : 1024;
		}

		const ServerConfig*	_match_server(const HttpRequest& req) const;
		void				addChild(ConfigNode* child);

	public:

		static const GlobalConfig* getGlobal()
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			return instance._getGlobal();
		}

		static const std::list<ServerConfig*>& getServers()
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			return instance._getServers();
		}
		static size_t getWorkerConnections()
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			return instance._getWorkerConnections();
		}

		static const ServerConfig* match_server(const HttpRequest& req)
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			return instance._match_server(req);
		};
		static void appendChild(ConfigNode* child)
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			instance.addChild(child);
		};

		static void appendError(const std::string& msg)
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			instance.addError(msg);
		};

		static	bool has_errors()
		{
			WebServerConfig	&instance = WebServerConfig::getInstance();
			return instance.hasErrors();
		}
};

#endif
