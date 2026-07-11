#ifndef SIGNAL_HPP
# define SIGNAL_HPP

# include <csignal>

extern volatile sig_atomic_t g_stop;

void setup_signal_handlers();

#endif