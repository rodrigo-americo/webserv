#ifndef CGI_HPP
# define CGI_HPP

# include "str.hpp"
# include "Pipe.hpp"
# include "FileSystem.hpp"
# include "HttpRequest.hpp"

class CgiProcess;
class ConnectionPool;
class Router;

class CgiEnv
{
private:
	const Router		&_router;
	utils::str			_script_path;
	std::vector<char *>	_data;

	static char* _envEntry(const utils::str& key, const utils::str& value);
	static std::string _toHttpEnvKey(const std::string &header_name);
	void	_init();

public:
	CgiEnv(const Router &router, utils::str script_path);
	~CgiEnv();

	std::vector<char *>	data() const;
};

class Cgi
{
private:
	const Router	&_router;
	Pipe			_pipe_in;
	Pipe			_pipe_out;
	FileSystem		_script_path;
	CgiEnv			_env;
	utils::str		_interpreter;

	void	_init();
	bool	_settupError();
	void	_execute() const;

public:
	Cgi(const Router &router);
	~Cgi();

	void	createProcess();


};

#endif
