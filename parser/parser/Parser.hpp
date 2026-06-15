/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 14:09:14 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/06 14:11:18 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <map>
# include <string>

# include "ParserToken.hpp"
# include "LexerBuilder.hpp"
# include "LexerIterator.hpp"
# include "ParserComposite.hpp"
# include "ParserAst.hpp"


/**
 * @brief Parser responsável por converter tokens em uma AST.
 *
 * Consome os tokens produzidos pelo lexer e constrói uma árvore
 * sintática baseada nos nós definidos em ParserComposite.
 *
 * O parser utiliza palavras-chave configuráveis para identificar
 * diretivas, blocos e modificadores especiais.
 */
class Parser
{
private:
	LexerBuilder									_lexer_builder; // @brief Builder utilizado para criar o lexer.
	LexerIterator									_it; // @brief Iterador utilizado para percorrer os tokens.
	std::map<std::string, ParserTokenType::type>	_block_keywords; // @brief Palavras-chave que representam blocos.
	std::map<std::string, ParserTokenType::type>	_directive_keywords; // @brief Palavras-chave que representam diretivas.
	std::map<std::string, ParserTokenType::type>	_modifiers; // @brief Modificadores especiais reconhecidos pelo parser.
	std::vector<std::string>						_errors; // @brief Lista de erros encontrados durante a análise.

	typedef	std::map<std::string, ParserTokenType::type>::iterator opts_iterator; // @brief Alias para iteradores dos mapas de palavras-chave.

	// @brief Ignora tokens de comentário consecutivos.
	void	_skipComments()
	{
		while (_it && *_it == LexerTokenType::COMMENT)
			++_it;
	}

	/**
	 * @brief Converte um token léxico em um token sintático.
	 *
	 * Realiza o mapeamento de palavras-chave registradas para os
	 * respectivos tipos de ParserToken.
	 *
	 * @param token Token proveniente do lexer.
	 * @param after_location Indica se o token aparece após uma diretiva
	 * de location, permitindo interpretar modificadores.
	 *
	 * @return Token convertido para o domínio do parser.
	 */
	ParserToken	_convertToken(const LexerToken &token, bool after_location = false)
	{
		if (token == LexerTokenType::STRING_SINGLE) return ParserToken::fromLexerToken(token, ParserTokenType::PT_STRING_SINGLE);
		if (token == LexerTokenType::STRING_DOUBLE) return ParserToken::fromLexerToken(token, ParserTokenType::PT_STRING_DOUBLE);

		if (after_location)
		{
			opts_iterator	modif_it = _modifiers.find(token.getContent());
			if (modif_it != _modifiers.end())
				return ParserToken::fromLexerToken(token, modif_it->second);
		}

		opts_iterator	direct_it = _directive_keywords.find(token.getContent());
		if (direct_it != _directive_keywords.end())
			return ParserToken::fromLexerToken(token, direct_it->second);

		opts_iterator	block_it = _block_keywords.find(token.getContent());
		if (block_it != _block_keywords.end())
			return ParserToken::fromLexerToken(token, block_it->second);

		if (token == LexerTokenType::END) return ParserToken::fromLexerToken(token, ParserTokenType::PT_END);

		return ParserToken::fromLexerToken(token, ParserTokenType::PT_WORD);
	}

	/**
	 * @brief Analisa uma diretiva ou bloco.
	 *
	 * Consome os tokens necessários para construir um nó da AST.
	 * Caso encontre um bloco, realiza a análise recursiva dos
	 * seus filhos.
	 *
	 * @return Ponteiro para o nó criado.
	 */
	ParserNode	*parseStatement()
	{
		_skipComments();

		ParserToken	name = _convertToken(*_it);
		if (name == ParserTokenType::PT_WORD)
			_errors.push_back((*_it).getLineAddress() + " Invalid node name!");
		++_it;

		bool	is_after_location = name == ParserTokenType::PT_LOCATION;
		std::vector<ParserToken>	values;
		while (_it
			&& *_it != LexerTokenType::COMMENT
			&& *_it != LexerTokenType::LBRACE
			&& *_it != LexerTokenType::SEMICOLON)
		{
			if (*_it == LexerTokenType::RBRACE)
				_errors.push_back((*_it).getLineAddress() + " Closing an unopened scope!");
			values.push_back(_convertToken(*_it, is_after_location));
			is_after_location = false;
			++_it;
		}
		if (*_it == LexerTokenType::LBRACE)
		{
			LexerIterator::token	lbrace = *_it;
			++_it; // consome { LBRACE
			Block	*block = new Block(name, values);
			while (_it && *_it != LexerTokenType::RBRACE)
			{
				_skipComments();
				if (!_it || *_it == LexerTokenType::LBRACE)
					break;
				block->children.push_back(parseStatement());
			}
			if (*_it != LexerTokenType::RBRACE)
				_errors.push_back(lbrace.getLineAddress() + " Unclosed scope");
			++_it; // consome } RBRACE
			return block;
		}
		++_it; // consome ; SEMICOLON
		if (name == ParserTokenType::PT_SERVER)
			name.setType(ParserTokenType::PT_SERVER_DIRECTIVE);
		Directive	*directive = new Directive(name, values);
		return directive;
	}

public:
	/**
	 * @brief Constrói um parser utilizando um lexer configurado.
	 *
	 * @param lexer_builder Builder responsável pela criação do lexer.
	 */
	Parser(LexerBuilder &lexer_builder): _lexer_builder(lexer_builder), _it(), _block_keywords(), _directive_keywords(), _modifiers() {};
	// @brief Constrói um parser vazio.
	Parser(): _lexer_builder(), _it(), _block_keywords(), _directive_keywords(), _modifiers() {};
	~Parser() {};

	/**
	 * @brief Registra uma palavra-chave de bloco.
	 *
	 * Exemplo: "http", "server", "location".
	 *
	 * @param content Texto da palavra-chave.
	 * @param type Tipo sintático associado.
	 */
	void	addBlockKeyword(const std::string &content, ParserTokenType::type type) { _block_keywords[content] = type; }

	/**
	 * @brief Registra uma palavra-chave de diretiva.
	 *
	 * Exemplo: "listen", "root", "index".
	 *
	 * @param content Texto da palavra-chave.
	 * @param type Tipo sintático associado.
	 */
	void	addDirectiveKeyword(const std::string &content, ParserTokenType::type type) { _directive_keywords[content] = type; }

	/**
	 * @brief Registra um modificador especial.
	 *
	 * Exemplo: modificadores utilizados por diretivas específicas.
	 *
	 * @param content Texto do modificador.
	 * @param type Tipo sintático associado.
	 */
	void	addModifier(const std::string &content, ParserTokenType::type type) { _modifiers[content] = type; }

	/**
	 * @brief Define o lexer utilizado pelo parser.
	 *
	 * @param lexer_builder Builder configurado.
	 */
	void	setLexer(LexerBuilder &lexer_builder) { _lexer_builder = lexer_builder; }

	/**
	 * @brief Executa a análise sintática.
	 *
	 * Constrói a AST completa a partir dos tokens produzidos
	 * pelo lexer e registra os erros encontrados.
	 *
	 * @return AST gerada pelo parser.
	 */
	ParserAst	parse()
	{
		_it = _lexer_builder.build();
		ParserAst	ast;
		if (!_it.error().empty())
			ast.addError(_it.error());
		Block	*root = new Block(ParserToken("root", 0, 0, "root", ParserTokenType::PT_MAIN));
		while (_it)
		{
			_skipComments();
			if (!_it)
				break;
			root->children.push_back(parseStatement());
		}
		for (size_t i = 0; i < _errors.size(); i++)
			ast.addError(_errors[i]);
		ast.setRoot(root);
		return ast;
	}
};

#endif
