/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_base.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:11:32 by brunofer          #+#    #+#             */
/*   Updated: 2026/06/14 18:41:41 by ighannam         ###   ########.fr       */
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
			bool operator==(const error& other) const
			{
				if (this->title == other.title && this->msg == other.msg)
					return true;
				return false;
			}
			std::string format() const
			{
				return title.empty() ? msg : "[" + title + "] " + msg;
			}
			friend std::ostream& operator<<(std::ostream& os, const error& e)
            {
                os << e.format();
                return os;
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
			bool operator==(const result& other) const
			{
				if (this->value == other.value && this->ok == other.ok && this->errors == other.errors)
					return true;
				return false;
			}
			bool operator!=(const result& other) const
			{
				if (*this == other)
					return false;
				return true;
			}
			friend std::ostream& operator<<(std::ostream& os, const result<T>& r)
            {
                if (r.ok)
                    os << "ok: " << r.value;
                else
                {
                    for (size_t i = 0; i < r.errors.size(); i++)
                        os << r.errors[i];
                }
                return os;
            }
		};

		template <typename T, typename Derived>
		class schema_base
		{
			public:
				struct functor_validator
				{
					virtual std::string operator()(const T& value) = 0;
					virtual functor_validator* clone() const = 0;
					virtual ~functor_validator() { };
				};
				struct functor_refine
				{
					virtual bool operator()(const T& value) = 0;
					virtual functor_refine* clone() const = 0;
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
						functor_refine* _refine_validator;
						functor_validator_refine(const std::string& message, functor_refine* refine_validator) : _msg(message), _refine_validator(refine_validator) {};
						~functor_validator_refine() { delete _refine_validator; }
						functor_validator* clone() const
						{
							return new functor_validator_refine(_msg, _refine_validator->clone());
						};
						std::string operator()(const T& value)
						{
							bool item = (*_refine_validator)(value);
							return item ? "" : _msg;
						}
					};
					addValidator(new functor_validator_refine(msg, refine_validator));
					return static_cast<Derived&>(*this);
				}
				schema_base() {};
				schema_base(const schema_base& other)
				{
					if (this != &other)
					{
						for (size_t i = 0; i < other.validators.size(); i++)
							this->validators.push_back(other.validators[i]->clone());
					}
				}
				schema_base& operator=(const schema_base& other)
				{
					if (this != &other)
					{
						for (size_t i = 0; i < validators.size(); i++)
							delete validators[i];
						validators.clear();
						for (size_t i = 0; i < other.validators.size(); i++)
							validators.push_back(other.validators[i]->clone());
					}
					return *this;
				}
				virtual ~schema_base()
				{
					for (size_t i = 0; i < validators.size(); i++)
					{
						delete validators[i];
					}
					validators.clear();					
				}
		};
	}
}




#endif
