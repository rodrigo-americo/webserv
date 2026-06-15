#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP

# include <string>
# include <list>
# include "ConfigNode.hpp"
# include "EventsConfig.hpp"
# include "HttpConfig.hpp"

class GlobalConfig: public ConfigContainer{
    private:
        GlobalConfig(const GlobalConfig &other);
        GlobalConfig &operator=(const GlobalConfig &other);
        int          _workers;
        std::string  _error_log;
        int          _pid;
        HttpConfig*  _http;
        EventsConfig* _events;
    public:
        GlobalConfig(): _workers(0), _pid(0), _http(NULL), _events(NULL) {}
        ~GlobalConfig() {
            delete _http;
            delete _events;
        }

        void setWorkers(int workers)               { _workers = workers; }
        void setErrorLog(const std::string& path)  { _error_log = path; }
        void setPid(int pid)                       { _pid = pid; }

        int                  getWorkers() const    { return _workers; }
        const std::string&   getErrorLog() const   { return _error_log; }
        int                  getPid() const        { return _pid; }
        const HttpConfig*    getHttp() const       { return _http; }
        const EventsConfig*  getEvents() const     { return _events; }

        void addChild(ConfigNode* child);
};

#endif
