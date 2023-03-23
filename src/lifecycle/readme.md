# Lifecycle

### Same idea behind birth, life, death

Assume we are talking about a random program

1) Load the program
2) Link dynamic libraries
3) Run initialization code
4) Call main

In order to create a pthread, __a function and its arguments are sufficient__.

Should you pass any arguments to the thread routine, you need to initalize the __pthread_attr_t__

### __Attributes__

* An __exited thread__ is the return value the thread will return upon finishing its execution

#### __Detach state__

* A __detached__ thread is a thread where its execution will run independently from the parent thread and you __do not need to wait for the result of the thread__
* A __non detached thread__ is a thread that it wil be waiting to be __joined__ and get its return value

* If a detached thread dies, its threadID can be reassigned to another thread before it returns, it is a rule of thumb that __a thread must be joined when it is terminated__, else the resources are not released and it is thus a memory leak

### __Scope__

* A __Process scoped thread__ __competes for resources with other threads__ in the __process level__
* A __System scoped thread__ ___competes for resources as a kernel entity__, essentially __faking an entity__

### __Scheduling policy__

* __Round Robin__
* __FIFO__
* __Other__, to inherit the normal priority on linux threads based on the characteristics of the thread


### __Scheduling inheriting policy__

* __Inherit__ to get parents scheduling policies and ignore the ones passed as arguments
* __Explicit__ to use the parameters used from the attributes

### __Set scheduling parameters__

* Only __priority is supported__, a single number

### __Thread stack__

* Although you can, you shouldnt be manually allocating stack for a thread.

* Based on the boundary, the size requested is aligned with the next memory boundary and after that a page with no access permission is "padded" so if you exceed the space you will get an overflow

* Default value is 1MB

* Take into consideration local variables, information structures, sequences

* in order to reduce __memory usage__, you can assign the size of the stack as a sum of the STACK_MIN which is sufficient to initializa a thread plus what you need to allocate

* You can combine that with the __stack address__ in order to request to use this memory address, keep in mind that if you request a specific size, __YOU ARE MALLOCING__

### __Thread ID__

* Needed to control scheduling classes, cancellation, signals

### __Initialize your data__

Sometimes yuo might want to do some sort of initialization before starting the program (eg .ini section in libraries)

The best way to do so is to have a thread calling a function (obviosuly locked) to check if initialization is complete

* All the other calls to that will be __blocked__

This function is called __once__

Drawbacks:

1) Keeping __global state__

2) Might be expensive, will probably need a flag to indicate its done!

### Some notes

* #### __It is good practise to never wait for a thread, use synchronization for getting the status__

* #### If the __main thread calls the thread exit function, it will kill the main thread but not the child ones__

* #### Although a thread can kill a thread, this should __never be the case__, the other thread might be holding locks and thus is __NOT POSIX COMPATIBLE__ (implement on your own)

### __A few notes on pthreads__

* Pthreads are identified by the pthread_t within the system, each thread has a unique indentifier, you can compare them only by using __pthread_equal__

* The __initial thread or main thread__ retains the behavior of a classic UNI process when main returns, that means that the process can terminate without letting the rest fo the threads to complete.

* _You dont really need that_

__Detaching a thread is basically nothing more than informing the system that when the thread is done, its resources can be reclaimed__

* __Always detach your threads__, either in attributes or explicitly by calling the detach function

* If you need to know its return value, __join from the parent thread__



#### __A few practical notes on setting up threads policies in Linux and subsequently POSIX environments__

* Do not use functions that use either __internal static buffers or signals with threads, a failure of something can lead to a race condition__

* If you are planning on setting up __scheduling policies and scope__, you need to modify the __resource limits__ given to your __thread__

* Use __setrlimit__ and yer needed capability appropriately

* In order to run it and apply the capabilities

1) you need to either permit your executable to do so by adding its path to the __CAP_SYS_RESOURCES__,
2) or run it with __superuser__ priviledges

Check the modified pipeline, the makefile and the code for more info