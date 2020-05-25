# **PROJECT 2 REPORT**
#### *Written by Justin Buhay and Bill Pansacola*
---

- Similar to Project 1, Justin and I remained in constant communication via
Discord and FB messenger while working on Project 2. However, we did not engage
in Pair Programming, but rather updated each other whenever we made any
progress.

#### ***PHASE 1 DEVELOPMENT***

- We first created our queue struct containing 4 data values. The array of
items of the type ```void*```, the front to keep track of the front index of the
queue, the rear to keep track of the last index of the queue, and the length to
keep track of its size.
- Then we implement ```queue_create()```, by allocating memory for the new
queue. And initialize its values. We need ```-1``` for both front and read
because that indicates that it doesn't contain any values yet.
And then we also allocate memory for the items.
- We free the queue and deallocate and test for any constraints with ```queue_destroy()```
- For ```queue_enqueue()``` we use two pointers to keep track of where we should
add our value. We want to add at our rear, and if there is none, we push our
front and read to both be ```0s```, to indicate 1 data value.
- We remove the front of the queue with ```queue_dequeue()```. And we do similar
implementations as ```enqueue()```.
- ```queue_delete()``` runs in ```0(n^2)``` because we have to delete at a
certain index and move every item to its proper index.
- ```queue_iterate()``` runs in ```O(n)``` because we iterate through the entire
queue and call the certain function on it.
- ```queue_length()``` prints out the length of our struct.

#### ***PHASE 2 DEVELOPMENT***

- For this phase, we worked on implementing ```uthread_create()```, ```uthread_yield()```, ```uthread_exit()```,
and the temporary implementation of ```uthread_join()```.
- We struggled a little bit with understanding the functionality with this phase
 at first but broke it down into subproblems.
- ```uthread_create()``` was an independent function where we initialize if we
are on our main thread and make the input thread, and otherwise create a new thread.
- We have to also account for 2 important global variables. The last thread we
created and our current thread, which we returned for ```uthread_self()```.
We make the ```latestUthreadMade``` variable increment based on the way threads
are indexed.
- For ```uthread_yield()``` we implemented it temporarily such that the current
thread returns back to READY state after it changes. For future phases we had
to reimplement this so that it goes into ```BLOCKED``` state instead.
- For ```uthread_join()``` we ran the infinite loop and break if there are no
more threads in the ```ready_queue```, as stated in project2.html file.

#### ***PHASE 3 DEVELOPMENT***

- During the beginning portions of Phase 3 Development, we ran into problems
implementing ```uthread_join()``` as our ```uthread_yield()``` wasn't working as
intended. Our yield function would simply push back a thread in the ready queue
without considering if it was a ```BLOCKED``` state or not. As stated above,
we reimplemented it in order to properly implement Phase 3.
- We completely reimplemented our ```uthread_join()``` function,
containing many "if" blocks to check the multiple edge cases for when a parent
joins a child. We need to wait for the child to finish it's execution in
the ```exit()``` thread and then we get its return value after the while loop
in our ```uthread_join()``` function.
- We did this by implementing another queue along side
our ```ready_queue```, ```blocked_queue```.
  - ```queue_enqueue()``` and ```queue_dequeue()``` was utilized to remove and
  insert threads in both the READY and BLOCKED queue.
- If the thread to be joined has already exited (or returned) then we can just
obtain its ```retval```.
- So in general we had to change up our ```uthread_exit()``` function to dequeue
 our ```blocked_q``` and ```ready_q```, telling us that it has finished executing.

#### ***PHASE 4 DEVELOPMENT***
- Phase 4 Development was pretty straightforward since we referenced most of our
implementations based off the material given to us via Piazza and
the Project2.html.
- Professor Porquet provided an Online Tutorial link to implementing an alarm,
that we based on our implementation on. We simply changed ```interval```
and ```value``` to ```HZ``` since it was stated in the directions.
- We placed ```preempt_start()``` in our ```initialize_uthread_library```
function to start preemption during the initialization phase.
- ```preempt_disable()``` and ```preempt_enable()``` were placed before and
after a context switch.

#### ***TESTING UNITS***
- We added all the edge cases in any cases of our queue failing on its
implementation
- We added tests that range from using all of our queue functions, and we used
assert for debugging to enqueuing and dequeuing NULL values to test for any
possible returns to not be -1.
- We also added an extra test for our ```uthread_join()``` function
in ```test_join.c``` which includes creating and joining a thread and obtaining
its return value into the main thread.
- Also modified ```uthread_yield.c``` so that it creates three threads in the
main function and joins thread 2, getting its return value. In this case it had
a return value of 2.

### **REFERENCES**

1. http://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html
2. https://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts
3. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions
4. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm
5. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Blocking-Signals
6. [Tutorial on implementing an alarm](http://www.informit.com/articles/article.aspx?p=23618&seqNum=14&fbclid=IwAR1R3PBmNgo4XpYCiOkEU2Jt6J-psYsBhITEa_QItoW4xzUIbsmr8vlMj7E)
7. [This video helped us implement a queue](https://www.youtube.com/watch?v=okr-XE8yTO8)
