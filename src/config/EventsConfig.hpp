#ifndef EVENTSCONFIG_HPP
# define EVENTSCONFIG_HPP

# include "ConfigNode.hpp"
# include "http.hpp"

class EventsConfig: public ConfigLeaf{
    private:
        EventsConfig(const EventsConfig &other);
        EventsConfig &operator=(const EventsConfig &other);
        IOMultiplexer _use;
        size_t _worker_connections;
    public:
        EventsConfig() : _use(IO_SELECT), _worker_connections(0) {}
        void setUse(IOMultiplexer use)                   { _use = use; }
        void setWorkerConnections(size_t connections)    { _worker_connections = connections; }

        IOMultiplexer  getUse() const                    { return _use; }
        size_t         getWorkerConnections() const      { return _worker_connections; }
};

#endif