#include "Server.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

static std::string _mimeType(const std::string& path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos) return "application/octet-stream";
    std::string ext = path.substr(dot);
    if (ext == ".html") return "text/html";
    if (ext == ".css")  return "text/css";
    if (ext == ".js")   return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".png")  return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".ico")  return "image/x-icon";
    return "application/octet-stream";
}


void Server::_serveAutoIndex(const HttpRequest &req, HttpResponse &res, const std::string &dir_path)
{
    DIR *dir = opendir(dir_path.c_str());
    if (!dir)
    {
        std::cout << "!dir \n";
        return _sendError(res, 403, "Forbidden", &req);
    }

    std::string body;
    body += "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    body += "<title>Index of " + req.path.getCleanPath() + "</title>";
    body += "<style>";
    body += "body{background:#0d1117;color:#e6edf3;font-family:'Courier New',monospace;padding:2rem;}";
    body += "h1{color:#58a6ff;margin-bottom:1.5rem;font-size:1.4rem;}";
    body += "table{width:100%;border-collapse:collapse;}";
    body += "tr:hover{background:#161b22;}";
    body += "td,th{padding:0.5rem 1rem;text-align:left;border-bottom:1px solid #21262d;}";
    body += "th{color:#8b949e;font-size:0.75rem;text-transform:uppercase;letter-spacing:0.1em;}";
    body += "a{color:#58a6ff;text-decoration:none;}";
    body += "a:hover{text-decoration:underline;}";
    body += ".size{color:#8b949e;font-size:0.85rem;}";
    body += "</style></head><body>";
    body += "<h1>Index of " + req.path.getCleanPath() + "</h1>";
    body += "<table><tr><th>nome</th><th>tamanho</th></tr>";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == ".") continue;

        std::string full = dir_path + name;
        struct stat st;
        bool is_dir = false;
        std::string size_str = "-";

        if (stat(full.c_str(), &st) == 0)
        {
            is_dir = S_ISDIR(st.st_mode);
            if (!is_dir)
            {
                std::ostringstream oss;
                if (st.st_size < 1024)
                    oss << st.st_size << " B";
                else if (st.st_size < 1024 * 1024)
                    oss << st.st_size / 1024 << " KB";
                else
                    oss << st.st_size / (1024 * 1024) << " MB";
                size_str = oss.str();
            }
        }

        std::string display = is_dir ? name + "/" : name;
        std::string base = req.path.getCleanPath().string();
        if (base.empty() || base[base.size() - 1] != '/')
            base += '/';
        std::string href = base + display;

        body += "<tr><td><a href=\"" + href + "\">" + display + "</a></td>";
        body += "<td class=\"size\">" + size_str + "</td></tr>";
    }
    closedir(dir);

    body += "</table></body></html>";

    res.statusCode(200, "OK");
    res.headers.content_type("text/html");
    res.body(body);
    res.send(ResponseHTTPVersion::HTTP_1_1);
}


void Server::_serveStatic(const HttpRequest &req, HttpResponse &res,
                           const ServerConfig &, const LocationConfig &location)
{
    std::string root = location.resolveRoot();
    
    std::string file_path = root + req.path.getCleanPath();
    if (!req.path.isNormalizable())
    {
        std::cout << "!req.path.isNormalizable() \n";
        return _sendError(res, 403, "Forbidden", &req);
    }
    struct stat st;
    if (stat(file_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
    {
        if (file_path[file_path.size() - 1] != '/')
            file_path += '/';

        const std::list<std::string>& indexes = location.getIndex();
        bool found = false;
        for (std::list<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); ++it)
        {
            std::string candidate = file_path + *it;
            std::ifstream f(candidate.c_str());
            if (f.good()) { file_path = candidate; found = true; break; }
        }
        if (!found)
        {
            if (location.getAutoIndex())
                return _serveAutoIndex(req, res, file_path);
            return _sendError(res, 403, "Forbidden", &req);
        }
    }
    if (stat(file_path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
        return _sendError(res, 404, "Not Found", &req);

    res.statusCode(200, "OK");
    res.headers.content_type(_mimeType(file_path));
    // sendFile() so guarda o fd e o tamanho; o conteudo e lido do disco em
    // pedacos e mandado pro socket via sendfile() conforme o loop de eventos
    // for avisando que o fd do cliente esta gravavel (ver SocketConnection::flushWrite).
    if (!res.sendFile(file_path, static_cast<size_t>(st.st_size), ResponseHTTPVersion::HTTP_1_1))
        return _sendError(res, 404, "Not Found", &req);
}
