# Cancellation

* __Stop the execution of a thread before it completes__

Things to do:

1) __Release locks__
2) __Free memory__

Idea:

* __Cancel based on thread id, at some point soon, the thread will  be cancelled__

A lot of operating systems implement __asynchronous cancellation__

* __kill it__

_The killer assumes that the thread can be safely eliminated, difficult to impossible_

POSIX implements a __cancellation state__ for __each thread__

* __Disable cancellation in critical section__
* __enable it afterwards__

#### __Deffered cancellation__

A thread exits __when it polls the library to find out if it should exit__ or when it is __blocked in a library call that is a cancellation point__

* Poll via __testcancel__ (checks if a bit has been set)
* If there is a pending request, __the function will not return at all and the thread will die__
* Some standard cancellation functions also call that

The thread will run if it is __in critical section__
* __Difficult if you need bounded cancellation times__

_Cancellation points are points in the control flow where the control returns to the scheduler, for example when you invoke a system call, in other words, if ou can influence the scheduling decisions, in other words, __interruptable___

* _When you are about to have a context switch and execute another call, this is a cancellation point_

#### __Cancellation cleanup handlers__

* __Mechanism to cleanup the threads state upon cancellation__
* __Called upon the exiting of the thread__

These handlers are basically __pushed into the threads cleanup stask__, which is basically an entity for us, but in reality its simple entries into the __call stack__

* __These functions will be called whether you cancel or exit__
* If the thread __wont be cancelled__, __pop__ the functions, you __dont need them__

___Always remember to pop after you exit the current scope of the thread (braces)___

__A basic law__

* In section of code that you __allocate resources__, push a __cleanup handler__
* They are responsible to __restore variables, free up memory, etc__
* __All these sections of code must be atomic in order to respect cancellation__
* Then you run __with cancellation allowed__
* Upon completion, the __handler is called__ and the handler shall be popped
* If there are cancellation points within the allocation and deallocation period, __cancellation must be turned off__

#### __Cancellation points__

They are __different from vendor to vendor__ (although there are standard functions that are cancellation points, google them)
* This makes them __not portable__

In general, there are __four cases__ where you need to cancel a thread

1) Kill it __instantly__
2) Kill it in __bounded CPU time__
3) Prevent it from making __global changes__
4) Prevent it from __wasting CPU time__

_4 implies 2, 1 is impossible and best case not well defined, 3 is not meaningful_

Conclusion: __4__

__Time cancellation is not guaranteed in POSIX__
* __Asynchronous__ cancellation will probably occupy it for a couple of ms
* __Deferred__ cancellation will keep you informed at the __synchronization points__

Imagine a log prio thread being asked to cancel from a super high prio thread
* might never get cancelled

* As long as you care about __4__, your thread will cancel in its next scheduled time in __async cancellation__

Deferred cancellation is
1) more or less a __polling scheme__ when the thread is running
2) __async__ when the thread is blocked

Example:

1) T2 is asking T1 to cancel
2) T1 probably has a cancellation point
3) When hit, the thread will be cancelled

Where to put the cancellation point?

* __inside loops!__ eg once every N iterations

### __Cancel sleeping threads__

* Obviously, you need for it to __wake up__ and hit a __cancellation point__

What if we had multiple hits in our example code?

* __Use synchronization!__ eg either a semaphore or a condition variable since waiting is a __cancellation point__

Rules of thumb

* __Never cancel a thread that has called join, you might create a zombie__

* Any cancellation handlers will be run with the mutex held. You’d better
make sure the mutex gets unlocked if you ever expect to use it again. Only the
owner thread is allowed to unlock a mutex, so, by definition, the unlocking has to
occur in a cleanup handler!

* You dont want to be starting new threads when there are one that are not gone! Either wait for them to join of exit

* In the code snippets, imagine calling again the searches function. There would be some shady stuff happening since no proper cleanup would occur on the global thread array. A barrier might be a solution but there are issues with detaching stuff, use a semaphore!

* Keep in mind that you can toogle on and off cancellation within the program based on the state

At the end of the day, __avoid cancellation unless its explicitly needed__