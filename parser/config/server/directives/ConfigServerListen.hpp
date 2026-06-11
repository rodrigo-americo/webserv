/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerListen.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:19:58 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 21:47:16 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_LISTEN_HPP
# define CONFIG_SERVER_LISTEN_HPP

# include <string>

struct ConfigServerListenKeepAlive
{
	enum type
	{
		OFF,	// Keepalive desabilitado (padrão)
		ON,		// Keepalive ativado com valores do kernel
		CUSTOM	// Keepalive ativado com valores explícitos
	};
};


struct ConfigServerListen
{
	typedef ConfigServerListenKeepAlive	KeepAlive;
	/// Endereço IP onde o socket vai fazer bind.
	/// Exemplos: "0.0.0.0" (todos IPv4), "::" (todos IPv6),
	/// "127.0.0.1" (loopback), "/var/run/nginx.sock" (unix socket).
	std::string address;

	/// Porta onde o socket vai escutar.
	/// Valor 0 indica que é um unix socket (is_unix = true).
	size_t port;

	/// Indica que é um unix domain socket (AF_UNIX).
	/// Quando true, address contém o caminho do socket
	/// e port é ignorado. Afeta a chamada socket().
	bool is_unix;

	bool is_ipv6;

	bool is_ipv4;

	// ----------------------------------------------------------------
	// Flags — configuradas antes de bind() salvo indicação contrária
	// ----------------------------------------------------------------

	/// Ativa TLS na conexão após accept().
	/// Não afeta o socket em si — o wrap TLS ocorre
	/// na conexão aceita, antes de passar ao handler HTTP.
	bool ssl;

	/// Ativa HTTP/2 via negociação ALPN durante o handshake TLS.
	/// Só tem efeito se ssl = true. Aplicado após accept().
	bool http2;

	/// Marca este listen como virtual host padrão.
	/// Usado pelo dispatcher quando nenhum server_name
	/// casa com o header Host da requisição. Não gera syscall.
	bool default_server;

	/// Ativa SO_REUSEPORT antes de bind().
	/// Permite que cada worker tenha seu próprio socket
	/// na mesma porta, evitando contenção no accept().
	bool reuseport;

	/// Ativa IPV6_V6ONLY antes de bind().
	/// Impede que um socket IPv6 aceite conexões IPv4.
	/// Só relevante quando address é "::".
	bool ipv6only;

	/// Ativa leitura do header PROXY Protocol após accept().
	/// O header é lido antes de passar a conexão ao handler HTTP,
	/// permitindo obter o IP real do cliente via proxy/load balancer.
	bool proxy_protocol;

	/// Ativa TCP_DEFER_ACCEPT antes de listen() (Linux only).
	/// O worker só é acordado quando dados chegam na conexão,
	/// evitando wakeups desnecessários em conexões vazias.
	bool deferred;

	/// Força bind explícito neste endereço antes de bind().
	/// Necessário quando o mesmo endereço/porta é compartilhado
	/// entre vhosts e um deles precisa de opções de socket distintas.
	bool bind;

	// ----------------------------------------------------------------
	// Pares chave=valor — configurados void setsockopt antes de bind()
	// salvo indicação contrária
	// ----------------------------------------------------------------

	/// Tamanho da fila de conexões pendentes passado a listen().
	/// Limita quantas conexões podem aguardar accept() simultaneamente.
	int backlog;

	/// Tamanho do buffer de recepção do socket em bytes (SO_RCVBUF).
	/// Configurado antes de bind(). Valor 0 usa o padrão do SO.
	int rcvbuf;

	/// Tamanho do buffer de envio do socket em bytes (SO_SNDBUF).
	/// Configurado antes de bind(). Valor 0 usa o padrão do SO.
	int sndbuf;

	/// Filtro de accept do FreeBSD (SO_ACCEPTFILTER), antes de listen().
	/// Equivalente ao deferred do Linux — só entrega a conexão
	/// ao servidor quando dados chegam. String vazia = desabilitado.
	std::string accept_filter;

	// ----------------------------------------------------------------
	// TCP Keepalive — aplicado após accept() void setsockopt
	// ----------------------------------------------------------------

	/// Modo de operação do TCP keepalive.
	/// OFF: desabilitado. ON: usa valores do kernel.
	/// CUSTOM: usa os valores explícitos abaixo.
	KeepAlive::type keepalive;

	/// Tempo de ociosidade antes de iniciar os probes (TCP_KEEPIDLE).
	/// Em segundos. Valor 0 indica uso do padrão do kernel.
	/// Só usado quando keepalive = CUSTOM.
	int keepalive_time;

	/// Intervalo entre probes consecutivos (TCP_KEEPINTVL).
	/// Em segundos. Valor 0 indica uso do padrão do kernel.
	/// Só usado quando keepalive = CUSTOM.
	int keepalive_intvl;

	/// Número de probes sem resposta antes de encerrar a conexão (TCP_KEEPCNT).
	/// Valor 0 indica uso do padrão do kernel.
	/// Só usado quando keepalive = CUSTOM.
	int keepalive_probes;

	ConfigServerListen()
		: address(), port(),
		is_unix(false), is_ipv6(false), is_ipv4(false),
		ssl(false), http2(false), default_server(false),
		reuseport(false), ipv6only(false), proxy_protocol(false),
		deferred(false), bind(false),
		backlog(511), rcvbuf(0), sndbuf(0), accept_filter(),
		keepalive(KeepAlive::OFF), keepalive_time(0),
		keepalive_intvl(0), keepalive_probes(0) {}
};

#endif
