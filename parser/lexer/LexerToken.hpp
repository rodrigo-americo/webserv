/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerToken.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 11:44:53 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:59:21 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_TOKEN_HPP
# define LEXER_TOKEN_HPP

# include <iostream>
# include "utils.hpp"

# include "has_type.hpp"
# include "LexerTokenType.hpp"

/**
 * @brief Classe base para representação de tokens.
 *
 * Armazena informações comuns utilizadas por diferentes tipos de token,
 * como localização no arquivo, conteúdo textual e tipo associado.
 *
 * @tparam Type Enumeração utilizada para identificar o tipo do token.
 */
template <typename Type>
class BaseToken: public segregation::has_type<Type>
{
	typedef segregation::has_type<Type>	base; // @brief Alias para a classe base que fornece suporte ao tipo.
	public:
		typedef Type		type; // @brief Alias para o tipo da enumeração utilizada.
	protected:
		std::string	_file_name; // @brief Nome do arquivo de origem.
		size_t		_line; // @brief Linha onde o token foi encontrado.
		size_t		_line_col; // @brief Coluna onde o token foi encontrado.
		std::string	_content; // @brief Conteúdo textual do token.
public:
	/**
	 * @brief Constrói um token com tipo definido.
	 *
	 * @param file_name Nome do arquivo de origem.
	 * @param line Linha do token.
	 * @param line_col Coluna do token.
	 * @param content Conteúdo textual.
	 * @param _type Tipo do token.
	 */
	BaseToken(const std::string &file_name, size_t line, size_t line_col, const std::string &content, type _type)
		: base(_type), _file_name(file_name), _line(line), _line_col(line_col), _content(content)  {};

	/**
	 * @brief Constrói um token sem tipo definido.
	 *
	 * @param file_name Nome do arquivo de origem.
	 * @param line Linha do token.
	 * @param line_col Coluna do token.
	 * @param content Conteúdo textual.
	 */
	BaseToken(const std::string &file_name, size_t line, size_t line_col, const std::string &content)
		: base(), _file_name(file_name), _line(line), _line_col(line_col), _content(content)  {};
	~BaseToken() {};

	/**
	 * @brief Obtém o nome do arquivo de origem.
	 *
	 * @return Referência para o nome do arquivo.
	 */
	const std::string	&getFileName() const { return _file_name; }

	/**
	 * @brief Obtém a linha do token.
	 *
	 * @return Número da linha.
	 */
	size_t				getLine() const { return _line; }

	/**
	 * @brief Obtém a coluna do token.
	 *
	 * @return Número da coluna.
	 */
	size_t				getLineColumn() const { return _line_col; }

	/**
	 * @brief Obtém uma representação textual da localização.
	 *
	 * Formato:
	 *
	 * `arquivo`:`linha`:`coluna`
	 *
	 * @return Endereço textual do token.
	 */
	std::string			getLineAddress() const
	{
		return _file_name + ":" + utils::to_string(_line) + ":" + utils::to_string(_line_col);
	}

	/**
	 * @brief Obtém o conteúdo textual do token.
	 *
	 * @return Referência para o conteúdo.
	 */
	const std::string	&getContent() const { return _content; }

	/**
	 * @brief Compara o tipo do token com um valor específico.
	 *
	 * @param type Tipo a ser comparado.
	 * @return true se os tipos forem iguais.
	 */
	bool	operator==(Type type) const { return this->_type == type; }

	/**
	 * @brief Compara o tipo do token com um valor específico.
	 *
	 * @param type Tipo a ser comparado.
	 * @return true se os tipos forem diferentes.
	 */
	bool	operator!=(Type type) const { return !(*this == type); }
};


/**
 * @brief Token produzido durante a análise léxica.
 *
 * Representa uma unidade identificada pelo lexer antes da
 * interpretação sintática realizada pelo parser.
 */
class LexerToken: public BaseToken<LexerTokenType>
{
	typedef BaseToken<LexerTokenType>	base; // @brief Alias para a classe base.

	public:
		/**
		 * @brief Constrói um token léxico.
		 *
		 * @param file_name Nome do arquivo de origem.
		 * @param line Linha do token.
		 * @param line_col Coluna do token.
		 * @param content Conteúdo textual.
		 * @param _type Tipo do token.
		 */
		LexerToken(const std::string &file_name, size_t line, size_t line_col, const std::string &content, type _type)
			: base(file_name, line, line_col, content, _type) {};
		~LexerToken() {};

		/**
		 * @brief Cria um token de string delimitada por aspas.
		 *
		 * O tipo é gerado dinamicamente a partir do delimitador
		 * utilizado, permitindo distinguir diferentes tipos de aspas.
		 *
		 * Exemplos:
		 * - "
		 * - '
		 * - delimitadores personalizados
		 *
		 * @param file_name Nome do arquivo de origem.
		 * @param line Linha do token.
		 * @param line_col Coluna do token.
		 * @param quote Delimitador utilizado.
		 * @param content Conteúdo interno da string.
		 *
		 * @return Novo token representando a string.
		 */
		static LexerToken	fromQuote(const std::string &file_name, size_t line, size_t line_col, const std::string &quote, const std::string &content)
		{
			size_t	_type = 0;
			size_t	size = quote.size() - 1;
			for (size_t i = 0; i < quote.size(); i++)
			{
				_type |= quote[i] << (size * 8);
				size--;
			}
			return LexerToken(file_name, line, line_col, content, static_cast<LexerTokenType>(_type));
		}
};

/**
 * @brief Imprime um token em formato legível.
 *
 * Exibe o tipo e o conteúdo do token no formato:
 * <TIPO>[conteúdo]
 *
 * @param os Stream de saída.
 * @param token Token a ser exibido.
 * @return Referência para a stream.
 */
inline std::ostream	&operator<<(std::ostream &os, LexerToken token)
{
	size_t		size = token.getContent().size();
	bool		has_endl = token.getContent()[size - 1] == '\n';
	std::string	content = has_endl ? token.getContent().substr(0, size - 1) : token.getContent();
	os << "<" << token.getType() << ">[" << content << "]" << (has_endl ? "\n" : "");
	return os;
}

#endif
