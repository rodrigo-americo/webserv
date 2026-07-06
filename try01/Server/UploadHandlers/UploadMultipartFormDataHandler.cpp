#include "UploadMultipartFormDataHandler.hpp"
#include "Router.hpp"

utils::str	UploadMultipartFormDataHandler::_getBoundary() const
{
	const std::string	&content_type = _router->req.headers.content_type();
	size_t pos = content_type.find("boundary=");
    if (pos == std::string::npos)
        return "";
    return "--" + content_type.substr(pos + 9);
};

UploadHandler	*UploadMultipartFormDataHandler::copy(const Router *router)
{
	return new UploadMultipartFormDataHandler(router);
};
void			UploadMultipartFormDataHandler::saveFile()
{
	utils::str boundary = _getBoundary();
	const std::string &body = _router->req.body;
	std::string delimiter = boundary.string() + "\r\n";
	utils::str end_marker = boundary + "--";

	size_t pos = body.find(delimiter);
	if (pos == utils::str::npos)
		return; // corpo malformado, mas já validado antes

	int saved = 0;
	while (pos != utils::str::npos)
	{
		pos += delimiter.size();

		// parsear headers da parte
		size_t header_end = body.find("\r\n\r\n", pos);
		if (header_end == utils::str::npos)
			break;
		utils::str part_headers = body.substr(pos, header_end - pos);
		pos = header_end + 4;

		// encontrar próximo boundary
		size_t next = body.find("\r\n" + boundary, pos);
		if (next == utils::str::npos)
			break;
		utils::str part_data = body.substr(pos, next - pos);

		// extrair filename do Content-Disposition
		size_t disp_pos = part_headers.find("Content-Disposition:");
		if (disp_pos == utils::str::npos)
		{
			pos = body.find(delimiter, next);
			continue;
		}
		size_t disp_end = part_headers.find("\r\n", disp_pos);
		utils::str disposition = part_headers.substr(disp_pos, disp_end - disp_pos);
		utils::str filename = Path(disposition).getFilename().string();

		if (filename.empty())
		{
			// parte sem arquivo (campo de texto), pular
			pos = body.find(delimiter, next);
			continue;
		}

		utils::str upload_path = _uploadPath(filename);
		if (!_saveFile(upload_path, part_data))
			return _router->error.internalServerError();
		saved++;
		pos = body.find(delimiter, next);
	}

	if (saved == 0)
		return _router->error.badRequest();

	_router->res.statusCode(201, "Created");
	_router->res.body("Upload OK\n");
	_router->res.send(ResponseHTTPVersion::HTTP_1_1);
};
