# include "SocketEvent.hpp"
# include "Socket.hpp"
# include "utils.hpp"

std::ostream	&operator<<(std::ostream &os, const SocketEvent &ev)
{
	os << "PoolEvente("
		<< "fd: "
		<< ev.socket->fd()
		<< ", readable:  " << utils::to_string(ev.readable)
		<< ", writable: " << utils::to_string(ev.writable)
		<< ", eof: " << utils::to_string(ev.eof)
		<< (ev.error.empty() ? "" : ", error: ") << (ev.error.empty() ? "" : utils::to_string(ev.error))
		<< ")";
	return os;
}
