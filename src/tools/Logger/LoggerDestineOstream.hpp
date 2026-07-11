/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LoggerDestineOstream.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 21:33:49 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 22:28:11 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_DESTINE_OSTREAM_HPP
# define LOGGER_DESTINE_OSTREAM_HPP

# include <iostream>

# include "utils.hpp"
# include "LoggerDestine.hpp"

class LoggerDestineOstream: public	LoggerDestine
{
private:
	std::ostream	&_os;

	void	doWrite(const utils::str &value) const
	{
		_os << value;
	}

public:
	LoggerDestineOstream(std::ostream &os, LoggerLevel::type min_lvl, LoggerLevel::type max_lvl)
		: LoggerDestine(min_lvl, max_lvl), _os(os) {};
	LoggerDestineOstream(std::ostream &os, LoggerLevel::type lvl)
		: LoggerDestine(lvl), _os(os) {};
	~LoggerDestineOstream() {};
};

#endif
