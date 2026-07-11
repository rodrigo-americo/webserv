#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

# include <map>
# include <string>
# include <fstream>
# include <sstream>
# include <iomanip>

# include "singleton.hpp"

static inline std::string generateSessionId()
{
    unsigned char buf[16];
    std::ifstream urandom("/dev/urandom", std::ios::binary);
    urandom.read(reinterpret_cast<char*>(buf), sizeof(buf));
    if (!urandom)
        return "";
    std::ostringstream oss;
    for (size_t i = 0; i < sizeof(buf); ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buf[i]);
    return oss.str();
}

class SessionManager: public patterns::singleton<SessionManager>
{
    friend class patterns::singleton<SessionManager>;
private:
    std::map<std::string, time_t> _sessions;
    SessionManager() {};
    ~SessionManager() {};
public:
    bool isValid(const std::string& key, time_t timeout_secs = 15)
    {
        std::map<std::string, time_t>::iterator it = _sessions.find(key);
        if (it == _sessions.end())
            return false;
        if (time(NULL) - it->second > timeout_secs)
        {
            _sessions.erase(it);
            return false;
        }
        return true;
    }
    std::string create()
    {
        std::string id = generateSessionId();
        if (id.size() == 0)
            return "";
        _sessions[id] = time(NULL);
        return id;
    }
};




#endif