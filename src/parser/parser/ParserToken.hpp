/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserToken.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 11:02:08 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/06 14:05:32 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_TOKEN_HPP
# define PARSER_TOKEN_HPP

# include <string>

# include "has_type.hpp"
# include "LexerToken.hpp"
# include "ParserTokenType.hpp"

/**
 * @brief Token utilizado durante a análise sintática.
 *
 * Representa uma unidade reconhecida pelo parser após a conversão
 * dos tokens produzidos pelo lexer. Além das informações herdadas
 * de BaseToken, permite alterar o tipo do token durante o processo
 * de análise.
 */
class ParserToken: public BaseToken<ParserTokenType::type>
{
	typedef BaseToken<ParserTokenType::type>	base; // @brief Alias para a classe base.
public:
	/**
	 * @brief Constrói um token sintático completo.
	 *
	 * @param file_name Nome do arquivo de origem.
	 * @param line Linha onde o token foi encontrado.
	 * @param line_col Coluna onde o token foi encontrado.
	 * @param content Conteúdo textual do token.
	 * @param type Tipo sintático do token.
	 */
	ParserToken(const std::string &file_name, size_t line, size_t line_col, const std::string &content, ParserTokenType::type type)
		: base(file_name, line, line_col, content, type) {};
	/**
	 * @brief Constrói um token sem tipo definido.
	 *
	 * O tipo poderá ser definido posteriormente.
	 *
	 * @param file_name Nome do arquivo de origem.
	 * @param line Linha onde o token foi encontrado.
	 * @param line_col Coluna onde o token foi encontrado.
	 * @param content Conteúdo textual do token.
	 */
	ParserToken(const std::string &file_name, size_t line, size_t line_col, const std::string &content)
		: base(file_name, line, line_col, content) {};
	~ParserToken() {};

	/**
	 * @brief Converte um token léxico em um token sintático.
	 *
	 * Copia todas as informações de localização e conteúdo do
	 * LexerToken, substituindo apenas seu tipo.
	 *
	 * @param token Token proveniente do lexer.
	 * @param type Tipo sintático desejado.
	 *
	 * @return Novo ParserToken.
	 */
	static ParserToken	fromLexerToken(const LexerToken &token, ParserTokenType::type type)
	{
		return ParserToken(token.getFileName(), token.getLine(), token.getLineColumn(), token.getContent(), type);
	}

	/**
	 * @brief Atualiza o tipo do token.
	 *
	 * @param type Novo tipo sintático.
	 */
	void				setType(ParserTokenType::type type) { _type = type; }

	/**
	 * @brief Atribui diretamente um tipo ao token.
	 *
	 * Permite alterar o tipo através do operador de atribuição.
	 *
	 * @param type Novo tipo sintático.
	 *
	 * @return Referência para o token.
	 */
	ParserToken	&operator=(ParserTokenType::type type) { _type = type; return *this; }
};

#endif
