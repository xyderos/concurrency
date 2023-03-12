# Scheduling

__There are a few techniques to schedule threads into kernel resources__

The idea behind that is that, we need a model to support the complexity of the operating system and the user space needs.

### __Many threads on one Light Weight Process__

aka __Many to One model__ or __co routines__

There are multiple threads created on the __user space__ and all of them are __taking turns__ into running into a __one Light Weight Process__

* Everything is happening on __user space__, __creation, scheduling, synchronization__
* __Fast, cheap, no kernel resources consumed__

This is a clean programming model, but it is not __meant to be run on multiple proccessing machine__, 

* It will not offer any speedup
* __Blocking calls will block the process__

#### __Hack for blocking system calls__

* __Wrapper routine around the blocking operation__
* Replacing the __blocking call__ with a __non blocking one__
* When such system call is made, the thread will go to __sleep__ and another one __will be run__
* __The kernel will signal that the system call is complete__
* The thread will __wake up__ and everything will proceed

### __One thread per Light Weight Process__

__You have a Light Weight Process per thread__, many threads can run simultaneously on different CPUs.

_The blocking call issue mentioned before is solved_

* But a __thread creation__ involves a __LWP creation__, meaning that you need to do a system call while you __might be handling scheduling and synchronization__

* Also, __LWP__ consume kernel resources and you cannot __spawn__ infinite threads

### __Many threads on many Light Weight Processes__

__Some threads are multiplexed into a number of Light Weight Process equal or smaller to the number of threads__

* __Creation, scheduling and synchronization is happening on user space__

Notes:

* __The number of LWPs can be tuned per machine__
* __Solves the blocking call issue__
* __Limited by the size of virtual memory__
* _No one is using that model, too strict_

### __Two level Model__

aka __Many To Many but supports requests for One to One binding for particular threads__

### __Thread scheduling__

Two basic models

1) __Process Local Scheduling__ (aka __Process Contention Scope__ aka __Unbound threads__ aka __Many to Many__)
* Local to the process
* The implementation controls which thread goes to what LWP
* Uses both __Many to One__ and __Many To Many__
2) __Global Scheduling__ (aka __System Contention Scope__ aka __Bound threads__ aka __One to One__)
* Scheduling is done __by the kernel__
* Threads have a __policy__ and a __priority__ associated with them which implements __details on the kernel level__, both of which are __optional__

There are two main cases where __scheduling comes handy__ but doenst solve all our problems

1) __Two completely independent processes__

* __Time slicing__ makes sense to be used for our scheduling purposes

2) __Direct dependency between two processes__ eg one needs to perform a task for me to continue

* __Priority__ makes sense to be used

_In reality, we use a mix of both, although most programs are based on the dependent case and scheduling is mainly happening based on the dependence upon a programs need for synchronization_

### __Process Local Scheduling__

__Scheduling is happening on the library level, but keep in mind that the library chooses the thread to bind on the LWP, THE LWP IS SCHEDULED ON A GLOBAL LEVEL__

_But dont care about the global scheduling, focus on the local one_

There are __four ways__ to perform a __thread level context switch__

1) __Synchronization__, eg request a lock and get it, if its not available, go to a queue and sleep until its available

2) __Preemption__, a __low prio thread does something__ that invokes __a high prio thread__, eg release a lock and change the priority

3) __Yield__, if there is a __similar priority thread queued, it will be executed__

4) __Time slicing__

__Synchronization and Yielding are the fastest and cheapest ways to perform thread level context switch__

_You can set the __priority__, although you shouldnt, check the examples under the __lifecycle__ section_

Threads have __four states__ (__can be five but its not POSIX compliant and thus not portable__)

1) __Active__, the thread is in the __LWP__
2) __Runnable__, the thread is ready to run but there are not enough __LWP__
3) __Sleeping__, waiting for a synchronization variable
4) __Stopped__, a call to suspension has been made, another thread can wake it up, __NOT POSIX COMPLIANT__
5) __Zombie__, dead but its waiting for its resources to be collected

A few conclusions

* __Runnables have lower priority that active threads__
* __If a sleeping thread will move to runnable, its a priority issue__
* __If one runnable has higher priority that an active one, it will replace it__
* __Undefined for equal guarantees__

Same thing happens on the __kernel side__, if a thread exits then its __LWP will go idle on the kernel level, if its requested, it will be used, if its not active for a long time, its getting killed, if you if your app becomes more active, more LWPs will be created for you__

_When a thread blocks, the LWP should also go to sleep, when the thread is unblocked, the LWP wakes up_

### __System Contention Scope__

__A thread is permanately bound to a LWP__, either __sleeping, suspended or active__

When to use them?

* __All the time__
* __When you want time slicing__
* __When you need a lot of threads, use the Process Scope!__

_Remember, more global threads means more kernel resources_

The reasoning behind that is that, if you are computationally intensive, you expect your threads to __either compute or wait for an event__ which means that you are unlikely to be doing __local context switching__

### __Context switch__

__The process of taking a thread off a LWP and replace it with one that is ready to run__

__The state of computation resides in the registers__, so when two process context switch

1) __All the registers are stored in the Process Strucutre__
2) __All the registers stored in the other process structure are loaded into the registers__
3) __Go back to user mode__

The rest of the data eg file descriptors remain in the __process structure__

_The exact method is happening on LWP level also, as well as threads, but on thread level it is super fast since no system call is needed_

#### __Preemption__

__Kick a thread off its LWP or an LWP from its CPU and place something else__

* For process scope, __the library takes care of that__
* For global scope, __the kernel scheduler takes care of that__

__There is a dedicated signal for the LWPs for preemtpion__

_Of course, the LWP runs the handler, sees the request and performs context switch_

_Another note: __NEVER SEND SIGNALS TO LWP, ONLY IN THREAD LEVEL___

__Preemption requires a system call, so it is implied that the kernel generates the signal__

Notice that context switching by preemption is __much more expensive from "voluntary" context switch__

You can request for a __number of LWPs__ (check _pthread_setconcurrency()_), but if you decide to have more and reuqest for such, __nothing is certain__

RULE OF THUMB:

* __If your program is CPU bound, one LWP per CPU will give you the maximum processing power (assuming that you also have the same number of threads)__

* __If your program is both CPU and IO bound, then one LWP per CPU is enough to cover all simultaneous blocking calls__

* __If your program is IO bound, you need as many LWPs as the number of simultaneous blocking system calls__

__You can also go into a more "real time" conditions!__

Typically, when you need to go into the absolute maximum efficiency, you might __NOT__ want to use

* __Blocking system calls__,
* __No IO__ (the best practise is to set buffers in the real time thread and fill them in from a normal thread that executes the IO calls)
* __No demand paging, yuo know beforehand all the memory you might need__

_POSIX has 3 real time scheduling classes, all of which are optional to implement_

1) __SCHED_FIFO__
* A priority queue
* No time slicing
* Threads never change their priority unless it happens explicitly

2) __SCHED_RR__
* __FIFO with time slicing__
* Threads rotate in their queues

3) __SCHED_OTHER__
* Different per distribution

You can define:

1) __Scope__
* Process
* System
2) __Policy__
* RR
* FIFO
* OTHER
3) __Priority__
* min and max
4) __Inheritance__
* Whether or not you will inherit the policy and its parameters from the creator

#### __Real time systems__

In case you are working in a __real time__ environment, you need to set up most of those fields
* POSIX doesnt provide default values

POSIX will not allow you to control over LWP and CPU, but can let you decide in which __allocation domain__ (sets of CPUs) your LWP can run
* Verify that by __both the allocation domain you want to run and the allocation domains you dont want to run__

__Cache locality__

If you context switch for a short time, the chances that the cache is still valid are quite good

This is solved via the __processor affinity__
* If an LWP is running on a specific processor, __it will probably stay there__
* If the CPU it was scheduled is not available fast enough, __it will go to another one__

Know how:

__It is difficult to figure it out__

* Servers want to have high throughput and avoid starvation, it make sense to have sufficient LWPs
* When you implement a broker based on messaged, priority matters

_At the end of the day, some threads might need to request something from other threads and will go to sleep until this is done_