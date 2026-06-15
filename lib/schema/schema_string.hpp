/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_string.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:21:49 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/14 18:22:30 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHENA_STRING_HPP
# define SCHENA_STRING_HPP

# include <string>

# include "schema_base.hpp"
# include "utils.hpp"


namespace schema
{
    namespace detail
    {
        class schema_string: public schema_base<std::string, schema_string>
        {
        private:
            typedef schema_base<std::string, schema_string> base;
            
        public:
            schema_string(): base() {};
            schema_string(const schema_string& other) : base(other) {}
            schema_string& operator=(const schema_string& other) {
                if (this != &other) {
                    base::operator=(other);
                }
                return *this;
            }
            ~schema_string() {};
            
            schema_string   &minLength(const size_t& len, const std::string& msg = "")
            {
                struct functor_validator_minLength: public base::functor_validator
                {
                    std::string _msg;
                    size_t _len;
                    functor_validator_minLength(const std::string& msg, const size_t& len): _msg(msg), _len(len) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_minLength(*this);
                    }
                    std::string operator()(const std::string& value)
                    {
                        if (value.size() < _len)
                            return _msg.empty() ? "Minimal lenght not reached. Must be >= " + utils::to_string(_len) : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_minLength(msg, len));
                return *this;
            }
            schema_string   &maxLength(const size_t& len, const std::string& msg = "")
            {
                struct functor_validator_maxLength: public base::functor_validator
                {
                    std::string _msg;
                    size_t _len;
                    functor_validator_maxLength(const std::string& msg, const size_t& len): _msg(msg), _len(len) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_maxLength(*this);
                    }
                    std::string operator()(const std::string& value)
                    {
                        if (value.size() > _len)
                            return _msg.empty() ? "Max length overreached. Must be <= " + utils::to_string(_len) : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_maxLength(msg, len));
                return *this;
            }
            schema_string   &startsWith(const std::string& prefix, const std::string& msg = "")
            {
                struct functor_validator_startsWith: public base::functor_validator
                {
                    std::string _msg;
                    std::string _prefix;
                    functor_validator_startsWith(const std::string& msg, const std::string& prefix): _msg(msg), _prefix(prefix) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_startsWith(*this);
                    }
                    std::string operator()(const std::string& value)
                    {
                        if (value.size() < _prefix.size())
                            return _msg.empty() ? "Prefix incorrect. Must be: " + _prefix : _msg;
                        for (size_t i = 0; i < _prefix.size(); i++)
                        {
                            if (value[i] != _prefix[i])
                                return _msg.empty() ? "Prefix incorrect. Must be: " + _prefix : _msg;
                        }                            
                        return "";
                    }
                };
                this->addValidator(new functor_validator_startsWith(msg, prefix));
                return *this;
            }
            schema_string   &noChars(const std::string& forbidden, const std::string& msg = "")
            {
                struct functor_validator_noChars: public base::functor_validator
                {
                    std::string _msg;
                    std::string _forbidden;
                    functor_validator_noChars(const std::string& msg, const std::string& forbidden): _msg(msg), _forbidden(forbidden) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_noChars(*this);
                    }
                    std::string operator()(const std::string& value)
                    {
                        if (value.find_first_of(_forbidden) != std::string::npos)
                            return _msg.empty() ? "Forbidden char found. String must not have: " + _forbidden : _msg;                        
                        return "";
                    }
                };
                this->addValidator(new functor_validator_noChars(msg, forbidden));
                return *this;
            }
            result<std::string> parse(std::string entry)
            {
                return this->runAllValidators(entry);
            }
        };
    }
}

#endif
