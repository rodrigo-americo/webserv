#ifndef UPLOAD_MULTIPART_FORM_DATA_ITERATOR_HPP
#define UPLOAD_MULTIPART_FORM_DATA_ITERATOR_HPP

# include "UploadMultipartFormDataPart.hpp"

class Router;

class UploadMultipartFormDataIterator
{
	private:
		const Router				*_router;
		size_t						_cursor;
		utils::str					_boundary;
		utils::str					_delimitter;
		utils::str					_end_marker;
		UploadMultipartFormDataPart	_current_part;

		utils::str	_getBoundary() const;
		void	_nextDelimiter();

	public:
		UploadMultipartFormDataIterator(const Router *router);
		~UploadMultipartFormDataIterator();

		operator bool();
		UploadMultipartFormDataIterator	&operator++();
		const UploadMultipartFormDataPart		&operator*() const;
		const UploadMultipartFormDataPart		*operator->() const;

};

#endif
