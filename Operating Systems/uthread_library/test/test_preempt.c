#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void *arg)
{
    printf("thread%d\n", uthread_self());
    return 0;
}

int thread2(void *arg)
{
    uthread_create(thread3, NULL);
    printf("thread%d\n", uthread_self());
    return 0;
}

int thread1(void *arg)
{
    printf("preemption?\n");
    return 0;
}

int main(void)
{

    uthread_join(uthread_create(thread1, NULL), NULL);
    printf("We should come back\n");
    return 0;
}