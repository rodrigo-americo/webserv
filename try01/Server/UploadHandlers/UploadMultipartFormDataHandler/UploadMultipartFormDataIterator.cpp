#include "UploadMultipartFormDataIterator.hpp"
#include "Router.hpp"

utils::str	UploadMultipartFormDataIterator::_getBoundary() const
{
	const std::string	&content_type = _router->req.headers.content_type();
	size_t pos = content_type.find("boundary=");
	if (pos == std::string::npos)
		return "";
	return "--" + content_type.substr(pos + 9);
};

void	UploadMultipartFormDataIterator::_nextDelimiter()
{
	_cursor	= _router->req.body.find(_delimitter.string(), _cursor);
	if (_cursor == utils::str::npos)
		return;
	_cursor += _delimitter.size();
	size_t part_end = _router->req.body.find(_delimitter.string(), _cursor);
	if (part_end == utils::str::npos)
		part_end = _router->req.body.find(_end_marker.string(), _cursor);
	_current_part = UploadMultipartFormDataPart(_router, _cursor, part_end - _cursor);
}

UploadMultipartFormDataIterator::UploadMultipartFormDataIterator(const Router *router)
	: _router(router), _cursor(utils::str::npos), _boundary(), _delimitter(), _end_marker(), _current_part(router, 0, 0)
{
	if (!_router) return;
	_boundary = _getBoundary();
	_delimitter = _boundary + "\r\n";
	_end_marker = _boundary + "--";
	_cursor = 0;
	_nextDelimiter();
};
UploadMultipartFormDataIterator::~UploadMultipartFormDataIterator() {};

UploadMultipartFormDataIterator::operator bool() { return _cursor != utils::str::npos; }

UploadMultipartFormDataIterator	&UploadMultipartFormDataIterator::operator++()
{
	_nextDelimiter();
	return *this;
}

const UploadMultipartFormDataPart		&UploadMultipartFormDataIterator::operator*() const
{
	return _current_part;
}

const UploadMultipartFormDataPart		*UploadMultipartFormDataIterator::operator->() const
{
	return &_current_part;
}
