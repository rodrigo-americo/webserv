/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_builder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 13:13:01 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 13:41:37 by bruno-valer      ###   ########.fr       */
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
				segment(std::string _text):	text(std::move(_text)), style(), has_style(false) {};
				segment(std::string _text, text_style _style):	text(std::move(_text)), style(std::move(_style)), has_style(true) {};
				~segment() = default;

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
			text_builder(std::string text): _segments(1, segment(std::move(text))) {};
			text_builder(text_style style, std::string text): _segments(1, segment(std::move(text), std::move(style))) {};
			~text_builder() = default;

			text_builder	&add(std::string text)
			{
				_segments.emplace_back(std::move(text));
				return *this;
			}

			text_builder	&add(text_style style, std::string text)
			{
				_segments.emplace_back(std::move(text), std::move(style));
				return *this;
			}

			std::string		build() const
			{
				std::string	result;
				for (const segment &seg : _segments)
					result += seg.apply();
				return result;
			}
		};
	}
}

#endif
