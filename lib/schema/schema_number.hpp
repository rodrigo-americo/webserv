/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_number.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:40:55 by ighannam          #+#    #+#             */
/*   Updated: 2026/05/29 19:16:51 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_NUMBER_HPP
#define SCHEMA_NUMBER_HPP

#include "schema_base.hpp"
#include "utils.hpp"

namespace schema
{
    namespace detail
    {
        template <typename T, typename TDerived>
        class schema_number : public schema_base<T, TDerived>
        {
            protected:
                typedef schema_base<T, TDerived> base;
            private:
                TDerived& derived() { return static_cast<TDerived&>(*this); };
                const TDerived& derived() const { return static_cast<const TDerived&>(*this); };
            public:
                schema_number() {};
                ~schema_number() {};
                TDerived& min(T value, std::string msg = "")
                {
                    return derived()._min(value, msg);
                }
                result<T> parse(std::string entry)
                {
                    std::stringstream ss;
                    ss << entry;
                    T value;
                    ss >> value;
                    if (ss.fail() || ss.peek() != std::stringstream::traits_type::eof())
                        return result<T>::failure(error(this->field_name, "Entry is " + entry + ", but it must be a number"));
                    return this->runAllValidators(value);
                }
        };

        template <typename T>
        class schema_int : public schema_number<T, schema_int<T> >
        {
            private:
                typedef schema_number<T, schema_int<T> > base;
                friend class schema_number<T, schema_int<T> >;
                schema_int& _min(T min, std::string& msg)
                {
                    struct functor_validator_min : public base::base::functor_validator
                    {
                        T min_value;
                        std::string _msg;
                        functor_validator_min(T _min, std::string& msg): min_value(_min), _msg(msg) {};
                        std::string operator()(T value)
                        {
                            if (value < min_value)
                                return _msg.empty() ? "Minimal value not reached. Is " + utils::to_string(value) + ". Must be at least " + utils::to_string(min_value) : _msg;
                            return "";
                        }
                    };
                    this->addValidator(new functor_validator_min(min, msg));
                    return *this;
                }
        };


        template <typename T>
        class schema_double : public schema_number<T, schema_double<T> >
        {
            private:
                typedef schema_number<T, schema_double<T> > base;
                friend class schema_number<T, schema_double<T> >;
                double _eps;
                schema_double& _min(T min, std::string& msg)
                {
                    struct functor_validator_min : public base::base::functor_validator
                    {
                        T min_value;
                        std::string _msg;
                        double eps;
                        functor_validator_min(T _min, std::string& msg, double _eps): min_value(_min), _msg(msg), eps(_eps) {};
                        std::string operator()(T value)
                        {
                            if (value - min_value >= eps)
                                return _msg.empty() ? "Minimal value not reached. Is " + utils::to_string(value) + ". Must be at least " + utils::to_string(min_value) : _msg;
                            return "";
                        }
                    };
                    this->addValidator(new functor_validator_min(min, msg, _eps));
                    return *this;
                }
            public:
                schema_double() : _eps(1e-9) {};
                
                schema_double& eps(double eps_) { _eps = eps_; return *this; };
        };
    }
    

    
    
}



#endif