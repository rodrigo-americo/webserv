#ifndef CGI_HPP
# define CGI_HPP

# include "Pipe.hpp"

class Router;

class Cgi
{
private:
	Router	&_router;
	Pipe	_pipe_in;
	Pipe	_pipe_out;

public:
	Cgi(Router &router): _router(router), _pipe_in(), _pipe_out() {};
	~Cgi() {};

	void	execute() const
	{
		/*
			fork ()
			{
			sss
			}
			if (==0)
			{
			exit(1)
			}
			waitpid()
			return status;
		*/
		(void)_router;
	}
};

#endif