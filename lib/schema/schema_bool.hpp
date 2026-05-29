/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_bool.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 17:25:06 by ighannam          #+#    #+#             */
/*   Updated: 2026/05/29 19:19:41 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEMA_BOOL_HPP
# define SCHEMA_BOOL_HPP

# include "schema_base.hpp"
#include "utils.hpp"
# include <algorithm>

namespace schema
{
    namespace detail
    {
        class schema_bool: public schema_base<bool, schema_bool>
        {
        private:
            typedef schema_base<bool, schema_bool > base;
            std::vector<std::string> true_indicators;
            std::vector<std::string> false_indicators;
        public:
            schema_bool() 
            { 
                true_indicators.push_back("true");
                false_indicators.push_back("false");
            };
            ~schema_bool() {};
            schema_bool& truthy(std::string msg = "")
            {
                struct functor_validator_true : public base::functor_validator
                {
                    std::string _msg;
                    std::vector<std::string> _true_indicators;
                    functor_validator_true(std::string& msg, std::vector<std::string> true_indicators) : _msg(msg), _true_indicators(true_indicators) {};
                    std::string operator()(bool value)
                    {
                        if (!value)
                            return _msg.empty() ? utils::to_string(value) + " must be true." : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_true(msg, true_indicators));
                return *this;
            }
            schema_bool& falsy(std::string msg = "")
            {
                struct functor_validator_false : public base::functor_validator
                {
                    std::string _msg;
                    std::vector<std::string> _false_indicators;
                    functor_validator_false(std::string& msg, std::vector<std::string> false_indicators) : _msg(msg), _false_indicators(false_indicators) {};
                    std::string operator()(bool value)
                    {
                        if (value)
                            return _msg.empty() ? utils::to_string(value) + " must be false." : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_false(msg, false_indicators));
                return *this;
            }
            schema_bool& add_truthy(const std::string& word)
            {
                true_indicators.push_back(word);
                return *this;
            }
            schema_bool& add_truthy(const std::vector<std::string>& words)
            {
                for (size_t i = 0; i < words.size(); i++)
                {
                    true_indicators.push_back(words[i]);
                }
                return *this;
            }
            schema_bool& add_falsy(const std::string& word)
            {
                false_indicators.push_back(word);
                return *this;
            }
            schema_bool& add_falsy(const std::vector<std::string>& words)
            {
                for (size_t i = 0; i < words.size(); i++)
                {
                    false_indicators.push_back(words[i]);
                }
                return *this;
            }
            result<bool> parse(const std::string& entry)
            {
                std::string lower_case_value = entry;
                std::transform(entry.begin(), entry.end(), lower_case_value.begin(),::tolower);
                bool is_truthy = utils::oneOf(true_indicators, lower_case_value);
                if (!is_truthy && !utils::oneOf(false_indicators, lower_case_value))
                    return result<bool>::failure(error(this->field_name, entry + " must be valid."));
                return this->runAllValidators(is_truthy);
            }
        };
    }
}

#endif