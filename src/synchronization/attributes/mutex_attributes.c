#include "mutex_attributes.h"

#include <pthread.h>

void mutex_attributes(void) {
  pthread_mutex_t lock;

  pthread_mutexattr_t attrs;
  pthread_mutexattr_init(&attrs);

  // type of mutex
  // NORMAL, cannot detect deadlock, if you attempt to relock, its a deadlock
  // unlocking it from another thread is undefined behavior
  // unlocking while unlocked is undefined behavior
  // ERRORCHECK, attempt to relock without unlocking returns an error
  // unlock it from another thread returns an error
  // unlock and unlocked mutex will return an error
  // RECURSIVE, attempting to relock this mutex without first unlocking it will
  // succeed in locking the mutex rest of the cases are the same as the error
  // check
  // DEFAULT, recursive lock is undefined behavior,
  // unlock from not the calling thread is undefined behavior
  // if you try to unlock from another thread or double unclock its an error
  pthread_mutexattr_settype(&attrs, 0);

  // prioceiling specifies the priority ceiling of initialized mutexes. The
  // ceiling defines the minimum priority level at which the critical section
  // guarded by the mutex is executed. prioceiling will be within the maximum
  // range of priorities defined by SCHED_FIFO. To avoid priority inversion,
  // prioceiling will be set to a priority higher than or equal to the highest
  // priority of all the threads that might lock the particular mutex.
  // oldceiling contains the old priority ceiling value
  pthread_mutexattr_setprioceiling(&attrs, 0);

  // PTHREAD_PRIO_NONE, priority and scheduling are not affected by the mutex
  // ownership.
  // PTHREAD_PRIO_INHERIT, value affects a thread's (such as thrd1) priority and
  // scheduling when higher-priority threads block on one or more mutexes owned
  // by thrd1 where those mutexes are initialized with PTHREAD_PRIO_INHERIT.
  // thrd1 runs with the higher of its priority or the highest priority of any
  // thread waiting on any of the mutexes owned by thrd1.
  // If thrd1 blocks on a mutex owned by another thread, thrd3, the same
  // priority inheritance effect recursively propagates to thrd3. Use
  // PTHREAD_PRIO_INHERIT to avoid priority inversion. Priority inversion occurs
  // when a low-priority thread holds a lock that a higher-priority thread
  // wants. Because the higher-priority thread cannot continue until the
  // lower-priority thread releases the lock, each thread is treated as if it
  // had the inverse of its intended priority.
  // If the symbol _POSIX_THREAD_PRIO_INHERIT is defined, for a mutex
  // initialized with the protocol attribute value PTHREAD_PRIO_INHERIT, the
  // following actions occur in the Solaris Operating Environment when the owner
  // of that mutex dies
  // The behavior on owner death depends on the value of the robustness argument
  // of pthread_mutexattr_setrobust_np().
  // workflow
  // 1) The mutex is unlocked.
  // 2)The next owner of the mutex acquires it with an error return of
  // EOWNERDEAD.
  // The next owner of the mutex should try to make the state protected by the
  // mutex consistent—the state might have been left inconsistent when the
  // previous owner died. If the owner is successful in making the state
  // consistent, call pthread_mutex_init() for the mutex and unlock the mutex.

  // If pthread_mutex_init() is called on a previously initialized,
  // but not yet destroyed mutex, the mutex is not reinitialized.
  // If the owner is unable to make the state consistent, do
  // not call pthread_mutex_init(), but unlock the mutex.In this event,
  // all waiters will be woken up and all subsequent calls to
  // pthread_mutex_lock() will fail to acquire the
  // mutex and return an error code of ENOTRECOVERABLE.You can now
  // make the mutex state consistent by calling
  // pthread_mutex_destroy() to uninitialize the mutex and
  // calling pthread_mutex_init() to reinitialize it.

  // If the thread that acquired the lock with EOWNERDEAD dies,
  // the next owner acquires the lock with an error code of EOWNERDEAD.
  pthread_mutexattr_setprotocol(&attrs, 0);

  // The scope of a mutex variable can be either process private (intraprocess)
  // or system wide (interprocess). If the mutex is created with the pshared
  // attribute set to the PTHREAD_PROCESS_SHARED state, and it exists in shared
  // memory, it can be shared among threads from more than one process. This is
  // equivalent to the USYNC_PROCESS flag in mutex_init() in the original
  // Solaris threads.
  pthread_mutexattr_setpshared(&attrs, 0);

  // robustness defines the behavior when the owner of the mutex dies
  // PTHREAD_MUTEX_ROBUST_NP, owner of the mutex dies, all subsequent calls to
  // pthread_mutex_lock() are blocked from progress in an unspecified manner.
  // PTHREAD_MUTEX_STALLED_NP, owner of the mutex dies, the mutex is unlocked.
  // The next owner of this mutex acquires it with an error return of
  // EOWNWERDEAD.
  // Your application must check the return code from pthread_mutex_lock() for
  // a mutex of this type.
  // The new owner of this mutex should make the state protected by the mutex
  // consistent; this state might have been left inconsistent when the
  // previous owner died.
  // If the new owner is able to make the state consistent, call
  // pthread_mutex_consistent_np() for the mutex, and unlock the mutex.
  // If the new owner is not able to make the state consistent, do not call
  // pthread_mutex_consistent_np() for the mutex, but unlock the mutex.
  // All waiters are woken up and all subsequent calls to
  // pthread_mutex_lock() fail to acquire the mutex. The return code is
  // ENOTRECOVERABLE. The mutex can be made consistent by calling
  // pthread_mutex_destroy() to uninitialize the mutex, and calling
  // pthread_mutex_int() to reinitialize it.
  // If the thread that acquire the lock with EOWNERDEAD died, the next owner
  // acquires the lock with an EOWNERDEAD return code.
  pthread_mutexattr_setrobust(&attrs, 0);

  // pthread_mutex_consistent() applies only if the symbol
  // _POSIX_THREAD_PRIO_INHERIT is defined and for mutexes that are initialized
  // with the protocol attribute value PTHREAD_PRIO_INHERIT.
  // If the owner of a mutex dies, the mutex can become inconsistent.
  // pthread_mutex_consistent_np makes the mutex object, mutex, consistent after
  // the death of its owner.
  // Call pthread_mutex_lock() to acquire the inconsistent mutex. The
  // EOWNWERDEAD return value indicates an inconsistent mutex.
  // Call pthread_mutex_consistent_np() while holding the mutex acquired by a
  // previous call to pthread_mutex_lock().
  // Because the critical section protected by the mutex might have been left in
  // an inconsistent state by the dead owner, make the mutex consistent only if
  // you are able to make the critical section protected by the mutex
  // consistent.
  // Calls to pthread_mutex_lock(), pthread_mutex_unlock(), and
  // pthread_mutex_trylock() for a consistent mutex behave in the normal manner.
  // The behavior of pthread_mutex_consistent_np() for a mutex that is not
  // inconsistent, or that is not held, is undefined.
  pthread_mutex_consistent(&lock);

  pthread_mutex_init(&lock, NULL);
}
