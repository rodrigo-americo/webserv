/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_number.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:40:55 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/17 18:41:06 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_NUMBER_HPP
#define SCHEMA_NUMBER_HPP

# include <map>
# include <sstream>
# include <string>
# include <iostream>

#include "schema_base.hpp"
#include "utils.hpp"

namespace schema
{
	namespace detail
	{
		template <typename Derived>
		class schema_number_sufix
		{
			private:
				Derived&		derived() { return static_cast<Derived&>(*this); };
				const Derived&	derived() const { return static_cast<const Derived&>(*this); };
			protected:
				typedef std::map<std::string, double > Sufixes;
				Sufixes		_sufixes;
				std::string	_sufixes_str;
				std::string	_custom_message;
				bool		_sufix_optional;
				bool		_ignore_case;
				double		_default_multiplier;

				void	_transformCaseInsensitiveSufixes()
				{
					Sufixes	copy = _sufixes;
					_sufixes.clear();
					for (Sufixes::iterator it = copy.begin(); it != copy.end(); ++it)
					{
						std::string	key = it->first;
						_toLower(key);
						_sufixes[key] = it->second;
					}
				}

				void	_toLower(std::string &str)
				{
					for (size_t i = 0; i < str.size(); i++)
						str[i] = std::tolower(str[i]);
				}

			public:
				schema_number_sufix()
					: _sufixes(), _sufixes_str(), _custom_message(),
					_sufix_optional(false), _ignore_case(false), _default_multiplier(1) {}
				~schema_number_sufix() {}

				Derived	&sufix(const std::string &sufix, double multiplier)
				{
					_sufixes[sufix] = multiplier;
					if (!_sufixes_str.empty())
						_sufixes_str += ", " + sufix;
					else
						_sufixes_str += sufix;
					return derived();
				}

				Derived	&sufixOptional(double default_multiplier = 1) { _sufix_optional = true; _default_multiplier = default_multiplier; return derived(); }
				Derived	&caseInsensitive() { _ignore_case = true; return derived(); }

				Derived	&message(const std::string &msg) { _custom_message = msg; return derived(); }
		};

		template <typename T, typename TDerived>
		class schema_number : public schema_base<T, TDerived>, public schema_number_sufix<TDerived>
		{
			protected:
				typedef schema_base<T, TDerived> base;
				typedef schema_number_sufix<TDerived> sufix_base;
			private:
				typedef typename schema_number_sufix<TDerived>::Sufixes Sufixes;

				TDerived& derived() { return static_cast<TDerived&>(*this); };
				const TDerived& derived() const { return static_cast<const TDerived&>(*this); };

			public:
				schema_number(): base(), sufix_base() {};
				schema_number(const schema_number& other) : base(other), sufix_base(other) {}
				schema_number& operator=(const schema_number& other) {
					if (this != &other) {
						base::operator=(other);
						sufix_base::operator=(other);
					}
					return *this;
				}
				~schema_number() {};

				result<T> parse(std::string entry)
				{
					std::stringstream ss;
					ss << entry;
					T value;
					ss >> value;


					if (ss.fail())
						return result<T>::failure(error(this->field_name, "ss.fail() Entry is " + entry + ", but it must be a number"));
					if (!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof())
						return result<T>::failure(error(this->field_name, "!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof() Entry is " + entry + ", but it must be a number"));
					if ((!this->_sufix_optional && this->_sufixes.empty())
						|| (this->_sufix_optional && ss.peek() == std::stringstream::traits_type::eof()))
						return this->runAllValidators(T(double(value) * this->_default_multiplier));

					std::string	sufix;
					ss >> sufix;
					if (this->_ignore_case)
					{
						this->_transformCaseInsensitiveSufixes();
						this->_toLower(sufix);
					}

					typename Sufixes::iterator	it = this->_sufixes.find(sufix);
					if (it == this->_sufixes.end())
					{
						if (!this->_custom_message.empty())
							return result<T>::failure(error(this->field_name, this->_custom_message));
						return result<T>::failure(error(this->field_name, "Sufix " + entry + " is invalid, must be one of '" + this->_sufixes_str + "'."));
					}
					value = T(double(value) * it->second);
					return this->runAllValidators(value);
				}

				TDerived& min(const T& value, const std::string& msg = "")
				{
					return derived()._min(value, msg);
				}
				TDerived& max(const T& value, const std::string& msg = "")
				{
					return derived()._max(value, msg);
				}
				TDerived& between(const T& value_min, const T& value_max, const std::string& msg = "")
				{
					return derived()._min(value_min, msg)._max(value_max, msg);
				}
		};

		template <typename T>
		class schema_int : public schema_number<T, schema_int<T> >
		{
			private:
				typedef schema_number<T, schema_int<T> > base;
				friend class schema_number<T, schema_int<T> >;
				schema_int& _min(const T& min, const std::string& msg)
				{
					struct functor_validator_min : public base::base::functor_validator
					{
						T min_value;
						std::string _msg;
						functor_validator_min(const T& _min, const std::string& msg): min_value(_min), _msg(msg) {};
						typename base::functor_validator* clone() const
						{
							return new functor_validator_min(*this);
						}
						std::string operator()(const T& value)
						{
							if (value < min_value)
								return _msg.empty() ? "Minimal value not reached. Is " + utils::to_string(value) + ". Must be at least " + utils::to_string(min_value) : _msg;
							return "";
						}
					};
					this->addValidator(new functor_validator_min(min, msg));
					return *this;
				}
				schema_int& _max(const T& max, const std::string& msg)
				{
					struct functor_validator_max : public base::base::functor_validator
					{
						T max_value;
						std::string _msg;
						functor_validator_max(const T& _max, const std::string& msg): max_value(_max), _msg(msg) {};
						typename base::functor_validator* clone() const
						{
							return new functor_validator_max(*this);
						}
						std::string operator()(const T& value)
						{
							if (value > max_value)
								return _msg.empty() ? "Maximal value overreached. Is " + utils::to_string(value) + ". Must be at max " + utils::to_string(max_value) : _msg;
							return "";
						}
					};
					this->addValidator(new functor_validator_max(max, msg));
					return *this;
				}
				schema_int& _oneOf(const std::vector<T>& options, const std::string& msg)
				{
					struct functor_validator_oneOf : public base::base::functor_validator
					{
						std::vector<T> _options;
						std::string _msg;
						functor_validator_oneOf(const std::vector<T>& options, const std::string& msg): _options(options), _msg(msg) {};
						typename base::functor_validator* clone() const
						{
							return new functor_validator_oneOf(*this);
						}
						std::string operator()(const T& value)
						{
							if (!utils::oneOf(_options, value))
								return _msg.empty() ? "Value " + utils::to_string(value) + " is not one of the given options." : _msg;
							return "";
						}
					};
					this->addValidator(new functor_validator_oneOf(options, msg));
					return *this;
				}
			public:
				schema_int& oneOf(const std::vector<T>& options, const std::string& msg = "")
				{
					return _oneOf(options, msg);
				}
		};


		template <typename T>
		class schema_double : public schema_number<T, schema_double<T> >
		{
			private:
				typedef schema_number<T, schema_double<T> > base;
				friend class schema_number<T, schema_double<T> >;
				double _eps;
				schema_double& _min(const T& min, const std::string& msg)
				{
					struct functor_validator_min : public base::base::functor_validator
					{
						T min_value;
						std::string _msg;
						double eps;
						functor_validator_min(const T& _min, const std::string& msg, double _eps): min_value(_min), _msg(msg), eps(_eps) {};
						typename base::functor_validator* clone() const
						{
							return new functor_validator_min(*this);
						}
						std::string operator()(const T& value)
						{
							if (value - min_value < -eps)
								return _msg.empty() ? "Minimal value not reached. Is " + utils::to_string(value) + ". Must be at least " + utils::to_string(min_value) : _msg;
							return "";
						}
					};
					this->addValidator(new functor_validator_min(min, msg, _eps));
					return *this;
				}
				schema_double& _max(const T& max, const std::string& msg)
				{
					struct functor_validator_max : public base::base::functor_validator
					{
						T max_value;
						std::string _msg;
						double eps;
						functor_validator_max(const T& _max, const std::string& msg, double _eps): max_value(_max), _msg(msg), eps(_eps) {};
						typename base::functor_validator* clone() const
						{
							return new functor_validator_max(*this);
						}
						std::string operator()(const T& value)
						{
							if (value - max_value >= eps)
								return _msg.empty() ? "Maximal value overreached. Is " + utils::to_string(value) + ". Must be at max " + utils::to_string(max_value) : _msg;
							return "";
						}
					};
					this->addValidator(new functor_validator_max(max, msg, _eps));
					return *this;
				}
			public:
				schema_double() : _eps(1e-9) {};
				schema_double(const schema_double& other) : base(other), _eps(other._eps) {}
				schema_double& operator=(const schema_double& other) {
					if (this != &other) {
						base::operator=(other);
						_eps = other._eps;
					}
					return *this;
				}

				schema_double& eps(double eps_) { _eps = eps_; return *this; };
		};
	}
}

#endif
