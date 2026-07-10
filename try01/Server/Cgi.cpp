# include "Cgi.hpp"
# include "CgiProcess.hpp"
# include "ConnectionPool.hpp"
# include "Logger.hpp"

Cgi::Cgi(const Router &router)
	: _router(router), _pipe_in(NULL), _pipe_out(NULL),
	_script_path(((router.config_location ? router.config_location->resolveRoot() : Path(utils::str(""))) + router.req.path).getPath()),
	_env(router, _script_path.path().getCleanPath()), _interpreter() { _init(); };
Cgi::~Cgi() 
{
	delete _pipe_in;
	delete _pipe_out;
};

void	Cgi::_init()
{
	if (!_router.config_location) return;
	const std::map<std::string, std::string>& cgi_map = _router.config_location->getCgiExtensions();
	std::map<std::string, std::string>::const_iterator it = cgi_map.find(_router.req.path.getExtension().string());
	if (it != cgi_map.end())
		_interpreter = it->second;
}

bool	Cgi::_settupError()
{
	if (_interpreter.empty())
	{
		_router.error.internalServerError();
		return true;
	}
	if (!_script_path.exists())
	{
		_router.error.notFound();
		return true;
	}
	if (!_script_path.isFile())
	{
		_router.error.forbiden();
		return true;
	}


	if (_pipe_in->error)
	{
		_router.error.internalServerError();
		return true;
	}
	if (_pipe_out->error)
	{
		_pipe_in->close();
		_router.error.internalServerError();
		return true;
	}
	return false;
}

void	Cgi::_execute() const
{
	// Logger::levelTrace();
	LOG_TRACE("CHILD_PROCESS: execute CGI closing " << *_pipe_in->write << ", " << *_pipe_out->read);
	if (_pipe_in->write->close() == -1 || _pipe_out->read->close()  == -1)
	{
		LOG_ERROR("CHILD_PROCESS: execute CGI closing pipe Error: " << *_pipe_in->write << ", " << *_pipe_out->read);
		_exit(1);
	}
	LOG_TRACE("CHILD_PROCESS: execute CGI dup2 " << *_pipe_in->read << " to " << STDIN_FILENO);
	if (_pipe_in->read->dup2(STDIN_FILENO) == -1)
	{
		LOG_ERROR("CHILD_PROCESS: execute CGI dup2 Error on " << *_pipe_in->read << " to " << STDIN_FILENO);
		_exit(1);
	}
	LOG_TRACE("CHILD_PROCESS: execute CGI dup2 " << *_pipe_out->write << " to " << STDOUT_FILENO);
	if (_pipe_out->write->dup2(STDOUT_FILENO) == -1)
	{
		LOG_ERROR("CHILD_PROCESS: execute CGI dup2 Error on " << *_pipe_in->write << " to " << STDOUT_FILENO);
		_exit(1);
	}
	LOG_TRACE("CHILD_PROCESS: execute CGI CD to " << _script_path.path().getLastDir());
	if (chdir(_script_path.path().getLastDir().c_str()) == -1)
	{
		LOG_ERROR("execve CGI error on CD to " << _script_path.path().getLastDir());
		_exit(1);
	}
	// LOG_TRACE("execve CGI building argv.");
	std::vector<char *> argv;
	argv.push_back(const_cast<char *>(_interpreter.c_str()));
	argv.push_back(const_cast<char *>(_router.req.path.getFilename().c_str()));
	argv.push_back(NULL);
	// LOG_TRACE("execve CGI child process.");
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
	fcntl(STDOUT_FILENO, F_SETFL, fcntl(STDOUT_FILENO, F_GETFL) & ~O_NONBLOCK);
	execve(_interpreter.c_str(), &argv[0], &_env.data()[0]);
	// LOG_ERROR("execve CGI Error.");
	_exit(1);
}

void	Cgi::createProcess()
{
	_pipe_in = new Pipe();
	_pipe_out = new Pipe();
	LOG_TRACE("settinup CGI process.");
	if (_settupError())
	{
		LOG_ERROR("settup CGI Error.");
		return;
	}

	LOG_TRACE("creatting CGI child process.");
	int pid_child = fork();
	if (pid_child == -1)
	{
		LOG_ERROR("CGI fork error.");
		_pipe_in->close();
		_pipe_out->close();
		return _router.error.internalServerError();
	}
	// Logger::levelFatal();
	if(pid_child == 0)
		_execute();
	if (pid_child != 0)
	{
		_pipe_in->closeRead();
		_pipe_out->closeWrite();

		LOG_TRACE("adding CGI process to Connection Pool.");
		CgiProcess		*process = new CgiProcess(_router.res.getConn(), _router.req, _pipe_in->write, _pipe_out->read, pid_child);
		ConnectionPool::getInstance().addCgi(process);
	}
}
