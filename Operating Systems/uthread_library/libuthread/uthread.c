#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

#define RUNNING 0
#define READY_TO_RUN 1
#define EXITED 2 // Zombie
#define BLOCKED 3
#define NOT_JOINED 4
#define JOINED 5

// 65535 for now
#define USHRT_MAX 65535

static uthread_ctx_t ctx[USHRT_MAX];
// the current thread we are on. 0 for main thread.
// TIDs
static uthread_t currentUthread = 0;
static uthread_t latestUthreadMade = 0;
static queue_t ready_q;
static queue_t blocked_q;

// data structure that can store information about a single thread.
typedef struct
{
	// TID
	uthread_t tid;
	// pointer to Context of the thread
	uthread_ctx_t *ctx;
	//information about stack
	void *stack;
	// information on the state of the thread; i.e. Running(0), Ready to run(1), or exited(2).
	int state;
	int retval;
	int joined;

} TCB;

static TCB *tcbarray;

/* TODO Phase 2 */
int find_item_ready_to_run(void *data, void *arg)
{
	int *a = &tcbarray[*((int *)data)].state;
	int match = (int)(long)arg;

	if (*a == match)
		return 1;

	return 0;
}
void uthread_yield(void)
{
	/* TODO Phase 2 */
	TCB *ptr;
	ptr = NULL;
	queue_dequeue(ready_q, (void **)&ptr);

	if (ptr != NULL && tcbarray[currentUthread].state != EXITED)
	{

		tcbarray[currentUthread].state = BLOCKED;
		queue_enqueue(blocked_q, &tcbarray[currentUthread]);
		ptr->state = RUNNING;
		uthread_t tempcurrUthread = currentUthread;
		currentUthread = ptr->tid;
		preempt_disable();
		if (ptr->ctx != NULL)
		{
			uthread_ctx_switch(tcbarray[tempcurrUthread].ctx, ptr->ctx);
		}
		preempt_enable();
	}
	else if (ptr != NULL && tcbarray[currentUthread].state == EXITED)
	{
		uthread_t tempcurrUthread = currentUthread;
		currentUthread = ptr->tid;

		if (ptr->ctx != NULL)
		{
			uthread_ctx_switch(tcbarray[tempcurrUthread].ctx, ptr->ctx);
		}
	}
}

uthread_t uthread_self(void)
{
	/* TODO Phase 2 */
	return currentUthread;
}

//When uthread_create is called for the first time, this function must first initialize the uthread library by registering the so-far single execution flow of the
//application as the main thread that the library can later schedule for execution like any other thread.

void initialize_uthread_library()
{

	// Starting Preemption
	preempt_start();

	// Register the main thread
	tcbarray = malloc(20 * sizeof(*tcbarray));
	ready_q = queue_create();
	blocked_q = queue_create();

	tcbarray[latestUthreadMade].tid = latestUthreadMade;

	void *stack = uthread_ctx_alloc_stack();
	tcbarray[latestUthreadMade].stack = stack;

	// &ctx[0]
	tcbarray[latestUthreadMade].ctx = &ctx[latestUthreadMade];
	tcbarray[latestUthreadMade].state = RUNNING;
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	// no need to initialize ctx[0]
	if (latestUthreadMade == 0)
	{
		initialize_uthread_library();
	}

	latestUthreadMade++;
	void *topOfStack = uthread_ctx_alloc_stack();
	uthread_ctx_init(&ctx[latestUthreadMade], topOfStack, func, arg);
	tcbarray[latestUthreadMade].tid = latestUthreadMade;
	tcbarray[latestUthreadMade].ctx = &ctx[latestUthreadMade];
	tcbarray[latestUthreadMade].stack = uthread_ctx_alloc_stack();
	tcbarray[latestUthreadMade].state = READY_TO_RUN;
	tcbarray[latestUthreadMade].joined = NOT_JOINED;
	queue_enqueue(ready_q, &tcbarray[latestUthreadMade]);

	if (topOfStack == NULL)
	{
		return -1;
	}

	return latestUthreadMade;
}

void uthread_exit(int retval)
{
	/* TODO Phase 2 */
	tcbarray[currentUthread].retval = retval;
	tcbarray[currentUthread].state = EXITED;
	TCB *ptr;
	ptr = NULL;
	queue_dequeue(blocked_q, (void **)&ptr);
	if (ptr == NULL)
	{
	}
	else
	{
		ptr->state = READY_TO_RUN;
		queue_enqueue(ready_q, ptr);
	}
	uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO Phase 2 */
	//	while(1==1){
	//		if(queue_length(ready_q) <= 0 || ready_q == NULL){
	//			//printf("We break\n");
	//			break;
	//
	//		} else{
	//			uthread_yield();
	//			//printf("Did we return\n");
	//		}
	//	}
	//	return -1;
	/* TODO Phase 3 */

	// include check if tid cannot be found
	if (tid == 0 || tcbarray[currentUthread].tid == tid || tcbarray[tid].joined == JOINED)
	{
		return -1;
	}

	// If it's ACTIVE, it means that its either READY_TO_RUN or RUNNING, thus making it an active thread
	if (tcbarray[tid].state == READY_TO_RUN || tcbarray[tid].state == RUNNING)
	{
		while (tcbarray[tid].state != EXITED)
		{
			// this is "blocking" but we need to fix our yield in order to properly block
			uthread_yield();
		}
		if (retval != NULL)
		{
			*retval = tcbarray[tid].retval;
		}
	}
	else if (tcbarray[tid].state == EXITED)
	{
		tcbarray[tid].joined = JOINED;
		tcbarray[currentUthread].joined = JOINED;

		// collect resources and free
		if (retval != NULL)
		{
			*retval = tcbarray[tid].retval;
		}
		uthread_ctx_destroy_stack(tcbarray[tid].stack);
		free(tcbarray[tid].ctx);
	}

	return 0;
}
