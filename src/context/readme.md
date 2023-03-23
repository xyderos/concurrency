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