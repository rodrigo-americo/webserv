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

	style	bold()														{ return style{}.bold(); }
	style	faint()														{ return style{}.faint(); }
	style	italic()													{ return style{}.italic(); }
	style	underline()													{ return style{}.underline(); }
	style	invert()													{ return style{}.invert(); }
	style	hidden()													{ return style{}.hidden(); }
	style	strikethrough()												{ return style{}.strikethrough(); }
	style	color(double r, double g, double b)							{ return style{}.color(r, g, b); }
	style	color(std::array<double, 3> arr)							{ return style{}.color(std::move(arr)); }
	style	interpolate(double r, double g, double b, double factor)	{ return style{}.interpolate(detail::rgb(r, g, b), factor); }
	style	interpolate(std::array<double, 3> arr, double factor)		{ return style{}.interpolate(detail::rgb(std::move(arr)), factor); }
	style	gradient(double r, double g, double b, double factor = 1.0)	{ return style{}.gradient(detail::rgb(r, g, b), factor); }
	style	gradient(std::array<double, 3> arr, double factor = 1.0)	{ return detail::text_style{}.gradient(detail::rgb(std::move(arr)), factor); }
	builder	add(std::string text)										{ return builder(std::move(text)); }
	builder	add(detail::text_style style, std::string text)				{ return builder(std::move(style), std::move(text)); }
}

#endif
