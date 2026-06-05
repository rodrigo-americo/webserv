/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 09:50:53 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:23:27 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include <string>
# include <vector>
# include <set>
# include <iostream>
# include <sstream>

# include "has_type.hpp"
# include "File.hpp"
# include "LexerToken.hpp"


/**
 * @brief Analisador léxico configurável.
 *
 * Converte o conteúdo de um arquivo em uma sequência de tokens.
 * Suporta delimitadores, comentários, strings delimitadas por aspas
 * e palavras comuns.
 */
class Lexer
{
	public:
		typedef LexerTokenType					token_type; // @brief Alias para o tipo do token.
		typedef LexerToken						token; // @brief Alias para a classe de token.
	private:
	typedef	std::pair<std::string, token_type>	delimitter; // @brief Associação entre um delimitador textual e seu tipo de token.
	std::vector<delimitter>		_delimitters; // @brief Lista de delimitadores reconhecidos.
	std::vector<std::string>	_comments; // @brief Prefixos de comentários suportados.
	std::vector<std::string>	_quotes; // @brief Delimitadores de strings suportados.
	File						_file; // @brief Arquivo sendo analisado.
	std::string					_error; // @brief Mensagem de erro gerada durante a análise.
	std::vector<token>			_tokens; // @brief Tokens produzidos pela análise léxica.

	delimitter						_temp_delimitter; // @brief Delimitador temporariamente identificado.
	std::string					_temp_quote; // @brief Delimitador de string temporariamente identificado.

	public:
		/**
		 * @brief Constrói um lexer para um arquivo específico.
		 *
		 * @param file_name Caminho do arquivo.
		 */
		Lexer(const std::string &file_name)
			: _delimitters(), _comments(), _quotes(), _file(file_name), _error(), _temp_delimitter(), _temp_quote() {};
		// @brief Constrói um lexer vazio.
		Lexer()
			: _delimitters(), _comments(), _quotes(), _file(), _error(), _temp_delimitter(), _temp_quote() {};
		~Lexer() {};

	private:
		// @brief Avança o cursor ignorando espaços em branco.
		void	_skipWhiteSpaces()
		{
			while (_file && std::isspace(*_file))
				++_file;
		}

		/**
		 * @brief Verifica se o cursor está sobre um delimitador.
		 *
		 * Em caso positivo, armazena o delimitador encontrado
		 * em _temp_delimitter.
		 *
		 * @return true se encontrou um delimitador.
		 */
		bool	_isDelimitter()
		{
			for (size_t kw = 0; kw < _delimitters.size(); kw++)
			{
				if (_file.nextIs(_delimitters[kw].first))
				{
					_temp_delimitter = _delimitters[kw];
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Lê o delimitador previamente identificado.
		 *
		 * @return Token correspondente ao delimitador.
		 */
		token	_readKeyword()
		{
			_file += _temp_delimitter.first.size();
			return token(_file.name(), _file.line(), _file.lineColumn(), _temp_delimitter.first, _temp_delimitter.second);
		}

		/**
		 * @brief Verifica se o cursor está sobre uma string delimitada.
		 *
		 * Em caso positivo, armazena o delimitador encontrado
		 * em _temp_quote.
		 *
		 * @return true se encontrou uma abertura de string.
		 */
		bool	_isQuote()
		{
			for (size_t qt = 0; qt < _quotes.size(); qt++)
			{
				if (_file.nextIs(_quotes[qt]))
				{
					_temp_quote = _quotes[qt];
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Lê uma string delimitada por aspas.
		 *
		 * Trata caracteres escapados e valida o fechamento
		 * correto da string.
		 *
		 * @return Token de string ou token de erro.
		 */
		token	_readQuote()
		{
			_file += _temp_quote.size();
			size_t	quote_start_cursor		= _file.cursor();
			size_t	quote_start_line		= _file.line();
			size_t	quote_start_line_col	= _file.lineColumn();
			while (_file)
			{
				if (*_file == _temp_quote[0])
				{
					if (_file.nextIs(_temp_quote))
					{
						token	_token = token::fromQuote(_file.name(), _file.line(), _file.lineColumn(), _temp_quote, _file.substr(quote_start_cursor, _file.cursor() - quote_start_cursor));
						_file += _temp_quote.size();
						return _token;
					}
				}
				if (*_file == '\\')
					++_file;
				++_file;
			}
			std::string e_header = _file.name()+":"+std::to_string(quote_start_line)+":"+std::to_string(quote_start_line_col)+" -> ";
			_error = e_header + "quote open '"+_temp_quote+"'.";
			return token(_file.name(), _file.line(), _file.lineColumn(), "", token_type::ERROR);
		}

		/**
		 * @brief Verifica se o cursor está sobre um comentário.
		 *
		 * @return true se encontrou um comentário.
		 */
		bool	_isComment()
		{
			for (size_t qt = 0; qt < _comments.size(); qt++)
			{
				if (_file.nextIs(_comments[qt]))
					return true;
			}
			return false;
		}

		/**
		 * @brief Lê um comentário até o fim da linha.
		 *
		 * @return Token de comentário.
		 */
		token	_readComment()
		{
			size_t	comment_start_cursor		= _file.cursor();
			while (*_file && *_file != '\n')
				++_file;
			return	token(_file.name(), _file.line(), _file.lineColumn(), _file.substr_back(comment_start_cursor), token_type::COMMENT);
		}

		/**
		 * @brief Lê uma palavra comum.
		 *
		 * A leitura termina ao encontrar espaço em branco,
		 * delimitador ou abertura de string.
		 *
		 * @return Token de palavra.
		 */
		token	_readWord()
		{
			if (!_file)
				return token(_file.name(), _file.line(), _file.lineColumn(), "", token_type::END);
			size_t	word_start_cursor		= _file.cursor();
			while (*_file && !std::isspace(*_file) && !_isDelimitter() && !_isQuote())
				++_file;
			return token(_file.name(), _file.line(), _file.lineColumn(), _file.substr(word_start_cursor, _file.cursor() - word_start_cursor), token_type::WORD);
		}

	public:
		/**
		 * @brief Define o arquivo a ser analisado.
		 *
		 * @param file_name Caminho do arquivo.
		 */
		void	setFile(const std::string &file_name) { _file.setName(file_name); }
		/**
		 * @brief Adiciona um delimitador reconhecido pelo lexer.
		 *
		 * @param _keyword Texto do delimitador.
		 * @param _type Tipo de token associado.
		 */
		void	addDelimitter(const std::string &_keyword, token_type _type) { _delimitters.push_back(std::make_pair(_keyword, _type)); }
		/**
		 * @brief Adiciona um prefixo de comentário.
		 *
		 * Exemplo: "#" ou "//".
		 *
		 * @param comment Prefixo do comentário.
		 */
		void	addComment(const std::string &comment) { _comments.push_back(comment); }
		/**
		 * @brief Adiciona um delimitador de string.
		 *
		 * Exemplo: "\"" ou "'".
		 *
		 * @param quote Delimitador de string.
		 */
		void	addQuote(const std::string &quote) { _quotes.push_back(quote); }

		/**
		 * @brief Executa a análise léxica do arquivo.
		 *
		 * Os tokens produzidos ficam disponíveis através de
		 * tokens().
		 */
		void	run()
		{
			_file.run();
			if (!_file.error().empty())
			{
				_error = _file.error();
				return;
			}
			while (_file)
			{
				_skipWhiteSpaces();
				if (_isComment())
				{
					_tokens.push_back(_readComment());
					continue;
				}
				if (_isDelimitter())
				{
					_tokens.push_back(_readKeyword());
					continue;
				}
				if (_isQuote())
				{
					_tokens.push_back(_readQuote());
					continue;
				}
				_tokens.push_back(_readWord());
			}
		}

		/**
		 * @brief Obtém a mensagem de erro da análise.
		 *
		 * @return Referência para a mensagem de erro.
		 */
		const std::string			&error() const { return _error; }

		/**
		 * @brief Obtém todos os tokens gerados.
		 *
		 * @return Vetor de tokens.
		 */
		const std::vector<token>	&tokens() const { return _tokens; }

		// @brief Exibe todos os tokens no stdout.
		void	show() const
		{
			for (size_t i = 0; i < _tokens.size(); i++)
				std::cout << _tokens[i] << " ";
		}
};

#endif
