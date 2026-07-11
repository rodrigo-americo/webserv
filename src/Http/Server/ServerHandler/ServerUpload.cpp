#include "Server.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

# include "Router.hpp"
# include "Path.hpp"
# include "UploadFactoyry.hpp"

void Server::_serveUpload(const Router &router)
{
	LOG_TRACE("_serveUpload()");
	UploadHandler	*handler = UploadFactoyry::create(&router);
	if (!handler)
		return router.error.unsupportedMediaType();
	LOG_TRACE("upload.media_type: " << handler->mediaType());
	handler->saveFile();
	delete handler;
}
