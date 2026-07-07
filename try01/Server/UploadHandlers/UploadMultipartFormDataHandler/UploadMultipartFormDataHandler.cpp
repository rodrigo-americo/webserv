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
	saveFile2();
	// utils::str boundary = _getBoundary();
	// const std::string &body = _router->req.body;
	// std::string delimiter = boundary.string() + "\r\n";
	// utils::str end_marker = boundary + "--";

	// size_t pos = body.find(delimiter);
	// if (pos == utils::str::npos)
	// 	return; // corpo malformado, mas já validado antes

	// int saved = 0;
	// /*
	// 	while (it)
	// 	{
	// 		UploadMultipartFormDataPart value = *it;
	// 		value.fileName();
	// 	}
	// */
	// while (pos != utils::str::npos)
	// {
	// 	pos += delimiter.size();

	// 	// parsear headers da parte
	// 	size_t header_end = body.find("\r\n\r\n", pos);
	// 	if (header_end == utils::str::npos)
	// 		break;
	// 	utils::str part_headers = body.substr(pos, header_end - pos);
	// 	pos = header_end + 4;

	// 	// encontrar próximo boundary
	// 	size_t next = body.find("\r\n" + boundary, pos);
	// 	if (next == utils::str::npos)
	// 		break;
	// 	utils::str part_data = body.substr(pos, next - pos);

	// 	// extrair filename do Content-Disposition
	// 	size_t disp_pos = part_headers.find("Content-Disposition:");
	// 	if (disp_pos == utils::str::npos)
	// 	{
	// 		pos = body.find(delimiter, next);
	// 		continue;
	// 	}
	// 	size_t disp_end = part_headers.find("\r\n", disp_pos);
	// 	utils::str disposition = part_headers.substr(disp_pos, disp_end - disp_pos);
	// 	utils::str filename = Path(disposition).getFilename().string();

	// 	if (filename.empty())
	// 	{
	// 		// parte sem arquivo (campo de texto), pular
	// 		pos = body.find(delimiter, next);
	// 		continue;
	// 	}

	// 	LOG_TRACE("saving file: " << filename);
	// 	utils::str upload_path = _uploadPath(filename);
	// 	if (!_saveFile(upload_path, part_data))
	// 		return _router->error.internalServerError();
	// 	saved++;
	// 	pos = body.find(delimiter, next);
	// }

	// if (saved == 0)
	// 	return _router->error.badRequest();

	// _router->res.statusCode(201, "Created");
	// _router->res.body("Upload OK\n");
	// _router->res.send(ResponseHTTPVersion::HTTP_1_1);
};

void			UploadMultipartFormDataHandler::saveFile2()
{
	LOG_TRACE("saveFile2...");
	LOG_INFO("saveFile2::info: ");
	std::vector<std::pair<utils::str, utils::str> >	_parts;
	size_t	iteration = 1;
	UploadMultipartFormDataIterator	it(_router);
	LOG_INFO("iterator: " << it->dispositionFilename());
	while (it)
	{
		LOG_INFO("Iterator loop visits: " << iteration);
		LOG_INFO("it->error(): " << it->error());
		if (it->error())
		{
			LOG_ERROR("Wrong part format...");
			return _router->error.badRequest();
		}
		utils::str	filename;
		LOG_INFO("getting extention...");
		utils::str	ext = it->content_type().empty() ? ".txt" : "." + it->content_type().split("/")[1];
		LOG_INFO("extention OK: " << ext);
		LOG_INFO("dispositionName: " << it->dispositionName() << " is file: " << utils::to_string(it->dispositionName().trim_const() == "file"));
		LOG_INFO("dispositionFilename: " << it->dispositionFilename());
		if (it->dispositionName() == "file" && it->dispositionFilename().empty())
			filename = "unnamed" + ext;
		else if (it->dispositionName() == "file")
			filename = it->dispositionFilename();
		LOG_INFO("pushing part..." << filename);
		LOG_INFO("pushing part: " << _uploadPath(filename) << ": " << it->body());
		_parts.push_back(std::make_pair(_uploadPath(filename), it->body()));
		++it;
	}
	LOG_INFO("Iterator loop ended!");
	for (size_t i = 0; i < _parts.size(); i++)
	{
		LOG_TRACE("saving file: " << _parts[i].first);
		if (!_saveFile(_parts[i].first, _parts[i].second))
		{
			LOG_ERROR("on save file: " << _parts[i].first);
			return _router->error.internalServerError();
		}
	}
	_router->res.statusCode(201, "Created");
	_router->res.body("Upload OK\n");
	_router->res.send(ResponseHTTPVersion::HTTP_1_1);
}
