#ifndef __H_SIGNAL_HANDLING
#define __H_SIGNAL_HANDLING
/*
 * Lucas Streanga
 * 1001612558
 */
#include<csignal>
#include<atomic>

std::atomic<bool> terminated = false;

void signal_handler( int signum ) { terminated = true; }

//seperate thread to recieve signals
void signal_thread()
{
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);


    if(sigwait(&set, &sig) == 0)
        signal_handler(SIGINT);
}

//simple function to block signals
void block_signal(int signal)
{
    sigset_t intmask;

    if ((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, signal) == -1))
    {
        std::cerr << "Failed to init signal mask" << '\n';
        exit(EXIT_FAILURE);
    }
    if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1)
    {
        std::cerr << "Failed to block signal SIGINT" << '\n';
        exit(EXIT_FAILURE);
    }
}

#endif
