/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 19:50:46 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:14:21 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include <string>
# include <fstream>
# include <limits>
# include <vector>
# include <iostream>

/**
 * @brief Classe utilitária para leitura e navegação de arquivos de texto utilizada pelo lexer/parser.
 *
 * ---
 *
 * Representa um arquivo carregado em memória com suporte a navegação
 * caractere por caractere.
 *
 * ---
 *
 * A classe lê o conteúdo de um arquivo para uma string interna e mantém
 * informações de posição, linha e coluna, permitindo que parsers percorram
 * o conteúdo de forma simples.
 */
class File
{
private:
	std::string					_name; // @brief Nome do arquivo.
	std::string					_error; // @brief Mensagem de erro gerada durante a leitura.
	std::string					_src; // @brief Conteúdo completo do arquivo.
	std::vector<std::string>	_lines; // @brief Conteúdo separado por linhas.
	size_t						_idx; // @brief Índice atual dentro de `_src`.
	size_t						_line; // @brief Linha atual do cursor.
	size_t						_line_col; // @brief Coluna atual do cursor.

	/**
	 * @brief Lê o arquivo e popula as estruturas internas.
	 *
	 * Em caso de falha, preenche a mensagem de erro.
	 */
	void	_readFile()
	{
		std::ifstream file(_name.c_str());
		if (!file.is_open())
		{
			_error = "Cannot open file '" + _name + "'.";
			return;
		}
		std::string	line;
		while (std::getline(file, line))
		{
			_lines.push_back(line);
			_src += line;
			_src += '\n';
		}
	}

public:
	/**
	 * @brief Constrói o objeto e tenta carregar o arquivo informado.
	 *
	 * @param name Caminho do arquivo.
	 */
	File(std::string name)
		: _name(name), _error(), _src(), _idx(0), _line(1), _line_col(0) { _readFile(); };
	// @brief Constrói um objeto vazio.
	File()
		: _name(), _error(), _src(), _idx(0), _line(1), _line_col(0) {};
	~File() {};

	// @brief Executa a leitura do arquivo caso ainda não tenha sido carregado.
	void				run()
	{
		if (_src.empty())
			_readFile();
	}

	/**
	 * @brief Define um novo arquivo e realiza sua leitura.
	 *
	 * @param file_name Caminho do arquivo.
	 */
	void				setName(const std::string &file_name) { _name = file_name; _readFile(); }

	/**
	 * @brief Obtém o nome do arquivo.
	 *
	 * @return Referência para o nome do arquivo.
	 */
	const std::string	&name() const { return _name; }

	/**
	 * @brief Obtém a linha atual.
	 *
	 * @return Número da linha atual.
	 */
	size_t				line() const { return _line; }

	/**
	 * @brief Obtém a coluna atual.
	 *
	 * @return Número da coluna atual.
	 */
	size_t				lineColumn() const { return _line_col; }

	/**
	 * @brief Obtém a posição atual do cursor.
	 *
	 * @return Índice atual no conteúdo.
	 */
	size_t				cursor() const { return _idx; }

	/**
	 * @brief Obtém o tamanho total do conteúdo.
	 *
	 * @return Quantidade de caracteres carregados.
	 */
	size_t				size() const { return _src.size(); }

	/**
	 * @brief Verifica se ocorreu algum erro.
	 *
	 * @return true se houver erro.
	 */
	bool				hasError() { return !_error.empty(); }

	/**
	 * @brief Obtém a mensagem de erro.
	 *
	 * @return Referência para a mensagem de erro.
	 */
	const std::string	&error() const { return _error; }

	/**
	 * @brief Verifica se a sequência a partir do cursor coincide com uma string.
	 *
	 * @param str Texto a ser comparado.
	 * @return true se houver correspondência.
	 */
	bool				nextIs(const std::string &str)
	{
		if (str.size() + _idx > size())
			return false;
		for (size_t i = 0; i < str.size(); i++)
			if (_src[_idx + i] != str[i])
				return false;
		return true;
	}

	/**
	 * @brief Retorna uma substring a partir do cursor atual.
	 *
	 * @param size Quantidade máxima de caracteres.
	 * @return Substring extraída.
	 */
	std::string			substr_forward(size_t size = std::numeric_limits<size_t>::max())
	{
		return _src.substr(_idx, size);
	}

	/**
	 * @brief Retorna uma substring entre uma posição anterior e o cursor.
	 *
	 * @param pos Posição inicial.
	 * @return Substring extraída.
	 */
	std::string			substr_back(size_t pos)
	{
		if (pos >= _idx) return "";
		return _src.substr(pos, _idx - pos + 1);
	}

	/**
	 * @brief Retorna uma substring a partir de uma posição específica.
	 *
	 * @param pos Posição inicial.
	 * @param size Quantidade máxima de caracteres.
	 * @return Substring extraída.
	 */
	std::string			substr(size_t pos, size_t size = std::numeric_limits<size_t>::max())
	{
		if (pos >= _idx) return "";
		return _src.substr(pos, size);
	}

	/**
	 * @brief Obtém o conteúdo de uma linha específica.
	 *
	 * @param idx Índice da linha.
	 * @return Ponteiro para a linha ou NULL se inválida.
	 */
	const std::string	*lineContentAt(size_t idx) const
	{
		if (idx >= _lines.size()) return NULL;
		return &_lines[idx];
	}

	/**
	 * @brief Obtém o conteúdo da linha atual.
	 *
	 * @return Ponteiro para a linha atual ou NULL se inválida.
	 */
	const std::string	*lineContent() const
	{
		if (_line >= _lines.size()) return NULL;
		return &_lines[_line];
	}

	/**
	 * @brief Verifica se o cursor ainda está dentro do conteúdo.
	 *
	 * @return true enquanto houver caracteres disponíveis.
	 */
	operator			bool() { return _idx < _src.size(); }

	/**
	 * @brief Avança o cursor múltiplas posições.
	 *
	 * @param value Quantidade de caracteres.
	 * @return Referência para o objeto.
	 */
	File				&operator+=(size_t value)
	{
		while (value > 0 && _idx < _src.size())
		{
			++(*this);
			value--;
		}
		return *this;
	}

	/**
	 * @brief Pós-incremento do cursor.
	 *
	 * @return Cópia do estado anterior.
	 */
	File				operator++(int)
	{
		File	copy(*this);
		if (_src[_idx] == '\n')
		{
			++(*this);
			return copy;
		}
		++(*this);
		return copy;
	}

	/**
	 * @brief Pré-incremento do cursor.
	 *
	 * Atualiza linha e coluna automaticamente.
	 *
	 * @return Referência para o objeto.
	 */
	File				&operator++()
	{
		if (_idx == _src.size()) return *this;
		if (_src[_idx] == '\n')
		{
			_line_col = 0;
			_line++;
			_idx++;
			return *this;
		}
		_idx++;
		_line_col++;
		return *this;
	}

	/**
	 * @brief Retrocede o cursor múltiplas posições.
	 *
	 * @param value Quantidade de caracteres.
	 * @return Referência para o objeto.
	 */
	File				&operator-=(size_t value)
	{
		if (value > _idx)
			value = _idx + 1;
		while (value > 0)
		{
			--(*this);
			--value;
		}
		return *this;
	}

	/**
	 * @brief Pós-decremento do cursor.
	 *
	 * @return Cópia do estado anterior.
	 */
	File				operator--(int)
	{
		File	copy(*this);
		if (_src[_idx] == '\n')
		{
			--(*this);
			return copy;
		}
		--(*this);
		return copy;
	}

	/**
	 * @brief Pré-decremento do cursor.
	 *
	 * Atualiza linha e coluna automaticamente.
	 *
	 * @return Referência para o objeto.
	 */
	File				&operator--()
	{
		if (_idx == 0) return *this;
		_idx--;
		if (_src[_idx] == '\n')
		{
			_line--;
			_line_col = _lines[_line].size();
			return *this;
		}
		_line_col--;
		return *this;
	}

	/**
	 * @brief Obtém o caractere atual.
	 *
	 * @return Referência para o caractere apontado pelo cursor.
	 */
	char				&operator*()
	{
		return _src[_idx];
	}

	/**
	 * @brief Obtém o caractere atual (versão constante).
	 *
	 * @return Referência constante para o caractere atual.
	 */
	const char			&operator*() const
	{
		return _src[_idx];
	}

	char				&operator->()
	{
		return _src[_idx];
	}

	const char			&operator->() const
	{
		return _src[_idx];
	}
};

#endif
