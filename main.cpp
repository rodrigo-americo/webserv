#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

int main(void){
	struct pollfd fds[10];
	int count = 0;
	int listen_fd = socket(AF_INET, SOCK_STREAM,0);
	if (listen_fd < 0){
		std::cerr << "socket() falhou" << std::endl;
		return (1);
	}
	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		std::cerr << "bind() falhou" << std::endl;
		return (1);
	}

	if (listen(listen_fd, 10) < 0){
		std::cerr << "listen() falhou" << std::endl;
		return (1);
	}

	std::cout << "Servidor ouvindo na porta 8080..." << std::endl;
	struct sockaddr_in client_addr;
	fds[0].fd = listen_fd;
	fds[0].events = POLLIN;
	count++;
	socklen_t client_len = sizeof(client_addr);
	while (true)
	{
		int ret = poll(fds, count, -1);
		if (ret < 0)
		{
			std::cerr << "poll() falhou" << std::endl;
			break;
		}
		for (int i = 0; i < count; i++){
			if (fds[i].fd == listen_fd && fds[i].revents & POLLIN){
				int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
				if (client_fd < 0){
					std::cerr << "accept() falhou" << std::endl;
					return (1);
				}
				fds[count].fd = client_fd;
				fds[count].events = POLLIN;
				count++;
				std::cout << "Cliente conectado! client_fd = " << client_fd << std::endl;
			}
			else if (fds[i].revents & POLLIN){
				char buffer[1024];
				std::memset(buffer, 0 , sizeof(buffer));
				int bytes_read = read(fds[i].fd, buffer, sizeof(buffer) - 1);
				std::cout << "Recebi " << bytes_read << " bytes:" << std::endl;
				std::cout << buffer << std::endl;
				const char* response =
					"HTTP/1.1 200 OK\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"Hello, World!";
				write(fds[i].fd, response, std::strlen(response));
				close(fds[i].fd);
				fds[i] = fds[count - 1];
				count--;
				i--;
			}
		}
	}
    close(listen_fd);
	return 0;
}
