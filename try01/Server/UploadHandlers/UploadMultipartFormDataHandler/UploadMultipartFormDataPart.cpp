#include "UploadMultipartFormDataPart.hpp"
#include "Router.hpp"

bool	UploadMultipartFormDataPart::idxError(size_t idx, size_t part_end)
{
	if (idx == utils::str::npos || idx > part_end)
	{
		_error = true;
		return true;
	}
	return false;
}

std::pair<utils::str, utils::str>	UploadMultipartFormDataPart::_getHeaderLine(size_t begin, size_t &line_end)
{
	LOG_INFO("_getHeaderLine::_error: " << _error);
	if (_error)
		return std::make_pair("", "");
	size_t	line_start = _header.find_first_not_of(" \r\n", begin);
	if (line_start == utils::str::npos)
	{
		line_end = utils::str::npos;
		return std::make_pair("", "");;
	}
	line_end = _header.find("\r\n", line_start);
	LOG_INFO("_getHeaderLine::line_end: " << line_end);
	LOG_INFO("_getHeaderLine::line_start: " << line_start);
	if (line_end == utils::str::npos)
		_error = true;
	if (_error)
		return std::make_pair("", "");
	utils::str	line = _header.substr(line_start, line_end - line_start);
	LOG_INFO("_getHeaderLine: " << line);
	size_t	colon_idx = line.find(":");
	if (colon_idx == utils::str::npos)
	{
		_error = true;
		return std::make_pair("", "");
	}
	std::pair<utils::str, utils::str>	result = std::make_pair(line.substr(0, colon_idx), line.substr(colon_idx + 1));
	if (result.first.trim().empty() || result.second.trim().empty())
		_error = true;
	return result;
}

utils::str	UploadMultipartFormDataPart::_parseKeyValueParam(const utils::str &key, const utils::str &data)
{
	size_t	key_idx = data.find(key);
	size_t	equal_idx = data.find("=", key_idx);
	if (key_idx != utils::str::npos)
	{
		if (equal_idx == utils::str::npos) { _error = true; return ""; }
		utils::str	value = data.substr(equal_idx + 1, data.find(";", equal_idx) - equal_idx - 1);
		// remove quotes "
		return value.trim().substr(1, value.size() - 2);
	}
	return "";
}

void	UploadMultipartFormDataPart::_parseContentDisposition(const utils::str &data)
{
	size_t	semicolon_idx = data.find(";");
	if (semicolon_idx == utils::str::npos)
	{
		LOG_INFO("_parseContentDisposition::disposition_type: " << data);
		if (data != "form-data")
		{
			_error = true;
			return;
		}
		_disposition_type = "form-data";
		return;
	}
	utils::str disposition_type = data.substr(0, semicolon_idx).trim();
	LOG_INFO("_parseContentDisposition::disposition_type: " << disposition_type);
	if (disposition_type != "form-data")
	{
		_error = true;
		return;
	}
	_disposition_type = "form-data";
	_disposition_name = _parseKeyValueParam("name", data);
	_disposition_filename = _parseKeyValueParam("filename", data);
	LOG_INFO("_parseContentDisposition::ok: true");
}

void	UploadMultipartFormDataPart::_parseContentType(const utils::str &data)
{
	LOG_INFO("_parseContentType...");
	LOG_INFO("_parseContentType::data.count('/'): " << data.count('/'));
	if (data.count('/') != 1)
	{
		_error = true;
		return;
	}
	_content_type = data;
}

void	UploadMultipartFormDataPart::_extractHeaderInfo()
{
	size_t	line_end = 0;
	std::pair<utils::str, utils::str>	line = _getHeaderLine(line_end, line_end);
	LOG_INFO("_extractHeaderInfo::line-> " << line.first << ": " << line.second);
	if (line_end == utils::str::npos)
	{
		_error = true;
		return;
	}
	while (line_end != utils::str::npos)
	{
		LOG_INFO("_extractHeaderInfo::line_end: " << line_end);
		if (line.first == "Content-Disposition")
			_parseContentDisposition(line.second);
		else if (line.first == "Content-Type")
			_parseContentType(line.second);
		if (_error)
		{
			LOG_ERROR("Header Error...");
			return;
		}
		LOG_INFO("_extractHeaderInfo::_getHeaderLine... ");
		line = _getHeaderLine(line_end, line_end);
		LOG_INFO("_extractHeaderInfo::line-> " << line.first << ": " << line.second);
	}
	LOG_INFO("_extractHeaderInfo::loop_exit::line_end: " << line_end);
	if (_disposition_type.empty())
		_error = true;
}

UploadMultipartFormDataPart::UploadMultipartFormDataPart(const Router *router, size_t part_begin, size_t part_size)
	: _router(router), _header(), _body(), _error(false),
	_disposition_type(), _disposition_name(), _disposition_filename(), _content_type()
{
	if (part_size == 0)
	{
		_error = true;
		return;
	}
	size_t	header_end = router->req.body.find("\r\n\r\n", part_begin);
	if (idxError(header_end, part_begin + part_size - 1))
	{
		LOG_ERROR("idxError: " << header_end);
		return;
	}

	_header = router->req.body.substr(part_begin, header_end - part_begin + 2);
	size_t	body_start = header_end + 4;
	_body = router->req.body.substr(body_start, part_size - (body_start - part_begin));
	LOG_INFO("Header and Body builder OK!");
	_extractHeaderInfo();
}
UploadMultipartFormDataPart::~UploadMultipartFormDataPart() {}

bool	UploadMultipartFormDataPart::error() const { return _error; }
const utils::str	&UploadMultipartFormDataPart::dispositionType() const { return _disposition_type; }
const utils::str	&UploadMultipartFormDataPart::dispositionName() const { return _disposition_name; }
const utils::str	&UploadMultipartFormDataPart::dispositionFilename() const { return _disposition_filename; }
const utils::str	&UploadMultipartFormDataPart::content_type() const { return _content_type; }
const utils::str	&UploadMultipartFormDataPart::body() const { return _body; }
