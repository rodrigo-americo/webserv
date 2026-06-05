/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   number_base.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:23:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/18 17:28:38 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATORNUMBERSCHEMA_HPP
# define VALIDATORNUMBERSCHEMA_HPP

# include <cmath>
# include <sstream>

# include "schema_base.hpp"

namespace schema
{
	namespace detail
	{
		template <typename Derived, typename T>
		class number_base: public schema_base<Derived, T>
		{

			protected:
				static std::string	_numberStr(T number)
				{
					std::ostringstream	oss;
					oss << number;
					return oss.str();
				}

			private:
				Derived	&derived() {return static_cast<Derived &>(*this);};
				const Derived	&derived() const {return static_cast<const Derived &>(*this);};

			public:
				number_base() = default;
				number_base(const number_base &other) = default;
				number_base(number_base &&other) = default;
				number_base	&operator=(const number_base &other) = default;
				number_base	&operator=(number_base &&other) = default;

				result<T>	parse(const std::string &raw) const
				{
					if (raw.empty())
						return result<T>::failure({this->_field_name, text::add(error_msg_style, "Field is empty!").build()});

					T	value{};
					std::istringstream	iss(raw);
					iss >> value;

					if (iss.fail() || iss.peek() != std::istringstream::traits_type::eof())
						return result<T>::failure({this->_field_name, text::add(error_msg_style, "'" + raw + "' Cannot be converted to a number!").build()});

					return this->_resultFor(value);
				}

				Derived	&min(T value, std::string msg = "") { return derived()._min(value, std::move(msg)); };
				Derived	&max(T value, std::string msg = "") { return derived()._max(value, std::move(msg)); };
				Derived	&between(T min_value, T max_value) { return this->min(min_value).max(max_value); };

				Derived	&positive(std::string msg = text::add(error_msg_style, "Must be positive (>= 0)").build()) { return derived()._positive(std::move(msg)); };
				Derived	&negative(std::string msg = text::add(error_msg_style, "Must be negative (< 0)").build()) { return derived()._negative(std::move(msg)); };
		};

		template <typename T>
		class integer_base: public number_base<integer_base<T>, T>
		{
			friend class number_base<integer_base<T>, T>;

			integer_base<T>	&_min(T val, std::string msg = "")
			{
				this->_addValidator([this, val, msg](const T &value) -> std::string {
					if (value < val)
						return msg.empty() ? text::add(error_msg_style, "Must be >= " + this->_numberStr(val)).build() : msg;
					return "";
				});
				return *this;
			};

			integer_base<T>	&_max(T val, std::string msg = "")
			{
				this->_addValidator([this, val, msg](const T &value) -> std::string {
					if (value > val)
						return msg.empty() ? text::add(error_msg_style, "Must be <= " + this->_numberStr(val)).build() : msg;
					return "";
				});
				return *this;
			};

			integer_base<T>	&_positive(std::string msg)
			{
				this->_addValidator([msg](const T &value) -> std::string {
					return value >= T(0) ? "" : msg;
				});
				return *this;
			};

			integer_base<T>	&_negative(std::string msg)
			{
				this->_addValidator([msg](const T &value) -> std::string {
					return value < T(0) ? "" : msg;
				});
				return *this;
			};

			public:
				integer_base() = default;
				integer_base(const integer_base &other) = default;
				integer_base(integer_base &&other) = default;
				integer_base	&operator=(const integer_base &other) = default;
				integer_base	&operator=(integer_base &&other) = default;

			integer_base<T>	&multipleOf(T val, std::string msg)
			{
				this->_addValidator([this, val, msg](const T &value) -> std::string {
					if (value % val != 0)
						return msg.empty() ? text::add(error_msg_style, "Must be multiple of " + this->_numberStr(val)).build() : msg;
					return "";
				});
				return *this;
			};

			integer_base<T>	&even(std::string msg)
			{
				this->_addValidator([msg](const T &value) -> std::string {
					if (value % T(2) != 0)
						return msg.empty() ? text::add(error_msg_style, "Must be even!").build() : msg;
					return "";
				});
				return *this;
			};

			integer_base<T>	&odd(std::string msg)
			{
				this->_addValidator([msg](const T &value) -> std::string {
					if (value % T(2) == 0)
						return msg.empty() ? text::add(error_msg_style, "Must be odd!").build() : msg;
					return "";
				});
				return *this;
			};
		};

		template <typename T>
		class float_base: public number_base<float_base<T>, T>
		{
			friend class number_base<float_base<T>, T>;

			private:
				double	_eps;

				float_base<T>	&_min(T val, std::string msg = "")
				{
					this->_addValidator([this, val, msg](const T &value) -> std::string {
						if (value - val < -_eps)
							return msg.empty() ? text::add(error_msg_style, "Must be >= " + this->_numberStr(val)).build() : msg;
						return "";
					});
					return *this;
				};

				float_base<T>	&_max(T val, std::string msg = "")
				{
					this->_addValidator([this, val, msg](const T &value) -> std::string {
						if (val - value < -_eps)
							return msg.empty() ? text::add(error_msg_style, "Must be <= " + this->_numberStr(val)).build() : msg;
						return "";
					});
					return *this;
				};

				float_base<T>	&_positive(std::string msg)
				{
					this->_addValidator([this, msg](const T &value) -> std::string {
						return value >= T(_eps) ? "" : msg;
					});
					return *this;
				};

				float_base<T>	&_negative(std::string msg)
				{
					this->_addValidator([this, msg](const T &value) -> std::string {
						return value < T(-_eps) ? "" : msg;
					});
					return *this;
				};

			public:
				float_base(double eps = 1e-9): _eps(eps) {};
				float_base(const float_base &other) = default;
				float_base(float_base &&other) = default;
				float_base	&operator=(const float_base &other) = default;
				float_base	&operator=(float_base &&other) = default;

				float_base<T>	&eps(double eps)
				{
					_eps = eps;
					return *this;
				}
		};
	}
}

#endif
