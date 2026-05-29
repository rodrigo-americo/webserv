/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_base.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:11:32 by brunofer          #+#    #+#             */
/*   Updated: 2026/05/29 17:18:44 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SCHEMA_BASE_HPP
# define SCHEMA_BASE_HPP

#include <string>
#include <vector>
#include <sstream>

namespace schema
{		
	namespace detail
	{

		struct error
		{
			std::string title;
			std::string msg;
			error(std::string _title, std::string _msg) : title(_title), msg(_msg) {};
			std::string format() const
			{
				return title.empty() ? msg : "[" + title + "] " + msg;
			}
		};

		template <typename T>
		struct result
		{
			bool ok;
			T value;
			std::vector<error> errors;
			result(T _value) : ok(true), value(_value), errors() {};
			result(std::vector<error> _errors) : ok(false), value(), errors(_errors) {};
			static result success(T value) { return result(value); };
			static result failure(std::vector<error> _errors) { return result(_errors); };
			static result failure(error _error) 
			{ 
				std::vector<error> err(1,_error);
				return result(err);
			};
			std::string format()
			{
				std::stringstream ss;
				for (size_t i = 0; i < errors.size(); i++)
				{
					ss << errors[i].format() << "\n";
				}
				return ss.str();
			}
			operator bool() { return ok; };
		};

		template <typename T, typename Derived>
		class schema_base
		{
			public:
				struct functor_validator
				{
					virtual std::string operator()(T value) = 0;
					virtual ~functor_validator() {};
				};
				struct functor_refine
				{
					virtual bool operator()(T value, std::string msg) = 0;
					virtual ~functor_refine() {};
				};
			protected:
				std::string field_name;
				std::vector<functor_validator*> validators;
				void addValidator(functor_validator* validator)
				{
					if (!validator)
						return;
					validators.push_back(validator);
				};
				result<T> runAllValidators(T value)
				{
					std::vector<error> errors;
					for (size_t i = 0; i < validators.size(); i++)
					{
						std::string msg = (*validators[i])(value);
						delete validators[i];
						if (!msg.empty())
							errors.push_back(error(field_name, msg));		
					}
					if (errors.empty())
						return result<T>::success(value);
					return result<T>::failure(errors);			
				}
				
			public:
				Derived& name(std::string _name) { field_name = _name; return static_cast<Derived&>(*this); };
				Derived& refine(functor_refine* refine_validator, std::string msg)
				{
					struct functor_validator_refine: public functor_validator
					{
						std::string _msg;
						functor_validator_refine(std::string& message) : _msg(message) {};
						std::string operator()(T value)
						{
							bool item = (*refine_validator)(value, _msg);
							delete refine_validator;
							return item ? "" : _msg;
						}
					};
					addValidator(new functor_validator_refine(msg));
					return static_cast<Derived&>(*this);
				}
		};


		
	}
}




#endif
