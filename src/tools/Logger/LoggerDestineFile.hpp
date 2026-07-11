/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LoggerDestineFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 21:12:09 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 22:28:05 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_DESTINE_FILE_HPP
# define LOGGER_DESTINE_FILE_HPP

#include <fstream>
# include <iostream>

# include "utils.hpp"
# include "LoggerDestine.hpp"

class LoggerDestineFile: public	LoggerDestine
{
private:
	utils::str	_file_name;

	bool	_fileIsOpen(std::ofstream &file) const
	{
		if (!file.is_open())
		{
			LoggerFormatter *formatter = new LoggerFormatterError();
			LoggerRecord	record(
				LoggerLevel::ERROR,
				"cannot open file '" + _file_name + "'.",
				__FILE__,
				__LINE__,
				utils::time());
			std::cerr << formatter->format(record);
			return false;
		}
		return true;
	}

	void	_testFile() const
	{
		std::ofstream	file(_file_name.c_str(), std::ios::trunc);
		if (!_fileIsOpen(file))
			return;
		file.close();
	}

	void	doWrite(const utils::str &value) const
	{
		std::ofstream	file(_file_name.c_str(), std::ios::app);
		if (!_fileIsOpen(file))
			return;
		file << value;
		file.close();
	}

public:
	LoggerDestineFile(const utils::str &file_name, LoggerLevel::type min_lvl, LoggerLevel::type max_lvl)
		: LoggerDestine(min_lvl, max_lvl), _file_name(file_name) {_testFile();};
	LoggerDestineFile(const utils::str &file_name, LoggerLevel::type lvl)
		: LoggerDestine(lvl), _file_name(file_name) {_testFile();};
	~LoggerDestineFile() {};
};

#endif
