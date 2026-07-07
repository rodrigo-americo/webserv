#include <csignal>

volatile sig_atomic_t g_stop = 0;

void handle_signal(int sig){
    (void)sig;
    g_stop = 1;
}

void setup_signal_handlers()
{
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN); 
}