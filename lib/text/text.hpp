/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 11:23:24 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 15:11:02 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXT_HPP
# define TEXT_HPP

# include "text_style.hpp"
# include "text_builder.hpp"

namespace text
{
	using style = detail::text_style;
	using builder = detail::text_builder;

	inline style	bold()														{ return style{}.bold(); }
	inline style	faint()														{ return style{}.faint(); }
	inline style	italic()													{ return style{}.italic(); }
	inline style	underline()													{ return style{}.underline(); }
	inline style	invert()													{ return style{}.invert(); }
	inline style	hidden()													{ return style{}.hidden(); }
	inline style	strikethrough()												{ return style{}.strikethrough(); }
	inline style	color(double r, double g, double b)							{ return style{}.color(r, g, b); }
	inline style	color(std::array<double, 3> arr)							{ return style{}.color(std::move(arr)); }
	inline style	interpolate(double r, double g, double b, double factor)	{ return style{}.interpolate(detail::rgb(r, g, b), factor); }
	inline style	interpolate(std::array<double, 3> arr, double factor)		{ return style{}.interpolate(detail::rgb(std::move(arr)), factor); }
	inline style	gradient(double r, double g, double b, double factor = 1.0)	{ return style{}.gradient(detail::rgb(r, g, b), factor); }
	inline style	gradient(std::array<double, 3> arr, double factor = 1.0)	{ return detail::text_style{}.gradient(detail::rgb(std::move(arr)), factor); }
	inline builder	add(std::string text)										{ return builder(std::move(text)); }
	inline builder	add(detail::text_style style, std::string text)				{ return builder(std::move(style), std::move(text)); }
}

#endif
