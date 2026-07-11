

#ifndef CGIRESPONSEPARSE_HPP
#define CGIRESPONSEPARSE_HPP


#include "str.hpp"


class CgiResponseParse
{
private:
    utils::str _raw;
    bool _malformed;
    int _status_code;
    utils::str _status_message;
    size_t _delim_pos;
    size_t _delim_size;
    std::vector<std::pair<utils::str, utils::str> > _headers;
    utils::str _body;
    void _setBoolMalformed()
    {
        size_t rnrn_pos = _raw.find("\r\n\r\n");
        size_t nn_pos = _raw.find("\n\n");

        if (rnrn_pos != utils::str::npos && (nn_pos == utils::str::npos || rnrn_pos <= nn_pos))
        { 
            _delim_pos = rnrn_pos;
            _delim_size = 4;
        }
        else if (nn_pos != utils::str::npos)
        {
            _delim_pos = nn_pos;
            _delim_size = 2;
        }
        if (_delim_pos == utils::str::npos)
            _malformed = true;
    };
public:
    CgiResponseParse(const utils::str& raw) : _raw(raw), _malformed(false), _status_code(200), _status_message("OK"), _delim_pos(utils::str::npos), _delim_size(0)
    {
        _setBoolMalformed();
        if (_malformed)
            return;
        
        utils::str header = raw.substr(0, _delim_pos);
        _body = raw.substr(_delim_pos + _delim_size);

        std::istringstream iss(header.string());
        std::string line;
        while (std::getline(iss, line))
        {
            if (!line.empty() && line[line.size()-1] == '\r') line.erase(line.size()-1);
            if (line.empty()) continue;
            size_t colon = line.find(':');
            if (colon == utils::str::npos) 
                continue;

            utils::str key = line.substr(0, colon);
            utils::str value = line.substr(colon + 1);
            value.trim();

            if (key.tolower() == "status")
            {
                size_t space = value.find(' ');
                if (space != utils::str::npos)
                {
                    char* endptr;
                    utils::str code_str = value.substr(0, space);
                    long code = std::strtol(code_str.c_str(), &endptr, 10);
                    if (endptr != code_str.c_str())
                    {
                        _status_code = static_cast<int>(code);
                        _status_message = value.substr(space + 1);
                    }
                }
            }
            else
                _headers.push_back(std::make_pair(key, value));
        }
    };
    ~CgiResponseParse() {};
    bool isMalformed()  const  { return _malformed; }
    int getStatusCode() const { return _status_code; }
    utils::str getStatusMsg()  const { return _status_message; }
    utils::str getBody()  const { return _body; }
    const std::vector<std::pair<utils::str, utils::str> >& getHeaders() const { return _headers; }
};



#endif


