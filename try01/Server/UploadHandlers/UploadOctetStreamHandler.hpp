#ifndef UPLOAD_OCTET_HANDLER_HPP
# define UPLOAD_OCTET_HANDLER_HPP

# include "UploadFactoyry.hpp"

class UploadOctetStreamHandler: public UploadHandler, public UploadFactoyryRegister<UploadOctetStreamHandler>
{
private:

public:
	UploadOctetStreamHandler(const Router *router): UploadHandler("application/octet-stream", router) {};
	UploadOctetStreamHandler(): UploadHandler("application/octet-stream") {};
	~UploadOctetStreamHandler() {};

	UploadHandler	*copy(const Router *router);
	void			saveFile();
};

#endif
