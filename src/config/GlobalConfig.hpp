#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP

# include "ConfigNode.hpp"
# include "EventsConfig.hpp"
# include "HttpConfig.hpp"

class GlobalConfig: public ConfigContainer{
    private:
        GlobalConfig(const GlobalConfig &other);
        GlobalConfig &operator=(const GlobalConfig &other);
        HttpConfig*  _http;
        EventsConfig* _events;
    public:
        GlobalConfig(): _http(NULL), _events(NULL) {}
        ~GlobalConfig() {
            delete _http;
            delete _events;
        }

        const HttpConfig*    getHttp() const       { return _http; }
        const EventsConfig*  getEvents() const     { return _events; }

        void addChild(ConfigNode* child);
};

#endif
