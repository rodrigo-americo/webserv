#include "UploadFactoyry.hpp"
# include "Router.hpp"


bool UploadHandler::_saveFile(const utils::str &path, const utils::str &data)
{
	std::ofstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		return false;
	file.write(data.c_str(), data.size());
	return file.good();
}
UploadHandler::UploadHandler(utils::str media_type, const Router *router): _media_type(media_type), _router(router) {};
UploadHandler::UploadHandler(utils::str media_type): _media_type(media_type), _router(NULL) {};
UploadHandler::~UploadHandler() {};
const utils::str		&UploadHandler::mediaType() const { return _media_type; }

utils::str				UploadHandler::_uploadPath(const utils::str &filename) const
{
	if (filename.empty())
	{
		_router->error.badRequest();
		return "";
	}

	FileSystem	upload_dir(_router->config_location->getUploadDir());
	if (!upload_dir.isDir())
	{
		_router->error.internalServerError();
		return "";
	}
	Path final_path = upload_dir.getUniquePathTo(filename);
	return final_path.getPath();
}


// Factory

UploadFactoyry::UploadFactoyry() {};
UploadFactoyry::~UploadFactoyry() {};
void	UploadFactoyry::_addItem(UploadHandler *item)
{
	if (item->mediaType().empty()) return;
	std::pair<iterator, bool> result = _items.insert(std::make_pair(item->mediaType(), item));
	if (!result.second)
		delete item;
}

utils::str	UploadFactoyry::_getMediaType(const Router *router)
{
	const std::string	&content_type = router->req.headers.content_type();
	size_t	semicolon_idx = content_type.find(";");
	if (semicolon_idx == std::string::npos)
		return content_type;
	return content_type.substr(0, semicolon_idx);
}
UploadHandler	*UploadFactoyry::_create(const Router *router)
{
	const utils::str	&media_type = _getMediaType(router);
	iterator	it = _items.find(media_type);
	if (it != _items.end())
		return it->second->copy(router);
	return NULL;
}
