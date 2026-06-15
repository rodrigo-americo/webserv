/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_style.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 11:24:44 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 22:45:28 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RGB_HPP
#define RGB_HPP

# include <vector>
# include <string>
# include <sstream>

# include <utils.hpp>

namespace text
{
	namespace detail
	{
		class rgb
		{
		private:
			std::vector<double>	_color;
		public:
			rgb(): _color() {};
			rgb(double r, double g, double b): _color()
			{
				_color.resize(3);
				setColor(r, g, b);
			};

			rgb(const std::vector<double> &arr): _color(arr) {};
			~rgb() {};

			void	setColor(double r, double g, double b)
			{
				_color[0] = r;
				_color[1] = g;
				_color[2] = b;
			}

			void	setColor(std::vector<double> &arr) { _color = arr; }

			rgb	interpolate(const rgb &other, double factor) const { return rgb(utils::lerp(_color[0], other._color[0], factor), utils::lerp(_color[1], other._color[1], factor), utils::lerp(_color[2], other._color[2], factor)); }

			std::string	toAnsi() const
			{
				std::ostringstream	oss;
				oss << "\033[38;2;"
					<< (int)_color[0] << ";"
					<< (int)_color[1] << ";"
					<< (int)_color[2] << "m";
				return oss.str();
			}
		};

		namespace ansi
		{
			static const char *reset = "\033[0m";
			static const char *bold = "\033[1m";
			static const char *faint = "\033[2m";
			static const char *italic = "\033[3m";
			static const char *underline = "\033[4m";
			static const char *invert = "\033[7m";
			static const char *hidden = "\033[8m";
			static const char *strikethrough = "\033[9m";
		}

		class text_style
		{
			private:
				bool	_bold;
				bool	_faint;
				bool	_italic;
				bool	_underline;
				bool	_invert;
				bool	_hidden;
				bool	_strikethrough;
				rgb		_color;
				bool	_has_gradient;
				rgb		_gradient_to;
				double	_gradient_factor;
			public:
				text_style()
					: _bold(false), _faint(false), _italic(false), _underline(false),
						_invert(false), _hidden(false), _strikethrough(false), _color(255, 255, 255),
						_has_gradient(false), _gradient_to(), _gradient_factor(1.0) {};
				~text_style() {};

				text_style	&bold() { _bold = true; return *this; }
				text_style	&faint() { _faint = true; return *this; }
				text_style	&italic() { _italic = true; return *this; }
				text_style	&underline() { _underline = true; return *this; }
				text_style	&invert() { _invert = true; return *this; }
				text_style	&hidden() { _hidden = true; return *this; }
				text_style	&strikethrough() { _strikethrough = true; return *this; }

				text_style	&color(const rgb &color) { _color = color; return *this; }
				text_style	&color(double r, double g, double b) { _color = text::detail::rgb(r, g, b); return *this; }
				text_style	&color(std::vector<double> &arr) { _color = arr; return *this; }

				text_style	&interpolate(const rgb &color, double factor) { _color = _color.interpolate(color, factor); return *this; }
				text_style	&gradient(const rgb &color, double factor = 1.0)
				{
					_has_gradient = true;
					_gradient_to = color;
					_gradient_factor = factor;
					return *this;
				}

				std::string	apply(const std::string &str) const
				{
					std::string	result;

					result += _color.toAnsi();
					if (_bold)			result += ansi::bold;
					if (_faint)			result += ansi::faint;
					if (_italic)		result += ansi::italic;
					if (_underline)		result += ansi::underline;
					if (_invert)		result += ansi::invert;
					if (_hidden)		result += ansi::hidden;
					if (_strikethrough)	result += ansi::strikethrough;
					if (!_has_gradient)
					{
						result += _color.toAnsi();
						return result + str + ansi::reset;
					}

					size_t	str_len = str.size();
					for (size_t i = 0; i < str_len; i++)
					{
						double char_factor = str_len > 1 ?
							(double)i / (double)str_len * _gradient_factor
							: _gradient_factor;
						result += _color.interpolate(_gradient_to, char_factor).toAnsi();
						result += str[i];
					}
					return result + ansi::reset;
				}
		};
	}

}

#endif
