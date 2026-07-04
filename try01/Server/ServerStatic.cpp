#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

#include "Logger.hpp"
#include "FileSystem.hpp"
#include "Router.hpp"
#include "Server.hpp"

static std::string _mimeType(const FileSystem fs)
{
	LOG_INFO("mime_type...");
    //size_t dot = fs.path().getPath().rfind('.');
    //if (dot == std::string::npos) return "application/octet-stream";
     if (fs.path().getExtension().empty()) return "application/octet-stream";
    //utils::str ext = fs.path().getPath().substr(dot);
    utils::str ext = fs.path().getExtension();
    if (ext == ".html") return "text/html";
    if (ext == ".css")  return "text/css";
    if (ext == ".js")   return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".png")  return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".ico")  return "image/x-icon";
    return "application/octet-stream";
}


void Server::_serveAutoIndex(const Router &router, const FileSystem &fs)
{
    if (!fs.exists())
    {
        LOG_WARN("autoindex: Path not found '" << fs.path().getPath() << "'");
        return router.error.forbiden();
    }

    std::string body;
    body += "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    body += "<title>Index of " + router.req.path.getCleanPath() + "</title>";
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
    body += "<h1>Index of " + router.req.path.getCleanPath() + "</h1>";
    body += "<table><tr><th>nome</th><th>tamanho</th></tr>";

	std::vector<FileSystem>	children = fs.ls();

	for (size_t i = 0; i < children.size(); i++)
	{
		utils::str size_str;
		size_str = children[i].sizeStr();

        Path name(children[i].isDir() ? children[i].path().getFilename() + "/" : children[i].path().getFilename());
        utils::str href = (router.req.path + name).getCleanPath();

		body += "<tr><td><a href=\"" + href + "\">" + name.getCleanPath() + "</a></td>";
        body += "<td class=\"size\">" + size_str + "</td></tr>";
	}

    body += "</table></body></html>";

    router.res.statusCode(200, "OK");
    router.res.headers.content_type("text/html");
    router.res.body(body);
    router.res.send(ResponseHTTPVersion::HTTP_1_1);
}


void Server::_serveStatic(const Router &router)
{
	LOG_INFO("static server.");

    Path path = router.config_location->resolveRoot() + router.req.path;
    std::string file_path =  path.getCleanPath().string(); //root + router.req.path.getCleanPath();
    if (!router.req.path.isNormalizable())
        return router.error.forbiden();

	LOG_INFO("path '" << router.req.path << "' is normalized");

	FileSystem	fs(file_path);
	LOG_INFO("path '" << file_path << "' isDir: " << utils::to_string(fs.isDir()));
    if (fs.isDir())
    {
        if (file_path[file_path.size() - 1] != '/')
            file_path += '/';

        const std::list<std::string>& indexes = router.config_location->getIndex();
        bool found = false;
        for (std::list<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); ++it)
        {
            std::string candidate = file_path + *it;
            LOG_DEBUG("candidate: " << candidate);
            std::ifstream f(candidate.c_str());
            if (f.good()) { found = true; fs.cd(Path(*it)); break; }
        }
        if (!found)
        {
            if (router.config_location->getAutoIndex())
                return _serveAutoIndex(router, fs);
            LOG_DEBUG("sending forbiden...");
            return router.error.forbiden();
        }
    }
	LOG_INFO("path '" << fs.path().getPath() << "' isFile: " << utils::to_string(fs.isFile()));
    if (!fs.isFile())
        return router.error.notFound();

    router.res.statusCode(200, "OK");
    router.res.headers.content_type(_mimeType(fs));
    // sendFile() so guarda o fd e o tamanho; o conteudo e lido do disco em
    // pedacos e mandado pro socket via sendfile() conforme o loop de eventos
    // for avisando que o fd do cliente esta gravavel (ver SocketConnection::flushWrite).
	LOG_INFO("sending file '" << fs.path().getPath() << "'");
    if (!router.res.sendFile(fs, ResponseHTTPVersion::HTTP_1_1))
        return router.error.notFound();
}
