/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerListenParser.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 10:58:09 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 00:32:11 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_LISTEN_PARSER_HPP
# define SERVER_LISTEN_PARSER_HPP

# include <vector>
# include <string>
# include <limits>

# include "utils.hpp"
# include "schema.hpp"

# include "ParserComposite.hpp"
# include "ConfigServerListen.hpp"
# include "ConfigServer.hpp"

class ServerListenParser
{
	private:
		Directive 					&_directive;
		std::vector<std::string>	_errors;

		void	_error(const ParserToken &token, const std::string &msg)
		{
			_errors.push_back(token.getLineAddress() + " " + msg);
		}

		template <typename T, typename Schema>
		void	_parseSchemedValue(const ParserToken &token, const std::string &value, Schema value_schema, T &config_item)
		{
			schema::detail::result<T>	res = value_schema.parse(value);
			if (!res)
			{
				for (size_t i = 0; i < res.errors.size(); i++)
					_error(token, res.errors[i].format());
				return;
			}
			config_item = res.value;
		}

		void	_parseFirstArg(const ParserToken &token, ConfigServerListen &config)
		{
			schema_size_t	port_schema = schema::sizet().between(1, 80000).name("listen port");
			utils::str arg = token.getContent();
			arg.trim();
			if (arg[0] == '[')
			{
				size_t	closing = arg.find(']');
				if (closing == std::string::npos)
					return _error(token, "ipv6 addres has no closing brackets ']'.");
				config.is_ipv6 = true;
				config.address = arg.substr(1, closing - 1).string();
				if (arg[closing + 1] != ':')
				{
					if (arg[closing + 1] != '\0')
						return _error(token, "ipv6 addres has no port separator ':'.");
					config.port = 80;
					return;
				}
				_parseSchemedValue(token, utils::str(&arg[closing + 2]).string(), port_schema, config.port);
				return;
			}
			if (arg.find("unix:") == 0)
			{
				config.is_unix = true;
				config.address = arg.substr(5).string();
				return;
			}
			size_t	port_sep = arg.find(':');
			if (port_sep != std::string::npos)
			{
				config.is_ipv4 = true;
				config.address = arg.substr(0, port_sep).string();
				_parseSchemedValue(token, utils::str(&arg[port_sep + 1]).string(), port_schema, config.port);
				return;
			}
			_parseSchemedValue(token, arg.string(), port_schema, config.port);
			config.is_ipv4 = true;
		}

		void	_parseKeepAlive(const ParserToken &token, const utils::str &value, ConfigServerListen &config)
		{
			if (value == "off")
			{
				config.keepalive = ConfigServerListenKeepAlive::OFF;
				return;
			}
			if (value == "on")
			{
				config.keepalive = ConfigServerListenKeepAlive::ON;
				return;
			}
			config.keepalive = ConfigServerListenKeepAlive::CUSTOM;
			size_t		start = value.find(':');
			size_t		end = value.find(':', start + 1);
			if (start == std::string::npos || end == std::string::npos)
			{
				_error(token, "invalid keepalive value!");
				return;
			}

			schema_int	time_schema = schema::integer().between(1, std::numeric_limits<int>::max())
				.sufix("ms", 1e-3).sufix("s", 1).sufix("m", 60).sufix("h", 3600)
				.sufix("d", 3600 * 24).sufix("w", 3600 * 24 * 7).sufixOptional().caseInsensitive();

			utils::str	temp = value.substr(0, start);
			time_schema.name("listen keepidle");
			if (!temp.trim().empty())
				_parseSchemedValue(token, temp.string(), time_schema, config.keepalive_time);

			time_schema.name("listen keepalive interval");
			temp = value.substr(start + 1, end - start);
			if (!temp.trim().empty())
				_parseSchemedValue(token, temp.string(), time_schema, config.keepalive_intvl);

			schema_int	probe_schema = schema::integer()
				.between(1, std::numeric_limits<int>::max()).name("listen keepalive probe");
			temp = value.substr(end + 1);
			if (!temp.trim().empty())
				_parseSchemedValue(token, temp.string(), probe_schema, config.keepalive_probes);
		}

		void	_parseKeyValueArgs(const ParserToken &token, size_t sep_idx, ConfigServerListen &config)
		{
			const std::string	key = token.getContent().substr(0, sep_idx);
			const std::string	value = token.getContent().substr(sep_idx + 1);
			schema_int	memory_size_schema = schema::integer().between(1, std::numeric_limits<int>::max())
				.sufix("k", 1024).sufix("m", 1024 * 1024).sufix("g", 1024 * 1024 * 1024).sufixOptional().caseInsensitive();

			memory_size_schema.name("listen rcvbuf");
			if (key == "rcvbuf")
				return _parseSchemedValue(token, value, memory_size_schema, config.rcvbuf);
			memory_size_schema.name("listen sndbuf");
			if (key == "sndbuf")
				return _parseSchemedValue(token, value, memory_size_schema, config.sndbuf);
			schema_int	backlog_schema = schema::integer()
				.between(1, std::numeric_limits<int>::max()).name("listen backlog");
			if (key == "backlog")
				return _parseSchemedValue(token, value, backlog_schema, config.backlog);
			schema_bool		ipv6only_schema = schema::boolean().add_falsy("off").add_truthy("on").name("listen ipv6only");
			if (key == "ipv6only")
				return _parseSchemedValue(token, value, ipv6only_schema, config.ipv6only);
			if (key == "so_keepalive")
				return _parseKeepAlive(token, value, config);
			_error(token, "key '" + key + "' is invalid.");
		}

		void	_parseArgs(const ParserToken &token, ConfigServerListen &config)
		{
			const std::string &arg = token.getContent();
			if (arg == "default_server") { config.default_server = true; return; }
			if (arg == "ssl") { config.ssl = true; return; }
			if (arg == "http2") { config.http2 = true; return; }
			if (arg == "proxy_protocol") { config.proxy_protocol = true; return; }
			if (arg == "reuseport") { config.reuseport = true; return; }
			if (arg == "bind") { config.bind = true; return; }
			if (arg == "deferred") { config.deferred = true; return; }
			_error(token, "arg '" + arg + "' is invalid.");
		}

	public:
		ServerListenParser(Directive &directive): _directive(directive) {}
		~ServerListenParser() {}

		void	toConfig(ConfigServer &config_server)
		{
			if (_directive.name != ParserTokenType::PT_LISTEN)
				return;
			ConfigServerListen	config;
			_parseFirstArg(_directive.values[0], config);
			for (size_t i = 1; i < _directive.values.size(); i++)
			{
				size_t	sep_idx = _directive.values[i].getContent().find("=");
				if (sep_idx != std::string::npos)
				{
					_parseKeyValueArgs(_directive.values[i], sep_idx, config);
					continue;
				}
				_parseArgs(_directive.values[i], config);
			}
			config_server.listeners.push_back(config);
		}

		bool	hasErrors() const { return !_errors.empty(); }
		const std::vector<std::string>	&errors() const { return _errors; }

};

#endif
