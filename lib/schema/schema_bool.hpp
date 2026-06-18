/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schema_bool.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 17:25:06 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/17 15:17:36 by ighannam         ###   ########.fr       */
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
            schema_bool(const schema_bool& other) : base(other), true_indicators(other.true_indicators), false_indicators(other.false_indicators) {}
            schema_bool& operator=(const schema_bool& other) {
                if (this != &other) {
                    base::operator=(other);
                    true_indicators = other.true_indicators;
                    false_indicators = other.false_indicators;
                }
                return *this;
            }
            ~schema_bool() {};
            schema_bool& truthy(const std::string& msg = "")
            {
                struct functor_validator_true : public base::functor_validator
                {
                    std::string _msg;
                    std::vector<std::string> _true_indicators;
                    functor_validator_true(const std::string& msg, const std::vector<std::string>& true_indicators) : _msg(msg), _true_indicators(true_indicators) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_true(*this);
                    }
                    std::string operator()(const bool& value)
                    {
                        if (!value)
                            return _msg.empty() ? utils::to_string(value) + " must be true." : _msg;
                        return "";
                    }
                };
                this->addValidator(new functor_validator_true(msg, true_indicators));
                return *this;
            }
            schema_bool& falsy(const std::string& msg = "")
            {
                struct functor_validator_false : public base::functor_validator
                {
                    std::string _msg;
                    std::vector<std::string> _false_indicators;
                    functor_validator_false(const std::string& msg, const std::vector<std::string>& false_indicators) : _msg(msg), _false_indicators(false_indicators) {};
                    functor_validator* clone() const
                    {
                        return new functor_validator_false(*this);
                    }
                    std::string operator()(const bool& value)
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
                std::string word_lower_case = word;
                std::transform(word.begin(), word.end(), word_lower_case.begin(),::tolower);
                true_indicators.push_back(word_lower_case);
                return *this;
            }
            schema_bool& add_truthy(const std::vector<std::string>& words)
            {
                std::string word_lower_case;
                for (size_t i = 0; i < words.size(); i++)
                {
                    word_lower_case = words[i];
                    std::transform(words[i].begin(), words[i].end(), word_lower_case.begin(),::tolower);
                    true_indicators.push_back(word_lower_case);
                }
                return *this;
            }
            schema_bool& add_falsy(const std::string& word)
            {
                std::string word_lower_case = word;
                std::transform(word.begin(), word.end(), word_lower_case.begin(),::tolower);
                false_indicators.push_back(word_lower_case);
                return *this;
            }
            schema_bool& add_falsy(const std::vector<std::string>& words)
            {
                std::string word_lower_case;
                for (size_t i = 0; i < words.size(); i++)
                {
                    word_lower_case = words[i];
                    std::transform(words[i].begin(), words[i].end(), word_lower_case.begin(),::tolower);
                    false_indicators.push_back(word_lower_case);
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
