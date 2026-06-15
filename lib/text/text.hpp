/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 11:23:24 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:28:34 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXT_HPP
# define TEXT_HPP

# include "text_style.hpp"
# include "text_builder.hpp"

namespace text
{
	typedef detail::text_style style;
	typedef detail::text_builder builder;

	inline style	bold()														{ return style().bold(); }
	inline style	faint()														{ return style().faint(); }
	inline style	italic()													{ return style().italic(); }
	inline style	underline()													{ return style().underline(); }
	inline style	invert()													{ return style().invert(); }
	inline style	hidden()													{ return style().hidden(); }
	inline style	strikethrough()												{ return style().strikethrough(); }
	inline style	color(double r, double g, double b)							{ return style().color(r, g, b); }
	inline style	color(std::vector<double> arr)								{ return style().color(arr); }
	inline style	interpolate(double r, double g, double b, double factor)	{detail::rgb color = detail::rgb(r, g, b); return style().interpolate(color, factor); }
	inline style	interpolate(std::vector<double> arr, double factor)			{detail::rgb color = detail::rgb(arr); return style().interpolate(color, factor); }
	inline style	gradient(double r, double g, double b, double factor = 1.0)	{detail::rgb color = detail::rgb(r, g, b); return style().gradient(color, factor); }
	inline style	gradient(std::vector<double> arr, double factor = 1.0)		{detail::rgb color = detail::rgb(arr); return detail::text_style().gradient(color, factor); }
	inline builder	add(std::string text)										{ return builder(text); }
	inline builder	add(detail::text_style style, std::string text)				{ return builder(style, text); }
}

#endif
