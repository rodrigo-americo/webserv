/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerIterator.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 22:33:46 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 22:21:26 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ITERATOR_HPP
# define LEXER_ITERATOR_HPP

# include "Lexer.hpp"

/**
 * @brief Iterador para navegação sobre os tokens produzidos pelo lexer.
 *
 * Armazena uma cópia dos tokens gerados por um Lexer e fornece
 * operações de navegação semelhantes às de iteradores convencionais.
 */
class LexerIterator
{
	public:
	typedef Lexer::token	token; // @brief Alias para o tipo de token utilizado pelo lexer.
private:
	std::vector<token>	_tokens; // @brief Lista de tokens percorrida pelo iterador.
	size_t				_idx; // @brief Índice atual dentro da lista de tokens.
	std::string			_error; // @brief Erro gerado durante a análise léxica.
public:
	/**
	 * @brief Constrói um iterador a partir de um lexer.
	 *
	 * Copia os tokens e a mensagem de erro produzidos pelo lexer.
	 *
	 * @param lexer Lexer previamente executado.
	 */
	LexerIterator(const Lexer &lexer): _tokens(lexer.tokens()), _idx(0), _error(lexer.error()) {};
	// @brief Constrói um iterador vazio.
	LexerIterator(): _tokens(), _idx(0), _error() {};
	~LexerIterator() {};

	/**
	 * @brief Obtém a quantidade total de tokens.
	 *
	 * @return Número de tokens armazenados.
	 */
	size_t		size() const { return _tokens.size(); }

	bool		empty() const { return _tokens.size() == 0; }

	/**
	 * @brief Obtém a mensagem de erro do lexer.
	 *
	 * @return Texto do erro ou string vazia.
	 */
	std::string	error() { return _error; }

	// @brief Exibe todos os tokens no stdout.
	void	show() const
	{
		for (size_t i = 0; i < _tokens.size(); i++)
			std::cout << _tokens[i] << " ";
	}

	/**
	 * @brief Carrega os dados de um lexer.
	 *
	 * Copia os tokens e a mensagem de erro do lexer informado.
	 *
	 * @param lexer Lexer de origem.
	 * @return Referência para o iterador.
	 */
	LexerIterator	&operator=(const Lexer &lexer) { _tokens = lexer.tokens(); _error = lexer.error(); _idx = 0; return *this; }

	/**
	 * @brief Verifica se ainda existem tokens disponíveis.
	 *
	 * @return true enquanto o cursor estiver dentro do intervalo válido.
	 */
	operator bool() { return _idx < _tokens.size(); }

	/**
	 * @brief Acessa o token atual usando sintaxe de seta.
	 *
	 * Caso o cursor esteja após o último token, retorna
	 * o último token armazenado.
	 *
	 * @return Referência constante para o token atual.
	 */
	const token	*operator->() const
	{
		if (empty()) return NULL;
		if (_idx == size()) return &_tokens.back();
		return &_tokens[_idx];
	}

	/**
	 * @brief Obtém o token atual.
	 *
	 * Caso o cursor esteja após o último token, retorna
	 * o último token armazenado.
	 *
	 * @return Referência constante para o token atual.
	 */
	const token	&operator*() const
	{
		if (empty())
			return _tokens.back();

		if (_idx >= size())
			return _tokens.back();

		return _tokens[_idx];
	}

	/**
	 * @brief Avança para o próximo token.
	 *
	 * @return Referência para o iterador.
	 */
	LexerIterator	&operator++()
	{
		if (_idx < size())
			_idx++;
		return *this;
	}

	/**
	 * @brief Avança para o próximo token (pós-incremento).
	 *
	 * @return Cópia do estado anterior.
	 */
	LexerIterator	operator++(int)
	{
		LexerIterator	copy(*this);
		++(*this);
		return copy;
	}

	/**
	 * @brief Avança múltiplas posições.
	 *
	 * @param value Quantidade de posições.
	 * @return Referência para o iterador.
	 */
	LexerIterator	&operator+=(size_t value)
	{
		while (value > 0 && _idx < size())
		{
			++(*this);
			--value;
		}
		return *this;
	}

	/**
	 * @brief Retrocede para o token anterior.
	 *
	 * @return Referência para o iterador.
	 */
	LexerIterator	&operator--()
	{
		if (_idx > 0)
			_idx--;
		return *this;
	}

	/**
	 * @brief Retrocede para o token anterior (pós-decremento).
	 *
	 * @return Cópia do estado anterior.
	 */
	LexerIterator	operator--(int)
	{
		LexerIterator	copy(*this);
		--(*this);
		return copy;
	}

	/**
	 * @brief Retrocede múltiplas posições.
	 *
	 * @param value Quantidade de posições.
	 * @return Referência para o iterador.
	 */
	LexerIterator	&operator-=(size_t value)
	{
		while (value > 0 && _idx > 0)
		{
			--(*this);
			--value;
		}
		return *this;
	}
};

#endif
