/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerBuilder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 00:29:16 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:26:43 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_BUILDFER_HPP
# define LEXER_BUILDFER_HPP

# include <iostream>

# include "Lexer.hpp"
# include "LexerIterator.hpp"

/**
 * @brief Builder para configuração e criação de um lexer.
 *
 * Permite configurar comentários, delimitadores, aspas e arquivo
 * de entrada através de uma interface fluente (method chaining).
 * Ao final, gera um LexerIterator pronto para consumo.
 */
class LexerBuilder
{
private:
	Lexer	_lexer; // @brief Instância do lexer em construção.
public:
	//  @brief Constrói um builder vazio.
	LexerBuilder() {};
	~LexerBuilder() {};

	/**
	 * @brief Define o arquivo que será analisado.
	 *
	 * @param file_name Caminho do arquivo.
	 * @return Referência para o builder.
	 */
	LexerBuilder	&withFile(const std::string file_name) { _lexer.setFile(file_name); return *this; }

	/**
	 * @brief Adiciona a configuração padrão de tokens.
	 *
	 * Configura:
	 * - Comentários iniciados por '#'
	 * - Delimitadores '{', '}' e ';'
	 * - Aspas simples e duplas
	 *
	 * @return Referência para o builder.
	 */
	LexerBuilder	&withDefaultTokens() {
		_lexer.addComment("#");
		_lexer.addDelimitter("{", LBRACE);
		_lexer.addDelimitter("}", RBRACE);
		_lexer.addDelimitter(";", SEMICOLON);
		_lexer.addQuote("\"");
		_lexer.addQuote("'");
		return *this;
	}

	/**
	 * @brief Adiciona um prefixo de comentário.
	 *
	 * @param comment Prefixo do comentário.
	 * @return Referência para o builder.
	 */
	LexerBuilder	&withComment(const std::string &comment) { _lexer.addComment(comment); return *this; }

	/**
	 * @brief Adiciona um delimitador de string.
	 *
	 * @param quote Delimitador de string.
	 * @return Referência para o builder.
	 */
	LexerBuilder	&withQuote(const std::string &quote) { _lexer.addQuote(quote); return *this; }

	/**
	 * @brief Adiciona um delimitador/token personalizado.
	 *
	 * @param keyword Texto que identifica o token.
	 * @param type Tipo associado ao token.
	 * @return Referência para o builder.
	 */
	LexerBuilder	&withKeyword(const std::string &keyword, LexerTokenType type) { _lexer.addDelimitter(keyword, type); return *this; }

	/**
	 * @brief Finaliza a configuração e executa o lexer.
	 *
	 * Realiza a análise léxica do arquivo configurado e retorna
	 * um iterador para os tokens gerados.
	 *
	 * @return Iterador para percorrer os tokens produzidos.
	 */
	LexerIterator	build()
	{
		_lexer.run();
		return LexerIterator(_lexer);
	}
};

#endif
