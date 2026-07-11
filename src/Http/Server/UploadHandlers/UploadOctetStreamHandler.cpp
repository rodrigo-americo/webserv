#include "UploadOctetStreamHandler.hpp"
#include "Router.hpp"

UploadHandler	*UploadOctetStreamHandler::copy(const Router *router)
{
	return new UploadOctetStreamHandler(router);
}

void			UploadOctetStreamHandler::saveFile()
{
	LOG_TRACE("saving file: " << _router->req.path.getFilename());
	utils::str upload_path = _uploadPath(_router->req.path.getFilename());
	if (upload_path.empty())
		return;
	if (!_saveFile(upload_path, _router->req.body))
		return _router->error.internalServerError();
	_router->res.statusCode(201, "Created");
	_router->res.body("Upload OK\n");
	_router->res.send(ResponseHTTPVersion::HTTP_1_1);
}
