#ifndef UPLOAD_FACTORY_HPP
# define UPLOAD_FACTORY_HPP

# include <fstream>
# include <iostream>
# include <map>
# include "str.hpp"
# include "singleton.hpp"
# include "factory.hpp"


class Router;

class UploadHandler
{
	protected:
		utils::str	_media_type;
		const Router *_router;

		bool		_saveFile(const utils::str &path, const utils::str &data);
		utils::str	_uploadPath(const utils::str &filename) const;

	public:
		UploadHandler(utils::str media_type, const Router *router);
		UploadHandler(utils::str media_type);
		virtual ~UploadHandler();

		virtual UploadHandler	*copy(const Router *router) = 0;
		virtual void			saveFile() = 0;
		const utils::str		&mediaType() const;

};

class UploadFactoyry: public patterns::factory<UploadFactoyry, utils::str, UploadHandler, const Router *>
{
	friend class patterns::singleton<UploadFactoyry>;
	friend class patterns::factory<UploadFactoyry, utils::str, UploadHandler, const Router *>;
	private:

		UploadFactoyry();
		UploadFactoyry(const UploadFactoyry &);
		UploadFactoyry	&operator=(const UploadFactoyry &);
		~UploadFactoyry();

		void	_addItem(UploadHandler *item);
		static utils::str	_getMediaType(const Router *router);
		UploadHandler	*_create(const Router *router);
};

#endif
