#ifndef UPLOAD_MULTIPART_HANDLER_HPP
# define UPLOAD_MULTIPART_HANDLER_HPP

# include <utility>

# include "UploadFactoyry.hpp"
# include "UploadMultipartFormDataIterator.hpp"

class Router;

class UploadMultipartFormDataHandler: public UploadHandler, public UploadFactoyryRegister<UploadMultipartFormDataHandler>
{
	private:

		utils::str	_getBoundary() const;

	public:
		UploadMultipartFormDataHandler(const Router *router): UploadHandler("multipart/form-data", router) {};
		UploadMultipartFormDataHandler(): UploadHandler("multipart/form-data") {};
		~UploadMultipartFormDataHandler() {};

		UploadHandler	*copy(const Router *router);
		void			saveFile();
		void			saveFile2();
};

#endif
