
#ifndef HTTP_RESPONSE_ERROR_HPP
# define HTTP_RESPONSE_ERROR_HPP

# include "str.hpp"
# include "Logger.hpp"
# include "utils.hpp"
# include "HttpResponse.hpp"
# include "FileSystem.hpp"
# include "ServerConfig.hpp"

class HttpResponseError: public	HttpResponse
{
private:
	const ServerConfig* _server_config;

	utils::str	_defaultErrorPagePath() const { return "www/error/"; }

	utils::str	_errorPagePath() const
	{
		return utils::to_string(status_code) + ".html";
	}

	utils::str	_genericErrorPagePath() const
	{
		utils::str	status_str = utils::to_string(status_code).substr(0, 2);
		return status_str + "x" + ".html";
	}

	void	_addFileContent(const utils::str &name)
	{
		LOG_DEBUG("adding file content of " << name);
		std::ifstream file(name.c_str());
		if (file.is_open())
		{
			std::ostringstream ss;
			ss << file.rdbuf();
			body(ss.str());
		}
	}

	void	_defaultErrorPage()
	{
		LOG_DEBUG("sending default error: ");
		FileSystem	fs(_defaultErrorPagePath());
		LOG_DEBUG("path " << fs.path().getPath() << " exists: " << fs.exists());
		if (!fs.exists())
			return _errorMessage();

		std::vector<FileSystem> children = fs.ls();
		LOG_DEBUG("children size " << children.size());
		if (children.empty())
			return _errorMessage();
		utils::str error_page;
		if (fs.hasChild((error_page = _errorPagePath())))
			return _addFileContent(_defaultErrorPagePath() + error_page);
		if (fs.hasChild((error_page = _genericErrorPagePath())))
			return _addFileContent(_defaultErrorPagePath() + error_page);
		return _errorMessage();
	}

	void	_errorPage()
	{
		const std::map<int, std::string>& pages = _server_config->getErrorPages();
		LOG_DEBUG("pages size: " << pages.size());
		std::map<int, std::string>::const_iterator it = pages.find(status_code);
		if (it == pages.end())
			return _defaultErrorPage();
		_addFileContent(it->second);
	}

	void	_errorMessage()
	{
		body(status_code_message + "\n");
	}

	void	_buildError()
	{
		LOG_DEBUG("has serber config: " << !!_server_config);
		if (!_server_config)
			return _defaultErrorPage();
		_errorPage();
	}

public:
	HttpResponseError(const HttpResponse &res, int code, const utils::str &msg, const ServerConfig* server)
		: HttpResponse(res), _server_config(server)
	{
		status_code = code;
		status_code_message = msg.string();
		_buildError();
	};
	~HttpResponseError() {};
};

#endif
