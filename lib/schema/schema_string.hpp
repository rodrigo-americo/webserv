/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_string.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:21:49 by ighannam          #+#    #+#             */
/*   Updated: 2026/05/29 15:53:44 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHENA_STRING_HPP
# define SCHENA_STRING_HPP

# include <string>

# include "schema_base.hpp"
#include "utils.hpp"


namespace schema
{
    namespace detail
    {
        class schema_string: public schema_base<std::string, schema_string>
        {
        private:
            typedef schema_base<std::string, schema_string> base;
            
        public:
            schema_string() {};
            ~schema_string() {};
            
            schema_string   &minLenght(size_t len, std::string msg = "")
            {
                struct functor_validator_minLenght: public base::functor_validator
                {
                    std::string _msg;
                    size_t _len;
                    functor_validator_minLenght(std::string msg, size_t len): _msg(msg), _len(len) {};
                    std::string operator()(std::string value)
                    {
                        if (value.size() < _len)
                            return _msg.empty() ? "Minimal lenght not reached. Must be >= " + utils::to_string(_len) : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_minLenght(msg, len));
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