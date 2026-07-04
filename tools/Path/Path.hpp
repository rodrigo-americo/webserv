#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <sstream>

#include "str.hpp"

class Path
{
private:
    utils::str _path;
    utils::str _normalized_path;
    utils::str _filename;
    utils::str _basename;
    utils::str _extension;
    utils::str _query_string;
    utils::str _clean_path;
    utils::str _last_dir;
    size_t _ext_pos;
    size_t _last_slash_pos;
    size_t _q_pos;

    void normalizePath()
    {
        std::vector<utils::str> stack;
        std::stringstream ss(_path.string());
        utils::str component;

        if (_path.empty())
            return;
        bool is_absolute = (_path[0] == '/');
        while (std::getline(ss, component.string(), '/'))
        {
            if (component.empty() || component == ".") {
                continue;
            } else if (component == "..") {
                if (stack.empty())
                    return;
                stack.pop_back();
            } else {
                stack.push_back(component);
            }
        }
        _normalized_path = is_absolute ? "/" : "";
        for (size_t i = 0; i < stack.size(); ++i)
        {
            _normalized_path += stack[i];
            if (i + 1 < stack.size()) _normalized_path += "/";
        }
        if (_normalized_path.empty())
        {
           _normalized_path = "/";
        }
        
    }

    void buildQueryString()
    {
        if (_normalized_path.empty() || _q_pos == utils::str::npos)
            return;
        _query_string = _normalized_path.substr(_q_pos + 1);
    }

    void buildCleanPath()
    {
        if (_normalized_path.empty())
            return;
        if (_q_pos != utils::str::npos)
            _clean_path = _normalized_path.substr(0, _q_pos);
        else
            _clean_path = _normalized_path;
    }
    
    void buildExtension()
    {
        if (_clean_path.empty() || _ext_pos == utils::str::npos)
            return;
        if (_ext_pos > _last_slash_pos)
            _extension = _clean_path.substr(_ext_pos, _clean_path.size() - _ext_pos);        
    }

    void buildFilename()
    {
        if (_clean_path.empty() || _last_slash_pos == utils::str::npos)
            return;
        _filename = _clean_path.substr(_last_slash_pos + 1, _clean_path.size() - _last_slash_pos);
    }
    
    void buildBasename()
    {
        if (_filename.empty())
            return;
        else if (_ext_pos > _last_slash_pos)
            _basename = _clean_path.substr(_last_slash_pos + 1, _ext_pos - _last_slash_pos - 1);
        else
            _basename = _filename;
    }
    
    void builLastDir()
    {
        if (_clean_path.empty())
            return;
        if (_last_slash_pos != utils::str::npos)
            _last_dir = _clean_path.substr(0, _last_slash_pos + 1);
    }
    
public:
    Path(const utils::str& path_) : _path(path_)
    {
        normalizePath();
        if (_normalized_path.empty())
            return;
        _q_pos = _normalized_path.find_first_of("?");
        buildQueryString();
        buildCleanPath();
        _ext_pos = _clean_path.find_last_of(".");
        _last_slash_pos = _clean_path.find_last_of("/");
        buildExtension();
        buildFilename();
        buildBasename();
        builLastDir();
    };
    
    ~Path() {};
    utils::str getFilename() const { return _filename; }
    utils::str getBasename() const { return _basename; }
    utils::str getExtension() const { return _extension; }
    utils::str getPath() const { return _path; }
    utils::str getNormalizedPath() const { return _normalized_path; }
    utils::str getQueryString() const { return _query_string; }
    utils::str getCleanPath() const { return _clean_path; }
    utils::str getLastDir() const { return _last_dir; }
    bool isNormalizable() const
    {
        return (!_normalized_path.empty());
    }
    
    Path operator+(const Path& other) const
    {
        utils::str result_path_str;
        if (_clean_path.empty() || other._clean_path.empty())
            return Path("");
        
        if (_clean_path[_clean_path.size() - 1] == '/')
        {
            if (other._clean_path[0] == '/')
                result_path_str = _clean_path.substr(0, _clean_path.size() - 1) + other._clean_path;
            else
                result_path_str = _clean_path + other._clean_path;
        }
        else
        {
            if (other._clean_path[0] == '/')
                result_path_str = _clean_path + other._clean_path;
            else
                result_path_str = _clean_path + "/" + other._clean_path;
        }
        return Path(result_path_str);
    }
    
};

inline std::ostream& operator<<(std::ostream& os, const Path& p)
{
    os << "path{normalized=" << p.getNormalizedPath().string()
       << ", clean=" << p.getCleanPath().string()
       << ", filename=" << p.getFilename().string()
       << ", extension=" << p.getExtension().string()
       << ", query string=" << p.getQueryString().string() << "}";
    return os;
}

#endif