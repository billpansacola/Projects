#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

sigset_t interrupt;

void signal_handler(int signum)
{
    uthread_yield();
}

void preempt_disable(void)
{
    /* TODO Phase 4 */
    sigprocmask(SIG_UNBLOCK, &interrupt, NULL);
}

void preempt_enable(void)
{
    /* TODO Phase 4 */
    sigprocmask(SIG_BLOCK, &interrupt, NULL);
}

void preempt_start(void)
{
    /* TODO Phase 4 */
    struct sigaction sa;
    struct itimerval timer;

    // Initialize the Signal mask
    sigemptyset(&interrupt);
    sigaddset(&interrupt, SIGVTALRM);

    /*
    reference: http://www.informit.com/articles/article.aspx?p=23618&seqNum=14
    setting up virtual timer
    */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signal_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = HZ;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = HZ;

    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}
