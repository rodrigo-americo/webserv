/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileSystem.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 08:00:59 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/03 11:34:27 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_SYSTEM_HPP
# define FILE_SYSTEM_HPP

# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>

# include <sstream>
# include <fstream>

# include "str.hpp"

class FileSystem
{
private:
	utils::str				_path;
	struct stat				_stat;
	bool					_exists;
	bool					_isfile;
	bool					_isdir;
	bool					_isreadable;
	bool					_iswritable;
	bool					_isexecutable;
	std::vector<utils::str>	_children;

	void	_init()
	{
		_exists = stat(_path.c_str(), &_stat) == 0;
		if (!_exists) return;
		_isfile = S_ISREG(_stat.st_mode);
		_isdir = S_ISDIR(_stat.st_mode);
		_isreadable = access(_path.c_str(), R_OK) == 0;
		_iswritable = access(_path.c_str(), W_OK) == 0;
		_isexecutable = access(_path.c_str(), X_OK) == 0;
	}

public:
	FileSystem(const utils::str &path)
		: _path(path), _stat(), _exists(false), _isfile(false), _isdir(false) { _init(); };
	~FileSystem() {};

	FileSystem	&cd(const utils::str &path)
	{
		FileSystem	fs(path);
		if (fs.isDir())
			*this = fs;
		return *this;
	}

	bool							exists() const { return _exists; }
	bool							isFile() const { return _isfile; }
	bool							isDir() const { return _isdir; }
	bool							isReadable() const { return _isreadable; }
	bool							isWritable() const { return _iswritable; }
	bool							isExecutable() const { return _isexecutable; }

	const std::vector<utils::str>	*ls()
	{
		if (!isFile()) return NULL;
		if (!_children.empty()) return &_children;
		DIR	*dir = opendir(_path.c_str());

		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			utils::str	name = entry->d_name;
			if (name == "." || name == "..") continue;
			_children.push_back(name);
		}
		return &_children;
	}

	utils::str	content() const
	{
		if (!isFile() || !isReadable()) return "";
		std::ifstream	file(_path.c_str());
		if (!file.is_open()) return "";
		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}

	size_t	size()
	{
		if (!isFile() && !isDir()) return 0;
		if (isFile()) return _stat.st_size;
		size_t	total_size = 0;
		const std::vector<utils::str>	*children = ls();
		if (!children) return 0;
		for (size_t i = 0; i < children->size(); i++)
			total_size += FileSystem((*children)[i]).size();
		return total_size;
	}
};

#endif
