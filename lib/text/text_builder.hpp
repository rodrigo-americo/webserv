/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_builder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 13:13:01 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:30:11 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXT_BUILDER_HPP
# define TEXT_BUILDER_HPP

# include <string>
# include <vector>

# include "text_style.hpp"

namespace text
{
	namespace detail
	{
		class text_builder
		{
			struct segment
			{
				std::string	text;
				text_style	style;
				bool		has_style;

				segment():	text(), style(), has_style(false) {};
				segment(const std::string &_text):	text(_text), style(), has_style(false) {};
				segment(const std::string &_text, const text_style &_style):	text(_text), style(_style), has_style(true) {};
				~segment() {};

				std::string	apply() const
				{
					if (has_style)
						return style.apply(text);
					return text;
				}
			};

		private:
			std::vector<segment>	_segments;
		public:
			text_builder(std::string &text): _segments(1, segment(text)) {};
			text_builder(text_style &style, std::string &text): _segments(1, segment(text, style)) {};
			~text_builder() {};

			text_builder	&add(const std::string &text)
			{
				_segments.push_back(segment(text));
				return *this;
			}

			text_builder	&add(text_style &style, std::string &text)
			{
				_segments.push_back(segment(text, style));
				return *this;
			}

			std::string		build() const
			{
				std::string	result;
				for (size_t i = 0; i < _segments.size(); i++)
					result += _segments[i].apply();
				return result;
			}
		};
	}
}

#endif
