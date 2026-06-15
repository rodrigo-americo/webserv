#ifndef EVENTSCONFIG_HPP
# define EVENTSCONFIG_HPP

# include "ConfigNode.hpp"
# include "http.hpp"

class EventsConfig: public ConfigLeaf{
    private:
        EventsConfig(const EventsConfig &other);
        EventsConfig &operator=(const EventsConfig &other);
        bool _multi_accept;
        IOMultiplexer _use;
        size_t _worker_connections;
    public:
        EventsConfig() : _multi_accept(false), _use(IO_SELECT), _worker_connections(0) {}
        void setMultiAccept(bool multi_accept)           { _multi_accept = multi_accept; }
        void setUse(IOMultiplexer use)                   { _use = use; }
        void setWorkerConnections(size_t connections)    { _worker_connections = connections; }

        bool           getMultiAccept() const            { return _multi_accept; }
        IOMultiplexer  getUse() const                    { return _use; }
        size_t         getWorkerConnections() const      { return _worker_connections; }
};

#endif