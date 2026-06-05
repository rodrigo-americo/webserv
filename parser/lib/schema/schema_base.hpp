/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_base.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:11:40 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/16 16:11:46 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_BASE_HPP
# define SCHEMA_BASE_HPP

# include <string>
# include <vector>
# include <iostream>
# include <fstream>
# include <functional>

# include "text.hpp"
# include "segregation.hpp"

namespace schema
{
	namespace detail
	{
		static const text::style	error_msg_style = text::bold().italic();
	}

	template <typename T>
	struct result
	{
		bool			ok;
		T				value;
		segregation::errors_vector	errors;

		static result	success(T _value) {return {true, _value, {}};};
		static result	failure(segregation::errors_vector _errors) {return {false, T{}, std::move(_errors)};};
		static result	failure(segregation::error _error) {return failure(segregation::errors_vector{std::move(_error)});};
		explicit operator bool() const {return ok;}

		void dump(std::ostream &os)
		{
			std::string error_header = text::add(segregation::error_title_style, "error:").add(" ").build();
			for (const auto &error: errors)
				os << error_header << error.format() << "\n";
		}

		void dump(const std::string &out_file)
		{
			std::ofstream	file(out_file, std::ios::trunc);
			if (!file.is_open())
			{
				std::cerr << "Cannot dump erros in file '" << out_file << "'\n";
				return;
			}
			std::string error_header = text::add(segregation::error_title_style, "error:").add(" ").build();
			for (const auto &error: errors)
				file << error_header << error.format() << "\n";
			file.close();
		}

		void dump()
		{
			dump(std::cerr);
		}
	};

	namespace detail
	{
		template <typename Derived, typename T>
		class schema_base
		{
			protected:
				using					Validator = std::function<std::string(const T&)>;
				std::string				_field_name;
				std::vector<Validator>	_validators;

			private:
				Derived	&derived() {return static_cast<Derived&>(*this);};
				const Derived	&derived() const {return static_cast<const Derived&>(*this);};

			protected:
				void	_addValidator(Validator validator) { _validators.push_back(validator); };

				segregation::errors_vector	_runAllValidators(const T &value) const
				{
					segregation::errors_vector errs;
					for (const Validator &callback : _validators)
					{
						std::string msg = callback(value);
						if (!msg.empty())
							errs.push_back({_field_name, msg});
					}
					return errs;
				}

				result<T>	_resultFor(T value) const
				{
					segregation::errors_vector errs = _runAllValidators(value);
					return errs.empty() ? result<T>::success(std::move(value))
										: result<T>::failure(std::move(errs));
				}

			public:
				Derived	&name(std::string name)
				{
					_field_name = std::move(name);
					return derived();
				}

				Derived	&refine(std::function<bool(const T&)> callback, std::string msg)
				{
					_addValidator([callback, msg](const T &value) { return callback(value) ? "" : msg; });
					return derived();
				}

		};
	}
}

#endif
