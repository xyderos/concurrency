# Synchronization

__Synchronize and coordinate threads__

### __Atomicity__

* Requires the existence of an __atomic test and set__ instruction in __hardware__

Since threads __can be preempted__, there should exist an instruction that does so in __1 CPU cycle, uninterruptable__

* Such instruction __loads a word from the memory and sets it to some value with no possibility of something happening in between__

1) If the value was 0 and you set it to 1, __you have ownership__
2) If the value was 1 and you set to 1, __you dont have ownership__

```
try_again:  ldstub address -> register
            compare register, 0
            branch_equal got_it
            call go_to_sleep
            jump try_again
got_it:     return
```
_Sample instruction to implement a lock in assembly_

Keep in mind that the logic __remains the same__, whether you sleep (eg sleep lock) or go back directly (spin lock) and no one guarantees you that even if you wake up, you will be the one to get the lock

There are a lot of similar instructions that guarantee __atomicity__, but there are __others that involve atomic instructions, such as compare and swap__

### __Critical section__

Code region where it should be completed __atomically__ with no possible interruptions.

The logic is simple:

1) __Lock__
2) __Do stuff__
3) __Unlock__

You might __lose the processor__ due to a context switch, __but no other thread will enter the critical section__, the other threads will have to wait for you to finish

* __Should be as short as possible__
* __Lock all your shared data between threads, either globals or static variables__

POSIX provides __three synchronization variables__ and the __join()__ function

There are two things you should do in general:

1) __Protect your shared data__, using _locks_
2) __Prevent threads from wasting resources when there is nothing to do__, using _semaphores, condition variables, join, barriers, etc_

### __Mutex__

* __Mutual Exclusion__ is the most basic __synchronization variable__
* __Singler owner for a code section

There is guarantee that there is a priority based scheme in mutexes, it is perfectly valid a thread to acquire the lock even if qanother thread has been waiting for a longer time, in other words, __starvation__

_They can also have a priority queue :)_

* It is __absolutely illegal (although permitable)__ for a mutex to __acquire a lock and for another one to release it__, __never release a mutex from a different thread__, better take a look at __semaphores__

You can also __try getting the lock__:

1) __lock__ is waiting to acquire the lock
2) __trylock__ will either succeed or return a __EBUSY__ signal

_Implies that you need to figure out what to do_

*Dont forget that __although you think mutexes protect data, they actually protect code sections__*

Always keep in mind that by implementing more complex mutexes to improve __fairness__ or __efficiency__, it is usually the case where your __program will become much slower because of the overhead__

__Read/Write locks__

* __You read often but you rarely write__

__RWlocks__ __read concurrently__  but writing is __serialized__

_Obviously you create overhead_

Factors to keep in mind

1) __The length of the data structure__
2) __Contention__

Rule of thumb

* __A global variable will be locked with a mutex__, whereas __traversing a 1000 element linkedlist will be with an RWlock__

Workflow

1) The first reader that will request the lock __will get it__
2) Subsequent readers will also get it as long as they are reading
3) When a __writer__ requests a lock, its put on the __sleeping queue__ until __readers exit__
4) A second writer will also be put on the queue __based on priority__
5) A new reader will be put on the __readers sleeping queue__ until all __writers__ exit
* __As long as there are writers and are appended to the queue, the scheduling is in favor of them, writing needs to be done in order for a reader to be runnable__
* __There is no priority between the readers__

__Priority Inheritance Mutexes__

Assume a high prio thread is blocked from a lower prio thread, that is in turn block by a middle prio thread hogging the CPU

* __Fairness__, elevate the scheduling prio of the __lower prio thread__ to the one __with the highest priority__
* __This is not the case in POSIX, a lot of threads might suffer _priority inversion___
* This is an option for __real time threads only__

In general, POSIX defines __two types of priority inheritance mutexes__

1) __Priority Ceiling Mutex__
* __You declare a priority a priority level for the mutex and any threads that locks automatically gets that priority__
* __A thread may lock a mutex without any change in priority level__

2) __When a thread goes to sleep__
* __while waiting for the mutex__
* __the owner is given the priority of the sleeper if the sleeper has higher priority__
* __Upon release, the sleeper regains its priority__

* __Difficult implementation!!!__

__FIFO Mutexes__

* __Ensure that there is an order based on priority where each thread is queued to use a mutex__

* Kinda rare

__Recursive mutexes__

* __They can be locked multiple times from the same thread without deadlocking__

_Not a good idea to use them, means that you probably have designed a bad algorithm_

* __Could for examnple implement an unlock all function, but should it be that way?__

__Non blocking synchronization__

* __All the standard functions but append a try prefix before them__

* __Good for spin locks and when you try to implement lock hierarchy__

__Debug a mutex__

* __Append a d before mutex in development__

1) Keeps track of owners
2) How many times was it locked
3) How many times you tried locking it

__Spin locks aka trylock__

* __Dont wait or sleep, constantly try to acquire the lock!__

Use them when

1) __The critical section is relatively short__
2) __You have high contention for the lock__
3) __You run on multiple CPUs__

* __A good practise is to test it and time it against a normal lock__
_If you see a difference, use it_

There is an extension called __adaptive spin locks__

* If the mutex is __running__ the lock is __spinning__
* else __dont__

_Super expensive for user space threads, this is a feature some operating systems offer_

Rule of thumb

* __If you know how long the critical section is, you can try to use it, otherwise you are wasting resources__


### __Semaphores__

Once upon a time...

When trains and railroads were rare and there was only a single track, only one train could go through at a time. _Semaphores_ were invented to let the trains know if other trains were on the rails at the same time. It used to be a vertical pole with a flag that could either hang at either 45 or 90 degrees to indicate the existence of other trains

__In computer science__

The simplest for of a semaphore, a __counting semaphore__ is a variable that you can __increment up to a specific limit, but can never decrement below zero__.

* __post__ increments it
* __wait__ decrements it

#### __If the semaphore is greater than zero, you can get it, otherwise you sleep until someone else increments it__

_This sound rather useful in situations where you care about __if it is either your turnor when you need to wait for something___

eg 

* __Wait in a thread with value zero__
* __Until another thread increments the semaphore which indicates you are good to go__

__POSIX has our backs!__

* __they are async safe! you can call post inside an eg signal handler__
* __it is the only condition variable that is async safe__

### __Be aware that semaphores can fail because of SIGNALS__

* Can return -1 with __errno to EINTR__

* If you __block away all signals__, then you can safely use it, otherwise use it __in a loop__

```
while (sem_wait(sem) != 0) {}
```

Note that __this is an extreme case where a signal can interrupt a semaphore__

One striking example of using semaphores is the __producer/consumer problem__

* Since semaphores are working __based on a number__, in a lot of cases __buffers' size__ is usually the number contained in the semaphore, __making the program agnostic of a particular number__, it __essentially encodes the minimum number of a buffer/structure__

A __Barrier__ based ona semaphore is useful when you want to know how many __threads have completed their work__

* Each __exiting thread__ will __increment the semaphore__ and the __thread that is waiting for them will decrement it__
* __Much better than join__!

__Semaphores include a __mutex__ internally.__

1) The first __wait locks the mutex__
* If it is __greater than 0__, it is __decremented__, __released__ and __wait__ returns
* If the __value of the semaphore is 0__, then the mutex is __released__ and the thread will go to __sleep__

2) Then there is post
* It __locks the mutex__, __increments__, __releases and wakes up the sleeper__

* We can safely say that a __semaphore__ is perfect when you want to do something that includes some sort of __counter and threads should not always be active__

### __Conditional Variables__

__The conditions on semaphores was arbitrary, you had to change the condition all the time__

As long as you can express a condition, you can use a __condition variable__

Workflow:

1) A thread __obtains a mutex__
2) Tests the __condition__
* __No other thread can alter the variable if it doesnt hold the mutex associated with that variable__
3) If the condition is __true__, you keep going __until you release a mutex__ (you can have multiple condition variables and thus you dont exit directly)
4) If the condition is __false__, the mutex __is released for you__ and you go to sleep
5) When another thread __alters the state__, it __signals__ you, waking up the sleeping thread
6) Your waken up thread __acquires the mutex__, __inspects__ the condition, and it will __either continue or go back to sleep__

__Keep in mind that you must test the condition__

* The thread that signaled you might havent evaluated the condition
* The condition might have changed while you were waking up and acquiring the mutex
* The condition __can be altered__

In order to wake up all the threads, you can __broadcast__

* The order of notifying is __undefined__, meaning that the threads will __contest__ one another to acquire the mutex

* There can be __unwanted contention of the mutex__, especially when you __broadcast__

Some things to keep in mind

* __Always signal when the condition is true!!!__

* __Always associate a mutex with a condition variable, dont mix them up!!!__

There is the possibility to __wait for a specific amount of time__ to wake up after a specific amount of time

* __Are you really sure that the condition will change by then?__
* __You can use either sleep or nanosleep__, but the system clock __might limit__ the precision
* Once the __waiting time expires__, you will get an __errno__ of __ETIMEDOUT__ and your thread will leave the __sleeping queue__

_In general, dont mess around with time in threads, it is super error prone_

__Lost wakeup__

* You attempted to __read or write on the condition without locking__
* You miss a __wakeup__ because you havent gone to __sleep__

### __Monitors__

* __Simply encapsulate the shared data with the locking mechanism__

1) In C
* __Declare everything to be local in the access function__
2) In C++
* __Object wrapper that does that for you__

_This is an example, dont use this code snippet, it will cause __undefined behavior___
```
void count_things(int i)
{
    static int count = 0;
    static pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&count_lock);
    count += i;
    i = count;
    pthread_mutex_unlock(&count_lock);
    
    return (i);
}
```

Awesome because __it enforces you to manipulate the data within monitored code__
* You will never __fail to lock__
* Access is __well defined__

Bad in
1) __Overlapping locks__
2) __trylock functions__

### __Message queues__

* __Solve them by using the producer/consumer model with a queue__

1) Complete control
2) Simple

Is there a reason to interrupt threads upon receiving a message? Are there any workarounds?

1) __Poll__ from this thread
2) __Wait to complete__ and then do a queue lookup
3) __Create a new thread to execute the task__

The best way to interrupt a POSIX thread is to use __signals__

### __Cross process synchronization__

* __Use shared memory__

_Just because threads from different processes are invisible to one another, this should happen by using a __shared kernel mutex___

* All the threads __must be aware__ of the synchronization variable and __one must initialize it__

_Then use it as a normal synchronization variable_

__CAVEAT__

* __Processes themshelves cannot be multithreaded__

_When one goes to sleep, it takes much longer to synchronize in cross processes rather than inside a single process_

* If an unsuccessful system call for the synchronization __fails__, the thread will go to __sleep__

1) Locally scheduled threads will put the __LWP__ to sleep
2) If one wakes it up, it wakes up

__None guarantees you that the thread that just woke up will succeed in the request to the synchronization variable__

A neat idea is to use a __file that contains the lock__ and by that you can __extend its lifetime__

### __life cycle__

* Initialize __once__
* Upon __destruction__, you must __free__ after destroying (this applies if you __mallocd__ the variable)
* Make sure that __it will not be used again__
* Make sure that __none else is having a pointer to this variable still__

### __Problems__

__Deadlocks__

* __A thread is waiting for another thread to proceed and the other one does the same thing__

Example:

Thread 1 acquires lock 1m thread 2 acquires lock 2
Thread 1 wants to acquire lock 2
Thread 2 wants to acquire lock 1

Solve by first __utilizing lock hierarchy__, keep locking and unlocking in the same order
* For cases that you dont need hierarchy, __trylock__

_In case it fails, release everything and try again_

Practical example:

Imagine a paging system that needs to acquire some pages from the memory

1) If I have locked page #2 and I need page #1
* __I am trying to lock page #1__
2) If I succeed, everything is good
* __keep executing__
3) If I fail, release the lock from page #2 and try acquiring the lock from page #1
* __I just preserved the order__

```
pthread_mutex_lock(&m2);
// some work is happening
if (EBUSY == pthread_mutex_trylock(&m1))
{
    pthread_mutex_unlock(&m2);
    pthread_mutex_lock(&m1);
    pthread_mutex_lock(&m2);
}
// I just got them both
//keep working
```

__Race conditions__

* __Undesired behavior when expected deterministic behavior__

In other words, __based in luck__

* __Usually happens when there is not a proper locking mechanism__

Rule of thumb

* __As long as you have reasonably acceptable answer from data races within multiple threads, it is okay__

__Recovery__

* In runtime, __you cant__

__You can implement fallback solutions, but there is probably a problem with your code__

1) For a single process
* __its fine, all the threads you spawned will die__
2) For multiple processes
* __problems arise, probably a problem with your coding techniques__

Two possible solutions

1) __undo mechanism__
* __introduced in system v__
* __invoked when owner process dies__
* __expensive and complex__

2) __Two stages__
* __Database technique, severe restrictions on crash recovery__

Workflow

1) Build a __timestamped structure__ indicating __how the db should look after the operation__
2) __Save that structure__
3) Do the change
4) __Update the timestamp on the database__
5) Delete the structure from the disk

_At the end of the day, if you need to introduce complex locking mechanisms you might want to reconsider you code and deadlocks are pretty nasty to recover from_