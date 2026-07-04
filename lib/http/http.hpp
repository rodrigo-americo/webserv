#ifndef HTTP_HPP
# define HTTP_HPP

enum HttpMethod {
    GET,
    POST,
    DELETE,
};

enum IOMultiplexer{
    IO_SELECT,
    IO_POLL,
    IO_EPOLL,
    IO_KQUEUE,
};

#endif