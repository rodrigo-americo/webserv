/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_string.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:41:49 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 16:43:48 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATORSTRINGSCHEMA_HPP
# define VALIDATORSTRINGSCHEMA_HPP

# include <string>
# include <functional>
# include <algorithm>

# include "schema_base.hpp"

namespace schema
{
	namespace detail
	{
		class string_base: public schema_base<string_base, std::string>
		{
			private:

				static bool	_verifyBytes(const std::string &value, std::function<bool(unsigned char c)> callback)
				{
					bool	error = false;
					for (const char c : value)
					{
						if (callback(static_cast<unsigned char>(c)))
						{
							error = true;
							break;
						}
					}
					return error;
				}

			public:
				string_base() = default;
				string_base(const string_base &other) = default;
				string_base(string_base &&other) = default;
				string_base	&operator=(const string_base &other) = default;
				string_base	&operator=(string_base &&other) = default;

				result<std::string> parse(const std::string& raw) const { return this->_resultFor(raw); }

				string_base	&minLength(size_t len, std::string msg = "")
				{
					this->_addValidator([len, msg](const std::string &value) -> std::string {
						if (value.size() < len)
							return msg.empty() ? text::add(error_msg_style, "Must have at least " + std::to_string(len) + " chars!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&maxLength(size_t len, std::string msg = "")
				{
					this->_addValidator([len, msg](const std::string &value) -> std::string {
						if (value.size() > len)
							return msg.empty() ? text::add(error_msg_style, "Must have at most " + std::to_string(len) + " chars!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&lengthBetween(size_t min, size_t max, std::string msg = "")
				{
					return minLength(min, msg).maxLength(max, msg);
				}

				string_base	&nonEmpty(std::string msg = "")
				{
					return minLength(1, std::move(msg));
				}

				string_base	&noSpaces(std::string msg = "")
				{
					this->_addValidator([msg](const std::string &value) -> std::string {
						if (value.find(' ' == std::string::npos))
							return msg.empty() ? text::add(error_msg_style, "Cannot contain spaces!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&noWhiteSpaces(std::string msg = "")
				{
					this->_addValidator([msg](const std::string &value) -> std::string {
						if (_verifyBytes(value, ::isspace))
							return msg.empty() ? text::add(error_msg_style, "Cannot contain white spaces!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&alphanumeric(std::string msg = "")
				{
					this->_addValidator([msg](const std::string &value) -> std::string {
						if (_verifyBytes(value, ::isalnum))
							return msg.empty() ? text::add(error_msg_style, "Must be alpha numeric!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&numeric(std::string msg = "")
				{
					this->_addValidator([msg](const std::string &value) -> std::string {
						if (_verifyBytes(value, ::isdigit))
							return msg.empty() ? text::add(error_msg_style, "Must be numeric!").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&oneOf(std::vector<std::string> opts, std::string msg = "")
				{
					this->_addValidator([opts, msg](const std::string &value) -> std::string {
						for (const auto &opt : opts)
							if (value == opt) return "";

						std::string joined_opts;
						for (size_t i = 0; i < opts.size(); i++)
						{
							joined_opts += "'" + opts[i] + "'";
							if (i < opts.size() - 1)
								joined_opts += ", ";
						}
						return msg.empty() ? text::add(error_msg_style, "Must be one of " + joined_opts + "!").build() : msg;
					});
					return *this;
				}

				string_base	&startsWith(std::string prefix, std::string msg = "")
				{
					this->_addValidator([prefix, msg](const std::string &value) -> std::string {
						if (value.size() < prefix.size() || value.substr(0, prefix.size()) != prefix)
							return msg.empty() ? text::add(error_msg_style, "Must start with '" + prefix + "'").build() : msg;
						return "";
					});
					return *this;
				}

				string_base	&endsWith(std::string sufix, std::string msg = "")
				{
					this->_addValidator([sufix, msg](const std::string &value) -> std::string {
						if (value.size() < sufix.size()
							|| value.substr(value.size() - sufix.size(), value.size()) != sufix)
							return msg.empty() ? text::add(error_msg_style, "Must end with '" + sufix + "'").build() : msg;
						return "";
					});
					return *this;
				}
		};
	}
}

#endif
