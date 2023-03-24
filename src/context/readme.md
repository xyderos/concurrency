# Context

Some general notes about concurrent programming in general

### __Fork__

__Never call fork in a multithreaded program unless you __exec__ directly__

* Forked processes/threads share everything, including the state and if they die, the state of the parent is lost!

* If a mutex was locked at the time of the call to fork, then it is still locked in the child, if such thing happens, you loose access to the mutex

* Thread specific data and destructors are not called!

* Things become even messier if you involve signals.

* Just dont fork with multithreaded programs

__Fork handlers__

Can execute three functions

1) __Prepare, before fork happens__
2) __Parent__, __happens after the fork in the parent__
3) __Child__, __happens after the fork child process__

* Do all the preparation and synchronization there

### __exec__

* Does not execute cleanup handlers, destructors, etc

### __process exit__

* Will not execute cleanup handlers

### __f(trylock/lock/unlock)file__

* Thread safe file reading functions

#### __A lot of functions that are not thread safe have already been implemented by appending the _r suffix___

### __Functional models__

1) __Boss/Worker__

* One thread is the __boss__, assigning tasks to __worker threads__
* When the worker is __done__, it signals the boss that its done and waits to receive anoother task
* Or the boss __can poll the worker to see if he is available to receive a task__

A variation is the __work queue model__ aka __producer/consumer__

eg a secretary at an office

2) __Work Crew__

* __Multiple threads work together in a single task__
* The task __is divided into pieces__ and parts of it are done __in parallel__

eg a working crew of cleaners in a building, __each one has its own job__

3) __Pipeline__

* Each task is split into smaller tasks that __each next step has a dependency on the previous one__
* The basic difference with __work crew__ is that, we need to produce __multiple results__ (high throughput)

eg Automoibile factory pipeline, __each part produces high throughput__

### __Scheduling issues__

* Just because you set real time scheduling and assigned high priorities, it __doesnt mean its faster__, overhead exists especially when 3rd party libraries come into place

* Multiple priorities and policies __will drastically increase the complexity of the program__, might cost performance also

#### __Priority inversion__

* When three or more threads __block the thread with the highest priority__

_eg top prio thread waits for a resource locked from a low prio thread while a mid prio thread is running_

Workarounds:

1) Associate a priority, __as high as the highest prio a thread can take with each resource and force any thread that using this object to increase its priority to the same as the associated resource__

2) Use the default throughput scheduling policy, __ensures fairness and avoids starvation__

#### __In order to achieve real time scheduling, you need to run with root privileges__

### __Granularity__

__The smallest storage unit a machine can load/store in an instruction__

* Since data race conditions can occur, you can apply some techniques to avoid __granularity race conditions__ based on __word tearing__

Three different types of granularities

1) __Natural granularity__

Despite the cpu, cache and instruction set architecture, the __natural granularity depends on the organization of the memory and buses__ eg an eight bit processor can process 8 bits, but the buss can transfer 32 or 64 bit memory granules

2) __Actual granularity__

When the computer supports multiple sets of granularities, the __compiler decides the actual granularity of a program__ by the instructions it produces. __This means that if you bring a 64 bit granule and you have an eight bit granule in the code, the operation is not atomic__


3) __System granularity__

* The OS should be responsible to provide __persistent granularity on the default settings the compiuler uses when generating object files__