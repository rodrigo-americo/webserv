#ifndef UPLOAD_MULTIPART_FORM_DATA_PART_HPP
#define UPLOAD_MULTIPART_FORM_DATA_PART_HPP

# include <utility>

# include "str.hpp"

class Router;

class UploadMultipartFormDataPart
{
	private:
		const Router	*_router;
		utils::str		_header;
		utils::str		_body;
		bool			_error;
		utils::str		_disposition_type;
		utils::str		_disposition_name;
		utils::str		_disposition_filename;
		utils::str		_content_type;

		bool	idxError(size_t idx, size_t part_end);
		std::pair<utils::str, utils::str>	_getHeaderLine(size_t begin, size_t &line_end);
		utils::str	_parseKeyValueParam(const utils::str &key, const utils::str &data);
		void		_parseContentDisposition(const utils::str &data);
		void		_parseContentType(const utils::str &data);
		void		_extractHeaderInfo();

	public:
		UploadMultipartFormDataPart(const Router *router, size_t part_begin, size_t part_size);
		~UploadMultipartFormDataPart();

		bool	error() const;
		const utils::str	&dispositionType() const;
		const utils::str	&dispositionName() const;
		const utils::str	&dispositionFilename() const;
		const utils::str	&content_type() const;
		const utils::str	&body() const;
};

#endif
