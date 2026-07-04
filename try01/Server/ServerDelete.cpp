#include <sys/stat.h>
#include <cstdio>

#include "Router.hpp"
#include "Server.hpp"

void Server::_serveDelete(const Router &router)
{
    std::string upload_dir = router.config_location->getUploadDir();

    std::string filename = router.req.path.getFilename().string();
    size_t slash = filename.rfind('/');
    if (slash != std::string::npos)
        filename = filename.substr(slash + 1);

    if (filename.empty())
        return router.error.badRequest();

    std::string file_path = upload_dir + "/" + filename;

    if (file_path.find("..") != std::string::npos)
        return router.error.forbiden();

    struct stat st;
    if (stat(file_path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
        return router.error.notFound();

    if (std::remove(file_path.c_str()) != 0)
        return router.error.internalServerError();

    router.res.statusCode(200, "OK");
    router.res.body("Deleted\n");
    router.res.send(ResponseHTTPVersion::HTTP_1_1);
}
