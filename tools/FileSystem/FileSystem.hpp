#ifndef FILE_SYSTEM_HPP
# define FILE_SYSTEM_HPP

# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>

# include <sstream>
# include <fstream>
# include <iostream>

# include "str.hpp"
# include "Path.hpp"

class FileSystem
{
private:
	Path					_path;
	struct stat				_stat;
	bool					_exists;
	bool					_isfile;
	bool					_isdir;
	bool					_isreadable;
	bool					_iswritable;
	bool					_isexecutable;

	void	_init()
	{
		_exists = stat(_path.getPath().c_str(), &_stat) == 0;
		if (!_exists) return;
		_isfile = S_ISREG(_stat.st_mode);
		_isdir = S_ISDIR(_stat.st_mode);
		_isreadable = access(_path.getPath().c_str(), R_OK) == 0;
		_iswritable = access(_path.getPath().c_str(), W_OK) == 0;
		_isexecutable = access(_path.getPath().c_str(), X_OK) == 0;
	}

public:
	FileSystem(const utils::str &path)
		: _path(path), _stat(), _exists(false), _isfile(false), _isdir(false),
		_isreadable(false), _iswritable(false), _isexecutable(false) { _init(); };
	~FileSystem() {};

	FileSystem	&operator=(const FileSystem &other)
	{
		if (this == &other) return *this;
		_path = other._path;
		_stat = other._stat;
		_exists = other._exists;
		_isfile = other._isfile;
		_isdir = other._isdir;
		_isreadable = other._isreadable;
		_iswritable = other._iswritable;
		_isexecutable = other._isexecutable;
		return *this;
	}

	Path							path() { return _path; }
	bool							exists() const { return _exists; }
	bool							isFile() const { return _isfile; }
	bool							isDir() const { return _isdir; }
	bool							isReadable() const { return _isreadable; }
	bool							isWritable() const { return _iswritable; }
	bool							isExecutable() const { return _isexecutable; }

	FileSystem	&cd(const Path &path)
	{
		FileSystem	fs((_path + path).getPath());
		if (isDir())
			*this = fs;
		_init();
		return *this;
	}

	std::vector<Path>	ls()
	{
		std::vector<Path> result;
		if (!isDir()) return result;
		DIR	*dir = opendir(_path.getPath().c_str());

		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			utils::str	name = entry->d_name;
			if (name == "." || name == "..") continue;
			result.push_back(Path(name));
		}
		return result;
	}

	bool	hasChild(const utils::str &name)
	{
		std::vector<Path>	childs = ls();
		if (childs.empty()) return false;
		for (size_t i = 0; i < childs.size(); i++)
			if (childs[i].getPath() == name)
				return true;
		return false;
	}

	utils::str	content() const
	{
		if (!isFile() || !isReadable()) return "";
		std::ifstream	file(_path.getPath().c_str());
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
		const std::vector<Path>	children = ls();
		if (children.empty()) return 0;
		for (size_t i = 0; i < children.size(); i++)
			total_size += FileSystem((children)[i].getPath()).size();
		return total_size;
	}
};

#endif
