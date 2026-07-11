#include <sys/stat.h>
#include <cstdio>

#include "Router.hpp"
#include "Server.hpp"

void Server::_serveDelete(const Router &router)
{
    Path upload_dir = Path(router.config_location->getUploadDir());
    Path filename = Path(router.req.path.getFilename());
    if (!filename.isNormalizable())
        return router.error.badRequest();

    Path file_path = upload_dir + filename;

    if (!file_path.isNormalizable())
        return router.error.forbiden();

    FileSystem fs(file_path.getCleanPath());
    if (!fs.exists())
        return router.error.notFound();
    if (!fs.isFile())
        return router.error.forbiden();

    if (std::remove(file_path.getPath().c_str()) != 0)
        return router.error.internalServerError();

    router.res.statusCode(200, "OK");
    router.res.body("Deleted\n");
    router.res.send(ResponseHTTPVersion::HTTP_1_1);
}
