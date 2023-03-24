# Context

Some general notes about the behavior of traditional processes in conjunction with pthreads.

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