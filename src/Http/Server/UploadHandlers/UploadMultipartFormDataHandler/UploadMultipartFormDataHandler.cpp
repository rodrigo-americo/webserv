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
};
