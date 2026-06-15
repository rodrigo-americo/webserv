#ifndef HTTP_HPP
# define HTTP_HPP

enum HttpMethod {
    GET,
    POST,
    DELETE,
};


enum LocationModifier{
    MOD_NONE,         // sem modificador
    MOD_EXACT,        // =
	MOD_REGEX,        // ~
	MOD_REGEX_CI,     // ~*
	MOD_PREFIX,       // ^~
};

enum IOMultiplexer{
    IO_SELECT,
    IO_POLL,
    IO_EPOLL,
    IO_KQUEUE,
};

#endif