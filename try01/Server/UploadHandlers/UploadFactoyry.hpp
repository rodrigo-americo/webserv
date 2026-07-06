#ifndef UPLOAD_FACTORY_HPP
# define UPLOAD_FACTORY_HPP

# include <fstream>
# include <iostream>
# include <map>
# include "str.hpp"
# include "singleton.hpp"


class Router;

class UploadHandler
{
	protected:
		utils::str	_media_type;
		const Router *_router;

		bool _saveFile(const utils::str &path, const utils::str &data);
		utils::str				_uploadPath(const utils::str &filename) const;

	public:
		UploadHandler(utils::str media_type, const Router *router);
		UploadHandler(utils::str media_type);
		virtual ~UploadHandler();

		virtual UploadHandler	*copy(const Router *router) = 0;
		virtual void			saveFile() = 0;
		const utils::str		&mediaType() const;

};

class UploadFactoyry: public patterns::singleton<UploadFactoyry>
{
	friend class patterns::singleton<UploadFactoyry>;
	private:
		typedef std::map<utils::str, UploadHandler*>::iterator	iterator;
		std::map<utils::str, UploadHandler*>	items;

		UploadFactoyry();
		UploadFactoyry(const UploadFactoyry &);
		UploadFactoyry	&operator=(const UploadFactoyry &);
		~UploadFactoyry();

		void	_addItem(UploadHandler *item);
		static utils::str	_getMediaType(const Router *router);
		UploadHandler	*_create(const Router *router);

	public:
		static void	registerItem(UploadHandler *item);
		static UploadHandler	*create(const Router *router);
};

template <typename T>
class UploadFactoyryRegister
{
	private:
		static bool	_register;
	public:
		UploadFactoyryRegister() { (void)_register; }
};

template <typename Derived>
bool	registerFuntion()
{
	UploadFactoyry::registerItem(new Derived());
	return true;
}

template <typename Derived>
bool	UploadFactoyryRegister<Derived>::_register = registerFuntion<Derived>();

#endif
