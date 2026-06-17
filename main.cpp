#include <iostream>
#include "parser/parser/ParserBuilder.hpp"
#include "config/ConfigBuilderVisitor.hpp"
// try01
#include "MultiplexerEpoll.hpp"
#include "MultiplexerPoll.hpp"
#include "MultiplexerSelect.hpp"
#include "ConnectionPool.hpp"
#include "Server.hpp"
#include "SocketListenner.hpp"


IMultiplexer* createMultiplexer(const WebServerConfig* config) {
    const GlobalConfig* global = config->getGlobal();
    const EventsConfig* events = global ? global->getEvents() : NULL;
    IOMultiplexer type = events ? events->getUse() : IO_EPOLL;

    if (type == IO_POLL)  return new MultiplexerPoll();
    if (type == IO_EPOLL) return new MultiplexerEpoll();
    return new MultiplexerSelect();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    ParserAst ast = ParserBuilder::defaultBuilder()
        .withFile(argv[1])
        .build();

    ast.validateStructure();
    if (ast.hasError())
    {
        ast.printErrors();
        return 1;
    }

    ConfigBuilderVisitor visitor;
    ast.applyVisitor(visitor);

    WebServerConfig* config = visitor.getResult();
    if (!config)
    {
        std::cerr << "Erro: configuração inválida" << std::endl;
        return 1;
    }

    if (config->hasErrors())
    {
        std::cerr << config->formatErrors();
        delete config;
        return 1;
    }
	size_t worker_connections = config->getWorkerConnections();
	IMultiplexer* mult = createMultiplexer(config);
	ConnectionPool &pool = ConnectionPool::getInstance();
	pool.setMultiplexer(mult);

	const std::list<ServerConfig*>& servers = config->getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end();it++){
		Server* srv = new Server();
		const std::list<ConfigServerListen>& listens = (*it)->getListen();
		for (std::list<ConfigServerListen>::const_iterator lit = listens.begin(); lit != listens.end(); lit++){
			SocketListenner* sock = new SocketListenner(*lit, worker_connections);
			if (sock->hasErrors()) { delete sock; continue; }
			pool.addListenner(sock, srv);
		}
	}
	pool.waitConnections();
    delete config;
    return 0;
}
