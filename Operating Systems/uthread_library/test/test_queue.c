#include <queue.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//simply checks that creating a queue succeeds
void test_create(void)
{
    queue_t q;

    q = queue_create();
    assert(q != NULL);
}

// checks a simple enqueue/dequeue scenario
void test_queue_simple(void)
{
    queue_t q;
    int data = 3, *ptr;

    q = queue_create();
    queue_enqueue(q, &data);

    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data);
}

// OUR own custom made unit test
void test_enqueue_and_dequeue_custom(void)
{
    queue_t q;
    int data1 = 10, data2 = 12, *ptr2;

    q = queue_create();

    queue_enqueue(q, &data2);
    for (int i = 0; i < 5; i++)
    {

        queue_enqueue(q, &data1);
    }
    queue_enqueue(q, &data2);

    queue_dequeue(q, (void **)&ptr2);
    assert(queue_length(q) == 6);
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
    int *a = (int *)data;
    int inc = (int)(long)arg;

    *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
    int *a = (int *)data;
    int match = (int)(long)arg;

    if (*a == match)
        return 1;

    return 0;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *ptr;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Add value '1' to every item of the queue */
    queue_iterate(q, inc_item, (void *)1, NULL);
    assert(data[0] == 2);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;
    queue_iterate(q, find_item, (void *)5, (void **)&ptr);
    assert(ptr != NULL);

    assert(*ptr == 5);
    // test our length function

    assert(ptr == &data[3]);
}

void test_queue_length()
{
    queue_t q = queue_create();
    int *ptr;
    for (int i = 0; i < 11; i++)
    {
        queue_enqueue(q, &i);
    }

    queue_dequeue(q, (void **)&ptr);
    assert(queue_length(q) == 10);
}

void test_enqueue__nullitem()
{
    queue_t q = queue_create();
    //int* ptr;
    int i = 0;
    i = queue_enqueue(q, NULL);
    assert(i == -1);
    assert(queue_length(q) == 0);
}

void test_dequeue__nullitem()
{
    queue_t q = queue_create();
    int i = 0;
    i = queue_dequeue(q, NULL);
    int j = queue_dequeue(NULL, NULL);
    assert(i == -1);
    assert(j == -1);
    assert(queue_length(q) == 0);
}

void test_delete_item()
{
    queue_t q = queue_create();

    int data = 3;
    int data2 = 4;
    int data3 = 5;

    queue_enqueue(q, &data);
    queue_enqueue(q, &data2);

    queue_enqueue(q, &data3);

    int num = 5;
    queue_enqueue(q, &num);

    queue_delete(q, &num);
    queue_delete(q, &data);


    assert(queue_length(q) == 2);
}

void test_destroy(){
    queue_t q = queue_create();

    int data = 3;
    int data2 = 4;

    queue_enqueue(q, &data);
    queue_enqueue(q, &data2);
    int destroyed = queue_destroy(q);
    assert(destroyed == 0);
}

int main()
{
    test_create();
    test_queue_simple();
    test_enqueue_and_dequeue_custom();
    test_iterator();
    test_queue_length();
    test_enqueue__nullitem();
    test_dequeue__nullitem();
    test_delete_item();
    test_destroy();
    return 0;
}