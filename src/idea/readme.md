# Idea

### __Allow one program to do __multiple tasks____

#### __Why?__

1) Unification around POSIX
2) __Shared Memory Symmetric Multiproceccors__

### __Some operating systems background__

Memory is divided into two sections

1) __Kernel space__, where the operating system resides,
2) __User space__, where the programs reside.

The distinction was happening based on __implicit agreements__ between programmers, there wasn't any __hardware support__ for enforming __memory isolation__, implying that a __user space program__ could access a __kernel space__ program. When a program needed to call a __kernel space__ function, it could directly call that and have access to the resources

Each program, essentially had:

1) The actual program, which in reality is a bunch of instruction pointed to by the __ProgramCounter__ register,
2) Some data, either __global__ or __local__ that are being used,
3) The __stack__, where __local data__ and return addresses are stored, where the __StackPointer__ register is responsible for the active location on the stack

In modern operating systems, memory protection is enforced by __hardware__ by having two different modes.

1) __User mode__, where a normal user program can run
2) __Kernel mode__, which allows some special programs to run that only __the kernel can execute__

* Examples of programs in kernel mode are __IO instructions, processor interrupt instructions, instructions that control the virtual memory and of course the change mode operation__

This implies that a __user space__ programs can access only __user space memory__ directly. In order to call a special instruction from the kernel, the user program makes a __system call__. This in turn makes some arguments and invokes a __trap instruction__. This causes the kernel to take over the machine.

1) The kernel figures out the request based on the data passed from the __trap__
2) It checks the users __permissions__
3) Executes the task
4) Returns information to the user

The OS needs to maintain data of each __process__ which defines the __process state__ into a __Process Control Block__.

Since __multiple processes can be run by multiple users within a system, they can directly access one anothers memory if there is a Shared Memory Segment__

### __Process and thread structure__

* Essentially, the only thing kernel knows about the program is the __process structure__

This can be eg __memory map, the cpu state that contains priorities, registers, signal mask, kernel stack etc as well as file descriptors with signal dispatches__

You might see that a thread is often called a __light weight process__ because the kernel essentially treats threads as processes that share some stuff (the ones mentioned above), think of it as a virtual CPU that is ready to execute a task

* __Thread is a user level term__
* __Light weight process is a kernel level term__
* When creating new processes, __the fork()__ function is used 
* Whereas when creating the corresponding threads, __clone()__ is used

Since we schedule __threads and not processes__, a process with two threads will use twice as much resources as one with a single thread, and __each thread holds its state__

Threads also have some special characteristics bounded to them based on their __capabilities__ eg _scheduling requests_

__This produces kernel level concurrency__ and most of the stuff should remain __unchanged__, every change of those might cost you either __performace or portability__

#### __Context switching__

When two processes perform context switch, the kernel:

1) __Saves the registers__
2) __Changes the virtual memory pointers__
3) __Loads the registers from the other process__
4) __continues execution__

Threads __only require a subset of the registers to be changed in order to perform a context switch__ (remember the term _virtual cpus_ from above)

In particular:

* Stack
* Stack Pointer
* Program Counter
* Thread info included in the __thread structure__ eg scheduling priority, signal masks
* and the CPU registers, Stack pointer and Program Counter are actuall just __registers__
* All the rest comes from __the process__ (_remember that the process structure is something that you cant change, it strictly resides on the kernel space_)

There is an implicit problem that arises, since you are sharing a lot of stuff from the __process view__, you introduce bugs within synchnronization and scheduling, eg if a thread is reading from a file, this is is internally using the _file position pointer_, so if two threads try to read without synchronization from a file __you will not have a deterministic behaviour__

_Note, the stack is actually part of the main programs heap_

### __Threads__

Just like an operating system can run multiple processes, __processes do the same by running more than one thread__

Definition:

__A different stream of control that can execute its instructions independently allowing multiple processes to execute numerous tasks__. It is an abstract concept, it comprises of everything a computer does in executing a traditional program. It is the program state that when it gets __scheduled on the CPU, it is the thingy that does the work__.

If a process __has data, code, kernel state and a set of registers__, the actual threads resides within that, think of it as the __instance of a computation__.

The actual differences between a thread and process are quite a few.

* A process is __heavy weight, kernel level entity__ and includes all the extra info such as __virtual memory map, file descriptors, IDs, etc__. The only way to access the data in process structure is to __invoke a system call__, the user program is __process agnostic__

* A thread is __light weight__, it has

1) __registers__,
2) __stack__,
3) and some other data.

Threads share:

1) __stuff the comprise the process as a kernel entity__, eg the address space, file descriptors etc,
2)  __the code__, it can be executed from __any thread__, 
3) __the state__

__Thread structure resides at the user space, making it fast to access__

Threads interact with the kernel in various ways:

1) __System calls__, functions that end up trapping to routines in the kernel.
2) __Signals__, the UNIX way of __interrupting a running process and informing it that something happened__
3) __Synchronization__, __mechanisms that coordinate multiple threads__
4) __Scheduling__, place threads into the CPU.

### __Know how__

In general, you can thread programs that should execute stuff independently, a lot of workers doing a lot of things. Examples are:

1) __Debuggers__, the monitor the program, run the GUI, keep track of performance etc
2) __Servers__,
3) __Repetitive tasks__, eg a simulation

Programs can seem to be reasonable to thread, eg:

1) __Numerical programs__, eg matrix operations, but its tricky in order to implement the correct slicing, synchronization, etc
2) __Old code__, inspecting some old code and thinking of doing modifications, but most likely you will bump into small interactions and cases that you didnt count on before and thus make it difficult to make the transition

* Sometimes compilers (or even runtimes) can thread stuff for you, depending on the complexity and the structure both of the program and the language itself

* __Not all programs need to be threaded__