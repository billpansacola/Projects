#include <queue.h>
#include <uthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int thread1(void *arg)
{
    printf("thread1\n");
    return 5;
}
int main(void)
{
    int ret;
    uthread_t tid;
    tid = uthread_create(thread1, NULL);
    uthread_join(tid, &ret);
    printf("thread1 returned %d\n", ret);
    return 0;
}