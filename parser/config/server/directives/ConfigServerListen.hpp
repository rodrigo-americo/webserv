/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServerListen.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:19:58 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 01:12:27 by bruno-valer      ###   ########.fr       */
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


class ConfigServerListen
{
	public:
		typedef ConfigServerListenKeepAlive	KeepAlive;
	protected:
		/// Endereço IP onde o socket vai fazer bind.
		/// Exemplos: "0.0.0.0" (todos IPv4), "::" (todos IPv6),
		/// "127.0.0.1" (loopback), "/var/run/nginx.sock" (unix socket).
		std::string _address;

		/// Porta onde o socket vai escutar.
		/// Valor 0 indica que é um unix socket (is_unix = true).
		size_t _port;

		/// Indica que é um unix domain socket (AF_UNIX).
		/// Quando true, address contém o caminho do socket
		/// e port é ignorado. Afeta a chamada socket().
		bool _is_unix = false;

		// ----------------------------------------------------------------
		// Flags — configuradas antes de bind() salvo indicação contrária
		// ----------------------------------------------------------------

		/// Ativa TLS na conexão após accept().
		/// Não afeta o socket em si — o wrap TLS ocorre
		/// na conexão aceita, antes de passar ao handler HTTP.
		bool _ssl = false;

		/// Ativa HTTP/2 via negociação ALPN durante o handshake TLS.
		/// Só tem efeito se ssl = true. Aplicado após accept().
		bool _http2 = false;

		/// Marca este listen como virtual host padrão.
		/// Usado pelo dispatcher quando nenhum server_name
		/// casa com o header Host da requisição. Não gera syscall.
		bool _default_server = false;

		/// Ativa SO_REUSEPORT antes de bind().
		/// Permite que cada worker tenha seu próprio socket
		/// na mesma porta, evitando contenção no accept().
		bool _reuseport = false;

		/// Ativa IPV6_V6ONLY antes de bind().
		/// Impede que um socket IPv6 aceite conexões IPv4.
		/// Só relevante quando address é "::".
		bool _ipv6only = false;

		/// Ativa leitura do header PROXY Protocol após accept().
		/// O header é lido antes de passar a conexão ao handler HTTP,
		/// permitindo obter o IP real do cliente via proxy/load balancer.
		bool _proxy_protocol = false;

		/// Ativa TCP_DEFER_ACCEPT antes de listen() (Linux only).
		/// O worker só é acordado quando dados chegam na conexão,
		/// evitando wakeups desnecessários em conexões vazias.
		bool _deferred = false;

		/// Força bind explícito neste endereço antes de bind().
		/// Necessário quando o mesmo endereço/porta é compartilhado
		/// entre vhosts e um deles precisa de opções de socket distintas.
		bool _bind = false;

		// ----------------------------------------------------------------
		// Pares chave=valor — configurados void setsockopt antes de bind()
		// salvo indicação contrária
		// ----------------------------------------------------------------

		/// Tamanho da fila de conexões pendentes passado a listen().
		/// Limita quantas conexões podem aguardar accept() simultaneamente.
		int _backlog = 511;

		/// Tamanho do buffer de recepção do socket em bytes (SO_RCVBUF).
		/// Configurado antes de bind(). Valor 0 usa o padrão do SO.
		int _rcvbuf = 0;

		/// Tamanho do buffer de envio do socket em bytes (SO_SNDBUF).
		/// Configurado antes de bind(). Valor 0 usa o padrão do SO.
		int _sndbuf = 0;

		/// Filtro de accept do FreeBSD (SO_ACCEPTFILTER), antes de listen().
		/// Equivalente ao deferred do Linux — só entrega a conexão
		/// ao servidor quando dados chegam. String vazia = desabilitado.
		std::string _accept_filter;

		// ----------------------------------------------------------------
		// TCP Keepalive — aplicado após accept() void setsockopt
		// ----------------------------------------------------------------

		/// Modo de operação do TCP keepalive.
		/// OFF: desabilitado. ON: usa valores do kernel.
		/// CUSTOM: usa os valores explícitos abaixo.
		ConfigServerListenKeepAlive::type _keepalive = ConfigServerListenKeepAlive::OFF;

		/// Tempo de ociosidade antes de iniciar os probes (TCP_KEEPIDLE).
		/// Em segundos. Valor 0 indica uso do padrão do kernel.
		/// Só usado quando keepalive = CUSTOM.
		int _keepalive_time = 0;

		/// Intervalo entre probes consecutivos (TCP_KEEPINTVL).
		/// Em segundos. Valor 0 indica uso do padrão do kernel.
		/// Só usado quando keepalive = CUSTOM.
		int _keepalive_intvl = 0;

		/// Número de probes sem resposta antes de encerrar a conexão (TCP_KEEPCNT).
		/// Valor 0 indica uso do padrão do kernel.
		/// Só usado quando keepalive = CUSTOM.
		int _keepalive_probes = 0;

	public:
		/// Retorna o endereço IP ou caminho unix onde o socket fará bind.
		const std::string& address() const { return _address; }

		void setAddress(const std::string &value) { _address = value; }

		/// Retorna a porta onde o socket vai escutar.
		/// Retorna 0 se for unix socket.
		size_t port() const { return _port; }

		void setPort(size_t value) { _port = value; }

		/// Retorna true se for um unix domain socket (AF_UNIX).
		bool is_unix() const { return _is_unix; }

		void setIs_unix(bool value) { _is_unix = value; }

		/// Retorna true se TLS deve ser aplicado após accept().
		bool is_ssl() const { return _ssl; }

		void setIs_ssl(bool value) { _ssl = value; }

		/// Retorna true se HTTP/2 deve ser negociado via ALPN.
		bool is_http2() const { return _http2; }

		void setIs_http2(bool value) { _http2 = value; }

		/// Retorna true se este listen é o virtual host padrão.
		bool is_default_server() const { return _default_server; }

		void setIs_default_server(bool value) { _default_server = value; }

		/// Retorna true se SO_REUSEPORT deve ser ativado antes de bind().
		bool is_reuseport() const { return _reuseport; }

		void setIs_reuseport(bool value) { _reuseport = value; }

		/// Retorna true se IPV6_V6ONLY deve ser ativado antes de bind().
		bool is_ipv6only() const { return _ipv6only; }

		void setIs_ipv6only(bool value) { _ipv6only = value; }

		/// Retorna true se PROXY Protocol deve ser lido após accept().
		bool is_proxy_protocol() const { return _proxy_protocol; }

		void setIs_proxy_protocol(bool value) { _proxy_protocol = value; }

		/// Retorna true se TCP_DEFER_ACCEPT deve ser ativado antes de listen().
		bool is_deferred() const { return _deferred; }

		void setIs_deferred(bool value) { _deferred = value; }

		/// Retorna true se bind explícito deve ser forçado neste endereço.
		bool is_bind() const { return _bind; }

		void setIs_bind(bool value) { _bind = value; }

		/// Retorna o tamanho da fila de conexões pendentes para listen().
		int backlog() const { return _backlog; }

		void setBacklog(int value) { _backlog = value; }

		/// Retorna o tamanho do buffer de recepção em bytes (SO_RCVBUF).
		/// Valor 0 indica uso do padrão do SO.
		int rcvbuf() const { return _rcvbuf; }

		void setRcvbuf(int value) { _rcvbuf = value; }

		/// Retorna o tamanho do buffer de envio em bytes (SO_SNDBUF).
		/// Valor 0 indica uso do padrão do SO.
		int sndbuf() const { return _sndbuf; }

		void setSndbuf(int value) { _sndbuf = value; }

		/// Retorna o nome do accept filter (FreeBSD). String vazia = desabilitado.
		const std::string& accept_filter() const { return _accept_filter; }

		void setAccept_filter(const std::string &value) { _accept_filter = value; }

		/// Retorna o modo de operação do TCP keepalive.
		ConfigServerListenKeepAlive::type keepalive() const { return _keepalive; }

		void setKeepalive(ConfigServerListenKeepAlive::type type) { _keepalive = type; }

		/// Retorna o tempo de ociosidade antes dos probes (TCP_KEEPIDLE).
		/// Em segundos. Valor 0 = padrão do kernel. Só válido se keepalive = CUSTOM.
		int keepalive_time() const { return _keepalive_time; }

		void keepalive_time(int value) { _keepalive_time = value; }

		/// Retorna o intervalo entre probes (TCP_KEEPINTVL).
		/// Em segundos. Valor 0 = padrão do kernel. Só válido se keepalive = CUSTOM.
		int keepalive_intvl() const { return _keepalive_intvl; }

		void keepalive_intvl(int value) { _keepalive_intvl = value; }

		/// Retorna o número de probes antes de encerrar a conexão (TCP_KEEPCNT).
		/// Valor 0 = padrão do kernel. Só válido se keepalive = CUSTOM.
		int keepalive_probes() const { return _keepalive_probes; }

		void keepalive_probes(int value) { _keepalive_probes = value; }
};

#endif
