# include "Cgi.hpp"
# include "Router.hpp"


CgiEnv::CgiEnv(const Router &router, utils::str script_path): _router(router), _script_path(script_path), _data() { _init(); }
CgiEnv::~CgiEnv()
{
	for (size_t i = 0; i < _data.size(); i++)
		delete _data[i];
};

char* CgiEnv::_envEntry(const utils::str& key, const utils::str& value)
{
	utils::str s = key + "=" + value;
	char* p = new char[s.size() + 1];
	std::strcpy(p, s.c_str());
	return p;
}

std::string CgiEnv::_toHttpEnvKey(const std::string &header_name)
	{
		std::string result = "HTTP_";
		for (size_t i = 0; i < header_name.size(); ++i)
		{
			char c = header_name[i];
			if (c == '-')
				result += '_';
			else if (c >= 'a' && c <= 'z')
				result += (c - 'a' + 'A');
			else
				result += c;
		}
		return result;
	}

void	CgiEnv::_init()
{
	_data.push_back(_envEntry("REQUEST_METHOD", RequestMethodStr[_router.req.method]));
	_data.push_back(_envEntry("QUERY_STRING", _router.req.path.getQueryString()));
	_data.push_back(_envEntry("CONTENT_LENGTH", utils::to_string(_router.req.body.size())));
	_data.push_back(_envEntry("CONTENT_TYPE", _router.req.headers.content_type()));
	_data.push_back(_envEntry("SCRIPT_FILENAME", _script_path));
	_data.push_back(_envEntry("SCRIPT_NAME", _router.req.path.getCleanPath()));
	_data.push_back(_envEntry("PATH_INFO", ""));
	_data.push_back(_envEntry("SERVER_PROTOCOL", "HTTP/1.1"));
	_data.push_back(_envEntry("GATEWAY_INTERFACE", "CGI/1.1"));
	_data.push_back(_envEntry("SERVER_NAME", _router.req.headers.host()));
	_data.push_back(_envEntry("SERVER_PORT", utils::to_string(_router.req.port)));
	_data.push_back(_envEntry("REDIRECT_STATUS", "200"));
	for (HttpHeaders::const_iterator it = _router.req.headers.begin(); it != _router.req.headers.end(); ++it)
	{
		if (it->first == "Content-Type" || it->first == "Content-Length")
			continue;
		_data.push_back(_envEntry(_toHttpEnvKey(it->first), it->second));
	}
	_data.push_back(NULL);
}

std::vector<char *>	CgiEnv::data() const
{
	return _data;
}
