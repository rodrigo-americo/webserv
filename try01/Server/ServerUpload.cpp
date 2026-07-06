#include "Server.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

# include "Router.hpp"
# include "Path.hpp"
# include "UploadFactoyry.hpp"

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

// static std::string _extractFilename(const std::string &disposition)
// {
//     size_t pos = disposition.find("filename=\"");
//     if (pos == std::string::npos)
//         return "";
//     pos += 10;
//     size_t end = disposition.find("\"", pos);
//     if (end == std::string::npos)
//         return "";
//     return disposition.substr(pos, end - pos);
// }

static bool _saveFile(const std::string &path, const std::string &data)
{
    std::ofstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return false;
    file.write(data.c_str(), data.size());
    return file.good();
}

static void _handleMultipart(const Router &router,
                              const std::string &upload_dir, const std::string &boundary)
{
    const std::string &body = router.req.body;
    std::string delimiter = boundary + "\r\n";
    std::string end_marker = boundary + "--";

    size_t pos = body.find(delimiter);
    if (pos == std::string::npos)
        return (void)router.res; // corpo malformado, mas já validado antes

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
        std::string filename = Path(disposition).getFilename().string();

        if (filename.empty())
        {
            // parte sem arquivo (campo de texto), pular
            pos = body.find(delimiter, next);
            continue;
        }

        std::string final_path = _uniquePath(upload_dir, filename);
        if (!_saveFile(final_path, part_data))
			return router.error.internalServerError("Failed to save file\n");
        saved++;
        pos = body.find(delimiter, next);
    }

    if (saved == 0)
		return router.error.badRequest("Failed to save file\n");

    router.res.statusCode(201, "Created");
    router.res.body("Upload OK\n");
    router.res.send(ResponseHTTPVersion::HTTP_1_1);
}

static void _handleOctetStream(const Router &router,
                                const std::string &upload_dir)
{
    // nome do arquivo vem do path: POST /upload/foto.png -> "foto.png"
    std::string filename = router.req.path.getCleanPath().string();
    size_t slash = filename.rfind('/');
    if (slash != std::string::npos)
        filename = filename.substr(slash + 1);

    if (filename.empty())
		return router.error.badRequest("No filename in path\n");

    std::string final_path = _uniquePath(upload_dir, filename);
    if (!_saveFile(final_path, router.req.body))
		return router.error.internalServerError("Failed to save file\n");
    router.res.statusCode(201, "Created");
    router.res.body("Upload OK\n");
    router.res.send(ResponseHTTPVersion::HTTP_1_1);
}

void Server::_serveUpload(const Router &router)
{
    std::string upload_dir = router.config_location->getUploadDir();

	FileSystem	fs(upload_dir);
    if (!fs.exists())
    {
		// mkdir - e uma funcao proibida
        if (mkdir(upload_dir.c_str(), 0755) != 0)
            return router.error.internalServerError();
    }
    else if (!fs.isDir())
        return router.error.internalServerError();

    const std::string &ct = router.req.headers.content_type();

    if (ct.find("multipart/form-data") != std::string::npos)
    {
        std::string boundary = _extractBoundary(ct);
        if (boundary.empty())
            return router.error.badRequest();
        _handleMultipart(router, upload_dir, boundary);
    }
    else
        _handleOctetStream(router, upload_dir);
}

void Server::_serveUpload2(const Router &router)
{
	UploadHandler	*handler = UploadFactoyry::create(&router);
	if (!handler)
		return;
	handler->saveFile();
}
