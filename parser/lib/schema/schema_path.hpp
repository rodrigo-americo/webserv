/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_path.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 21:59:54 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 12:52:41 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_PATH_HPP
# define SCHEMA_PATH_HPP

# include <string>
# include <functional>
# include <unistd.h>
# include <sys/stat.h>

# include "schema_string.hpp"

namespace schema
{
	namespace detail
	{
		class path_base: protected string_base
		{
			using base = string_base;
		private:
			std::vector<base::Validator>	_validators_for_existing;
			bool							_push_validator_for_exising;
			std::string						_existing_message;
			struct stat						_st;
		public:
			path_base() {};
			~path_base() {};

			result<std::string>	parse(const std::string &raw)
			{
				if (_push_validator_for_exising || !_validators_for_existing.empty())
					this->_addValidator([this](const std::string &value) -> std::string {

						if (stat(value.c_str(), &_st) != 0)
							return _existing_message.empty() ? "path " + value + " dont exists." : _existing_message;
						for (size_t i = 0; i < _validators_for_existing.size(); i++)
							this->_addValidator(_validators_for_existing[i]);
					});
				return this->_resultFor(raw);
			}

			path_base	&existing(const std::string msg = "")
			{
				_push_validator_for_exising = true;
				_existing_message = msg;
				return *this;
			}

			path_base	&directory(const std::string msg = "")
			{
				_validators_for_existing.push_back([this, &msg](const std::string &value) -> std::string {
					if (!S_ISDIR(_st.st_mode))
						return msg.empty() ? "path " + value + " is not a directory." : msg;
					return "";
				});
				return *this;
			}

			path_base	&file(const std::string msg = "")
			{
				_validators_for_existing.push_back([this, &msg](const std::string &value) -> std::string {
					if (!S_ISREG(_st.st_mode))
						return msg.empty() ? "path " + value + " is not a file." : msg;
					return "";
				});
				return *this;
			}

			path_base	&readable(const std::string msg = "")
			{
				_validators_for_existing.push_back([&msg](const std::string &value) -> std::string {
					if (access(value.c_str(), R_OK))
						return msg.empty() ? "path " + value + " is not readable." : msg;
					return "";
				});
				return *this;
			}

			path_base	&writable(const std::string msg = "")
			{
				_validators_for_existing.push_back([&msg](const std::string &value) -> std::string {
					if (access(value.c_str(), W_OK))
						return msg.empty() ? "path " + value + " is not writable." : msg;
					return "";
				});
				return *this;
			}

			path_base	&executable(const std::string msg = "")
			{
				_validators_for_existing.push_back([&msg](const std::string &value) -> std::string {
					if (access(value.c_str(), X_OK))
						return msg.empty() ? "path " + value + " is not executable." : msg;
					return "";
				});
				return *this;
			}

			path_base	&absolute(const std::string msg = "")
			{
				this->_addValidator([&msg](const std::string &value) -> std::string {
					if (value[0] != '/')
						return msg.empty() ? "path " + value + " is not absolute." : msg;
					return "";
				});
				return *this;
			}

			path_base	&relative(const std::string msg = "")
			{
				this->_addValidator([&msg](const std::string &value) -> std::string {
					if (value[0] == '/')
						return msg.empty() ? "path " + value + " is not relative." : msg;
					return "";
				});
				return *this;
			}

			path_base	&extension(const std::string &file_extension ,const std::string msg = "")
			{
				this->endsWith(file_extension, msg.empty() ? "file extension must be '" + file_extension + "'" : msg);
				return *this;
			}
		};
	}
}

#endif
