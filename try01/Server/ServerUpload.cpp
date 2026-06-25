#include "Server.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

// "foto.png" -> "foto(1).png", "foto(2).png", ...
// "arquivo"  -> "arquivo(1)", "arquivo(2)", ...
static std::string _uniquePath(const std::string &dir, const std::string &filename)
{
    std::string base = filename;
    std::string ext;

    size_t dot = filename.rfind('.');
    if (dot != std::string::npos)
    {
        base = filename.substr(0, dot);
        ext  = filename.substr(dot);
    }

    std::string path = dir + "/" + filename;
    struct stat st;
    int n = 1;
    while (stat(path.c_str(), &st) == 0)
    {
        std::ostringstream oss;
        oss << dir << "/" << base << "(" << n << ")" << ext;
        path = oss.str();
        n++;
    }
    return path;
}

static std::string _extractBoundary(const std::string &content_type)
{
    size_t pos = content_type.find("boundary=");
    if (pos == std::string::npos)
        return "";
    return "--" + content_type.substr(pos + 9);
}

static std::string _extractFilename(const std::string &disposition)
{
    size_t pos = disposition.find("filename=\"");
    if (pos == std::string::npos)
        return "";
    pos += 10;
    size_t end = disposition.find("\"", pos);
    if (end == std::string::npos)
        return "";
    return disposition.substr(pos, end - pos);
}

static bool _saveFile(const std::string &path, const std::string &data)
{
    std::ofstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return false;
    file.write(data.c_str(), data.size());
    return file.good();
}

static void _handleMultipart(const HttpRequest &req, HttpResponse &res,
                              const std::string &upload_dir, const std::string &boundary)
{
    const std::string &body = req.body;
    std::string delimiter = boundary + "\r\n";
    std::string end_marker = boundary + "--";

    size_t pos = body.find(delimiter);
    if (pos == std::string::npos)
        return (void)res; // corpo malformado, mas já validado antes

    int saved = 0;
    while (pos != std::string::npos)
    {
        pos += delimiter.size();

        // parsear headers da parte
        size_t header_end = body.find("\r\n\r\n", pos);
        if (header_end == std::string::npos)
            break;
        std::string part_headers = body.substr(pos, header_end - pos);
        pos = header_end + 4;

        // encontrar próximo boundary
        size_t next = body.find("\r\n" + boundary, pos);
        if (next == std::string::npos)
            break;
        std::string part_data = body.substr(pos, next - pos);

        // extrair filename do Content-Disposition
        size_t disp_pos = part_headers.find("Content-Disposition:");
        if (disp_pos == std::string::npos)
        {
            pos = body.find(delimiter, next);
            continue;
        }
        size_t disp_end = part_headers.find("\r\n", disp_pos);
        std::string disposition = part_headers.substr(disp_pos, disp_end - disp_pos);
        std::string filename = _extractFilename(disposition);

        if (filename.empty())
        {
            // parte sem arquivo (campo de texto), pular
            pos = body.find(delimiter, next);
            continue;
        }

        std::string final_path = _uniquePath(upload_dir, filename);
        if (!_saveFile(final_path, part_data))
        {
            res.statusCode(500, "Internal Server Error");
            res.body("Failed to save file\n");
            res.send(ResponseHTTPVersion::HTTP_1_1);
            return;
        }
        saved++;
        pos = body.find(delimiter, next);
    }

    if (saved == 0)
    {
        res.statusCode(400, "Bad Request");
        res.body("No valid file found in multipart body\n");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    res.statusCode(201, "Created");
    res.body("Upload OK\n");
    res.send(ResponseHTTPVersion::HTTP_1_1);
}

static void _handleOctetStream(const HttpRequest &req, HttpResponse &res,
                                const std::string &upload_dir)
{
    // nome do arquivo vem do path: POST /upload/foto.png -> "foto.png"
    std::string filename = req.path;
    size_t slash = filename.rfind('/');
    if (slash != std::string::npos)
        filename = filename.substr(slash + 1);

    if (filename.empty())
    {
        res.statusCode(400, "Bad Request");
        res.body("No filename in path\n");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }

    std::string final_path = _uniquePath(upload_dir, filename);
    if (!_saveFile(final_path, req.body))
    {
        res.statusCode(500, "Internal Server Error");
        res.body("Failed to save file\n");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    res.statusCode(201, "Created");
    res.body("Upload OK\n");
    res.send(ResponseHTTPVersion::HTTP_1_1);
}

void Server::_serveUpload(const HttpRequest &req, HttpResponse &res,
                           const LocationConfig &location)
{
    std::string upload_dir = location.getUploadDir();

    // garantir que o diretório existe
    struct stat st;
    if (stat(upload_dir.c_str(), &st) != 0)
    {
        if (mkdir(upload_dir.c_str(), 0755) != 0)
            return _sendError(res, 500, "Internal Server Error", &req);
    }
    else if (!S_ISDIR(st.st_mode))
        return _sendError(res, 500, "Internal Server Error", &req);

    const std::string &ct = req.headers.content_type();

    if (ct.find("multipart/form-data") != std::string::npos)
    {
        std::string boundary = _extractBoundary(ct);
        if (boundary.empty())
            return _sendError(res, 400, "Bad Request", &req);
        _handleMultipart(req, res, upload_dir, boundary);
    }
    else
    {
        _handleOctetStream(req, res, upload_dir);
    }
}
