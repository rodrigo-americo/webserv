/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_time.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:41:58 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/03 20:02:44 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATORTIMESCHEMA_HPP
# define VALIDATORTIMESCHEMA_HPP

# include <string>

# include "schema_base.hpp"
# include "structure.hpp"

namespace schema
{
	namespace detail
	{
		class time_base: public schema_base<time_base, structure::time>
		{
			private:
				static constexpr double	eps = structure::time::eps;
				std::string				_format;

			public:
				time_base() = default;
				time_base(const time_base &other) = default;
				time_base(time_base &&other) = default;
				time_base	&operator=(const time_base &other) = default;
				time_base	&operator=(time_base &&other) = default;

				time_base	&minTotalHours(double val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if ((time_base.inHours() - val) < eps)
							return msg.empty() ? text::add(error_msg_style, "Total Hours must be >= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&maxTotalHours(double val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if ((val - time_base.inHours()) < eps)
							return msg.empty() ? text::add(error_msg_style, "Total Hours must be <= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&betweenTotalHours(double min_value, double max_value, std::string msg = "")
				{
					return minTotalHours(min_value, msg).maxTotalHours(max_value, msg);
				}

				time_base	&minHours(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getHours() < val)
							return msg.empty() ? text::add(error_msg_style, "Hours must be >= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&maxHours(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getHours() > val)
							return msg.empty() ? text::add(error_msg_style, "Hours must be <= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&betweenHours(size_t min_value, size_t max_value, std::string msg = "")
				{
					return minHours(min_value, msg).maxHours(max_value, msg);
				}

				time_base	&minMinutes(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getMinutes() < val)
							return msg.empty() ? text::add(error_msg_style, "Minutes must be >= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&maxMinutes(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getMinutes() > val)
							return msg.empty() ? text::add(error_msg_style, "Minutes must be <= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&betweenMinutes(size_t min_value, size_t max_value, std::string msg = "")
				{
					return minMinutes(min_value, msg).maxMinutes(max_value, msg);
				}

				time_base	&minSeconds(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getSeconds() < val)
							return msg.empty() ? text::add(error_msg_style, "Seconds must be >= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&maxSeconds(size_t val, std::string msg = "")
				{
					this->_addValidator([val, msg](const structure::time &time_base) -> std::string {
						if (time_base.getSeconds() > val)
							return msg.empty() ? text::add(error_msg_style, "Seconds must be <= " + std::to_string(val)).build() : msg;
						return "";
					});
					return *this;
				}

				time_base	&betweenSeconds(size_t min_value, size_t max_value, std::string msg = "")
				{
					return minSeconds(min_value, msg).maxSeconds(max_value, msg);
				}

				time_base	&setFormat(std::string format)
				{
					_format = std::move(format);
					return *this;
				};

				result<structure::time>	parse(const std::string &raw) const
				{
					std::string	format = _format.empty() ? "HHhMM" : _format;
					structure::time		time_base = structure::time::parse(raw, format);
					if (!time_base.getParseError().empty())
						return result<structure::time>::failure({this->_field_name, time_base.getParseError()});
					return this->_resultFor(time_base);
				}
		};
	}
}

#endif
