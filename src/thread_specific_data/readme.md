# Thread Specific Data

__Concept__:

Data become __globally accessible__ and __unique per thread__

Tries to fix __race conditions__ by creating an array of __key offsets to value cells__ within the __thread structure__

__Workflow__

1) Make a __key__ and add it to the __Thread Specific Data__ arrays for __all threads__
* __pthread_key_t__
2) __Access it__
* __getspecific, setspecific__
3) It takes a __void*__, can store whatever
4) Can also __declare a destructor for each item__
* __declare a function to run when the thread exits__

* If you __delete__, no callback is called
* If you call __getspecific__ and the value has not been set, you get __NULL__, obviously, __destructors wont be called__ upon __null values__

There is a __special register__ that most compiler writers dont use
* __This holds a reference to the thread structure of the running thread__

The __array is of limited size for portrability purposes__

### __A smart hack, errno__

__errno is a value in the thread specific data structure__, system calls affect that value, its not __global__, its __local__.

While thread specific data sounds good, __it is super expensive__ (think of all the calls that are happening)

* Use it when you __want to avoid a lot of globals__

* Use it when you want to __cache once__

### __Thread Local Storage__

* __alternative to TSD__
* Declare a set of __globals to be locals__
* Since you cannot create variables on the fly, you __probably(?) need to do some shady memory mapping manipulation__
* Bad thing to do, __not portable__

### __Why use them?__

* __Destroy the idea of a global state__
* __Avoid false sharing__