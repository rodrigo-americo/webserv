/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_errors.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 11:57:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/24 14:44:14 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HAS_ERRORS_HPP
# define HAS_ERRORS_HPP

# include <string>

# include "text.hpp"

namespace segregation
{
	static const text::style	error_title_style = text::bold().color(231, 76, 60).italic().underline();

	struct error
	{
		std::string	field;
		std::string	message;

		error(): field(), message() {};
		error(std::string _field, std::string _msg): field(_field), message(_msg) {};
		~error() = default;

		std::string	format() const
		{
			text::builder	message_builder = text::add(error_title_style, "[" + field + "]").add(" ").add(message);
			return field.empty() ? message : message_builder.build();
		}
	};

	using errors_vector = std::vector<error>;

	class has_errors
	{
		protected:
			errors_vector	_errors;

			void	_collect(const errors_vector &errors, std::string field_name = "", std::string prefix = "")
			{
				for (const auto &error : errors)
					_collect(error, std::move(field_name), std::move(prefix));
			}

			void	_collect(const error &_error, std::string field_name = "", std::string prefix = "")
			{
				error	err = field_name.empty() ? _error : error(std::move(field_name), _error.format());
				err.message = (prefix + err.message);
				_errors.push_back(err);
			}

		public:
			bool						hasErrors() const { return !_errors.empty(); }
			const errors_vector			&getErrors() const { return _errors; };
			std::string					formatErrors() const
			{
				std::string	errors;
				for (const auto &err: _errors)
					errors += err.format() + "\n";
				return errors;
			}
	};
}

#endif
