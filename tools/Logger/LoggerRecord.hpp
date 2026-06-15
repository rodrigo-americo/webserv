/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LoggerRecord.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:48:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 21:53:54 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_RECORD_HPP
# define LOGGER_RECORD_HPP

# include "utils.hpp"

struct LoggerLevel
{
	enum type
	{
		TRACE,	// Fluxo interno do código — entrada/saída de funções, iterações de loop. Muito verboso, só útil para rastrear um bug muito específico
		DEBUG,	// Informação de diagnóstico — valores de variáveis, estados internos. Útil durante desenvolvimento, desligado em produção
		INFO,	// Eventos normais e esperados do sistema — servidor iniciou, usuário logou, job concluído
		WARN,	// Algo inesperado mas recuperável — arquivo de config não encontrado (usou o padrão), retry de conexão. O sistema continua funcionando
		ERROR,	// Falha em uma operação específica — não conseguiu salvar arquivo, query falhou. O sistema continua, mas algo deu errado
		FATAL	// Falha irrecuperável — o programa vai encerrar na sequência. Corrupção de memória, porta em uso, dependência crítica ausente
	};
};

struct LoggerRecord
{
	LoggerLevel::type	level;
	utils::str			msg;
	utils::str			file;
	size_t				line;
	utils::time			time;

	LoggerRecord(): level(), msg(), file(), line(), time() {}
	LoggerRecord(
			LoggerLevel::type lvl,
			const utils::str &_msg,
			const utils::str &_file,
			size_t _line,
			const utils::time &_time)
		: level(lvl), msg(_msg), file(_file), line(_line), time(_time) {}
};

#endif
