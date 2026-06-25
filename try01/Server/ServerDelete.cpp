#include "Server.hpp"
#include <sys/stat.h>
#include <cstdio>

void Server::_serveDelete(const HttpRequest &req, HttpResponse &res,
                           const LocationConfig &location)
{
    std::string upload_dir = location.getUploadDir();

    std::string filename = req.path;
    size_t slash = filename.rfind('/');
    if (slash != std::string::npos)
        filename = filename.substr(slash + 1);

    if (filename.empty())
        return _sendError(res, 400, "Bad Request", &req);

    std::string file_path = upload_dir + "/" + filename;

    if (file_path.find("..") != std::string::npos)
        return _sendError(res, 403, "Forbidden", &req);

    struct stat st;
    if (stat(file_path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
        return _sendError(res, 404, "Not Found", &req);

    if (std::remove(file_path.c_str()) != 0)
        return _sendError(res, 500, "Internal Server Error", &req);

    res.statusCode(200, "OK");
    res.body("Deleted\n");
    res.send(ResponseHTTPVersion::HTTP_1_1);
}
