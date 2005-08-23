/* Copyright (C) 1992, 1993, 1994, 1995, 1996 the Florida State University
   Distributed by the Florida State University under the terms of the
   GNU Library General Public License.

This file is part of Pthreads.

Pthreads is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation (version 2).

Pthreads is distributed "AS IS" in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with Pthreads; see the file COPYING.  If not, write
to the Free Software Foundation, 675 Mass Ave, Cambridge,
MA 02139, USA.

Report problems and direct all questions to:

  pthreads-bugs@ada.cs.fsu.edu

  @(#)pthread.c	3.1 10/11/96

*/

/*
 * Implementation of fork, join, exit, etc.
 */

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
# include<stdlib.h>
#endif

#include "internals.h"
#include "setjmp.h"
#include <pthread/asm.h>

#ifdef STAND_ALONE
extern struct pthread tcb[MAX_THREADS];
#endif

volatile int pthread_started = FALSE;
static volatile int n_pthreads = 0;
static volatile pthread_key_t n_keys = 0;
static void (*key_destructor[_POSIX_DATAKEYS_MAX])();

/*------------------------------------------------------------*/
/*
 * pthread_alloc - allocate pthread structure
 */
static pthread_t pthread_alloc(func, arg)
     pthread_func_t func;
     any_t arg;
{
  pthread_t t;
  extern char *pthread_calloc();

#ifdef STAND_ALONE
  int i;
  char *c;

  for (i = 0; i < MAX_THREADS; i++) {
    t = (pthread_t) SA((int) &tcb[i]);
    if (t->state & T_RETURNED && t->state & T_DETACHED) {
      t->state &= ~(T_RETURNED | T_DETACHED);
      c = (char *) &tcb[i];
      for (i = sizeof(*t); i > 0; i--, c++)
	*c = '\0';

      break; 
   }
  }
  if (i >= MAX_THREADS)
    t = NO_PTHREAD;
#else /* !STAND_ALONE */
#ifdef MALLOC
  if (!pthread_started)
    t = (pthread_t) pthread_calloc(1, sizeof(struct pthread));
  else
#endif /* MALLOC */
    t = (pthread_t) calloc(1, sizeof(struct pthread));
#endif /* STAND_ALONE */
  if (t != NO_PTHREAD) {
    t->func = func;
    t->arg = arg;
    t->state = T_RUNNING | T_CONTROLLED;
  }
  return(t);
}

/*------------------------------------------------------------*/
/*
 * pthread_self - returns immutable thread ID of calling thread
 */
pthread_t pthread_self()
{
  return(mac_pthread_self());
}

/*------------------------------------------------------------*/
/*
 * pthread_init - initialize the threads package. This function is 
 * the first function that be called by any program using this package.
 * It initializes the main thread and sets up a stackspace for all the
 * threads to use. Its main purpose is to setup the mutexes and 
 * condition variables and allow one to access them.
 */
void pthread_init()
{
  pthread_t t;
  int i;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGJMP_BUF env;
#else
  sigjmp_buf env;
#endif

#ifdef SOLARIS
  /*
   * This dummy reference ensures that the initialization routine
   * is always linked.
   */
  extern void pthread_dummy();
  if (FALSE)
    pthread_dummy();
#endif /* SOLARIS */

  if (pthread_started)
    return;

  pthread_init_signals();

  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_condattr_init(&pthread_condattr_default);
  pthread_attr_init(&pthread_attr_default);

  t = pthread_alloc((pthread_func_t) 0, (any_t *) NULL);
  t->state |= T_MAIN | T_CONTROLLED;

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGSETJMP(env, FALSE, TRUE);
#else
  sigsetjmp(env, FALSE);
#endif

  process_stack_base = (char *) env[JB_SP];
  pthread_stack_init(t);

  t->attr.prio = MIN_PRIORITY;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  t->base_prio = MIN_PRIORITY;
#ifdef SRP
  t->max_ceiling_prio = NO_PRIO;
  t->new_prio = NO_PRIO;
#endif
#endif
#ifdef DEF_RR
  t->attr.sched = SCHED_RR;
#else
  t->attr.sched = SCHED_FIFO;
#endif
  /*
   * 1st thread inherits signal mask from process
   */
#ifdef STAND_ALONE
  pthread_sigcpyset2set(&t->mask, &proc_mask);
#else
  SIGPROCMASK(SIG_SETMASK, (struct sigaction *) NULL, &t->mask);
#endif
  mac_pthread_self() = t;	
  pthread_q_all_enq(&all, t);
  pthread_q_primary_enq(&ready, t);
  state_change = FALSE;
  pthread_started = TRUE;
#ifdef DEF_RR
  pthread_timed_sigwait(t, NULL, RR_TIME, NULL, t);
#endif
#if defined(IO) && defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__)
  fd_init();
#endif
  n_pthreads = 1; /* main thread counts, fd_server does not count */
#ifdef GNAT
  {
    extern int gnat_argc;
    extern char **gnat_argv;

    dsm_init(gnat_argc, gnat_argv);
  }
#endif /* GNAT */
}

/*------------------------------------------------------------*/
/*
 * pthread_body - base of the pthreads implementation.
 * Procedure invoked at the base of each pthread (as a wrapper).
 */
void pthread_body()
{
  pthread_t t = mac_pthread_self();
#ifdef REAL_TIME
  struct timespec tp;
#endif /* REAL_TIME */
  static void pthread_terminate();
#ifdef DEBUG
#ifdef STAND_ALONE
pthread_timer_q_t pthread_timer;              /* timer queue                 */
#else
pthread_timer_q pthread_timer;                /* timer queue                 */
#endif
#endif /* DEBUG */

#ifdef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif

#ifdef REAL_TIME
  NTIMERCLEAR(tp);
  if (ISNTIMERSET(t->attr.starttime)) {
    tp.tv_sec = t->attr.starttime.tv_sec;
    tp.tv_nsec = t->attr.starttime.tv_nsec;
  }

  if (ISNTIMERSET(t->attr.deadline)) {
    SET_KERNEL_FLAG;
    pthread_timed_sigwait(t, &t->attr.deadline, ABS_TIME, pthread_exit, -1);
    CLEAR_KERNEL_FLAG;
  }

  do {
    if (ISNTIMERSET(tp)) {
      SET_KERNEL_FLAG;
      pthread_timed_sigwait(t, &tp, ABS_TIME, pthread_exit, -1);
      CLEAR_KERNEL_FLAG;
    }
#ifdef DEBUG
    fprintf(stderr, "body time Q head %x\n", pthread_timer.head);
#endif /* DEBUG */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    if (!SYS_SIGSETJMP(t->body, FALSE, TRUE))
#else
    if (!sigsetjmp(t->body, FALSE))
#endif
#endif /* REAL_TIME */

      t->result = (*(t->func))(t->arg);

#ifdef REAL_TIME
    if (!LE0_NTIME(t->attr.period)) {
      PLUS_NTIME(tp, tp, t->attr.period);
      SET_KERNEL_FLAG;
      pthread_cancel_timed_sigwait(t, FALSE, ALL_TIME, FALSE);
      CLEAR_KERNEL_FLAG;
      pthread_absnanosleep(&tp);
    }
  } while (!LE0_NTIME(t->attr.period));
#endif /* REAL_TIME */

  pthread_terminate();
}

/*------------------------------------------------------------*/
/*
 * pthread_terminate - terminate thread: call cleanup handlers and
 * destructor functions, allow no more signals, dequeue from ready,
 * and switch context
 */
static void pthread_terminate()
{
  register pthread_t p, t = mac_pthread_self();
  register pthread_key_t i;
  register cleanup_t new;
#ifdef CLEANUP_HEAP
  register cleanup_t old;
#endif
  sigset_t abs_all_signals;

  /*
   * No more signals, also remove from queue of all threads
   */
  pthread_sigcpyset2set(&abs_all_signals, &all_signals);
  sigaddset(&abs_all_signals, SIGCANCEL);
  if (!pthread_siggeset2set(&t->mask, &abs_all_signals)) {
    SET_KERNEL_FLAG;

    if (t->state & (T_SIGWAIT | T_SIGSUSPEND)) {
      t->state &= ~(T_SIGWAIT | T_SIGSUSPEND);
      sigemptyset(&t->sigwaitset);
    }

    pthread_q_deq(&all, t, ALL_QUEUE);
    pthread_sigcpyset2set(&t->mask, &abs_all_signals);

    CLEAR_KERNEL_FLAG;
  }

  /*
   * call cleanup handlers in LIFO
   */
#ifdef CLEANUP_HEAP
  for (old = (cleanup_t) NULL, new = t->cleanup_top; new;
       old = new, new = new->next) {
    (new->func)(new->arg);
    if (old)
      free(old);
  }
  if (old)
    free(old);
#else
  for (new = t->cleanup_top; new; new = new->next)
    (new->func)(new->arg);
#endif

  /*
   * call destructor functions for data keys (if both are defined)
   */
  for (i = 0; i < n_keys; i++)
    if (t->key[i] && key_destructor[i])
      (key_destructor[i])(t->key[i]);

  /*
   * dequeue thread and schedule someone else
   */

  SET_KERNEL_FLAG;
  if (t->state & (T_SYNCTIMER | T_ASYNCTIMER)) {
#ifdef TIMER_DEBUG
    fprintf(stderr, "pthread_terminate: pthread_cancel_timed_sigwait\n");
#endif
    pthread_cancel_timed_sigwait(t, FALSE, ALL_TIME, FALSE);
  }
  t->state &= ~T_RUNNING;
  t->state |= T_RETURNED;

  /*
   * Terminating thread has to be detached if anyone tries to join
   * but the memory is not freed until the dispatcher is called.
   * This is required by pthread_join().
   * The result is copied into the TCB of the joining threads to
   * allow the memory of the current thread to be reclaimed before
   * the joining thread accesses the result.
   */
  if (t->joinq.head) {
    t->state |= T_DETACHED;
    for (p = t->joinq.head; p; p = p->next[PRIMARY_QUEUE])
      p->result = t->result;
    pthread_q_wakeup_all(&t->joinq, PRIMARY_QUEUE);
  }
  
#ifdef STACK_CHECK
  if (!(t->state & T_MAIN))
    pthread_unlock_all_stack(t);
#endif

  /*
   * The last threads switches off the light and calls UNIX exit
   */
  SIM_SYSCALL(TRUE);
  if (--n_pthreads) {
    pthread_q_deq(&ready, t, PRIMARY_QUEUE);
    CLEAR_KERNEL_FLAG;
  }
  else {
#ifdef STAND_ALONE
    exit();
#else
    pthread_clear_sighandler();
    pthread_process_exit(t->state & T_DETACHED ? 0 : 1);
#endif
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_create - Create a new thread of execution. the thread 
 * structure and a queue and bind them together.
 * The completely created pthread is then put on the active list before
 * it is allowed to execute. Caution: The current implementation uses 
 * pointers to the thread structure as thread ids. If a thread is not 
 * valid it's pointer becomes a dangling reference but may still be 
 * used for thread operations. It's up to the user to make sure he 
 * never uses dangling thread ids. If, for example, the created thread
 * has a higher priority than the caller of pthread_create() and the 
 * created thread does not block, the caller will suspend until the 
 * child has terminated and receives a DANGLING REFERENCE as the 
 * thread id in the return value of pthread_create()! This 
 * implementation could be enhanced by modifying the type pthread_t of
 * a pointer to the thread control block and a serial number which had
 * to be compared with the serial number in the thread control block 
 * for each thread operation. Also, threads had to be allocated from a
 * fixed-size thread control pool or the serial number would become a 
 * "magic number".
 */
int pthread_create(thread, attr, func, arg)
     pthread_t *thread;
     pthread_attr_t *attr;
     pthread_func_t func;
     any_t arg;
{
  register pthread_t t;
  pthread_t parent_t = mac_pthread_self();

  if (!attr)
    attr = &pthread_attr_default;

  if (!attr->flags || thread == NULL) {
    set_errno(EINVAL);
    return(-1);
  }

#ifdef REAL_TIME
  {
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    if ((ISNTIMERSET(attr->starttime) && !GTEQ_NTIME(attr->starttime, now)) ||
	(ISNTIMERSET(attr->deadline)  && !GTEQ_NTIME(attr->deadline , now)) ||
	(ISNTIMERSET(attr->period) && !ISNTIMERSET(attr->starttime))) {
      set_errno(EINVAL);
      return(-1);
    }
  }
#endif /* REAL_TIME */

  t = pthread_alloc(func, arg);
  if (t == NO_PTHREAD) {
    set_errno(EAGAIN);
    return(-1);
  }

  t->attr.stacksize = attr->stacksize;
  t->attr.flags = attr->flags;
#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
  t->max_ceiling_prio = NO_PRIO;
  t->new_prio = NO_PRIO;
#endif
#endif
  if (attr->inheritsched == PTHREAD_DEFAULT_SCHED) {
    t->attr.contentionscope = attr->contentionscope;
    t->attr.inheritsched = attr->inheritsched;
    t->attr.sched = attr->sched;
    t->attr.prio = attr->prio;
#ifdef _POSIX_THREADS_PRIO_PROTECT
    t->base_prio = attr->prio;
#endif
  }
  else {
    t->attr.contentionscope = parent_t->attr.contentionscope;
    t->attr.inheritsched = parent_t->attr.inheritsched;
    t->attr.sched = parent_t->attr.sched;
#ifdef _POSIX_THREADS_PRIO_PROTECT
    t->attr.prio = t->base_prio = parent_t->base_prio;
#else
    t->attr.prio = parent_t->attr.prio;
#endif
  }
#ifdef REAL_TIME
  t->attr.starttime.tv_sec = attr->starttime.tv_sec;
  t->attr.starttime.tv_nsec = attr->starttime.tv_nsec;
  t->attr.deadline.tv_sec = attr->deadline.tv_sec;
  t->attr.deadline.tv_nsec = attr->deadline.tv_nsec;
  t->attr.period.tv_sec = attr->period.tv_sec;
  t->attr.period.tv_nsec = attr->period.tv_nsec;
#endif /* REAL_TIME */
#ifdef DEF_RR
  t->num_timers = 0;
  t->interval.tv_sec = 0;
  t->interval.tv_usec = 0;
#endif
  pthread_queue_init(&t->joinq);
  /*
   * inherit the parent's signal mask
   */
  pthread_sigcpyset2set(&t->mask, &parent_t->mask);
  if (attr->detachstate)
    t->state |= T_DETACHED;
  SET_KERNEL_FLAG;
  ++n_pthreads; 
  if (!pthread_alloc_stack(t)) {
    CLEAR_KERNEL_FLAG;
    set_errno(ENOMEM);
    return(-1);
  }
  pthread_initialize(t);
  pthread_q_all_enq(&all, t);
  pthread_q_primary_enq(&ready, t);
  SIM_SYSCALL(TRUE);
  CLEAR_KERNEL_FLAG;
	
  *thread= t;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_equal() - Cmpares two threads. Returns
 *      0 if t1 <> t2
 *      1 if t1 == t2
 */
int pthread_equal(t1, t2)
     pthread_t t1, t2;
{
  return(t1 == t2);
}

/*------------------------------------------------------------*/
/*
 * pthread_detach - Detaching a running thread simply consists of 
 * marking it as such. If the thread has returned then the resources 
 * are also freed.
 */
int pthread_detach(thread_ptr)
     pthread_t *thread_ptr;
{
  pthread_t thread;

  if ((thread_ptr == NULL) || (*thread_ptr == NO_PTHREAD)) {
    set_errno(EINVAL);
    return(-1);
  }

  thread = *thread_ptr;

  SET_KERNEL_FLAG;

  if (thread->state & T_DETACHED) {
    CLEAR_KERNEL_FLAG;
    set_errno(ESRCH);
    return(-1);
  }
  
  thread->state |= T_DETACHED;

  if (thread->state & T_RETURNED) {
#if defined(MALLOC) || defined(STAND_ALONE)
    pthread_free(thread->stack_base);
#ifndef STAND_ALONE
    pthread_free(thread);
#endif /* !STAND_ALONE */
#else /* !(MALLOC || STAND_ALONE) */
    free(thread->stack_base);
    free(thread);
#endif /* MALLOC || STAND_ALONE */
    *thread_ptr = NO_PTHREAD;
    SIM_SYSCALL(TRUE);
  }

  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_join - Wait for a thread to exit. If the status parameter is
 * non-NULL then that threads exit status is stored in it.
 */
int pthread_join(thread, status)
     pthread_t thread;
     any_t *status;
{
  register pthread_t p = mac_pthread_self();

  if (thread == NO_PTHREAD) {
    set_errno(EINVAL);
    return(-1);
  }
  
  if (thread == p) {
    set_errno(EDEADLK);
    return(-1);
  }

  SET_KERNEL_FLAG;
  
  if (thread->state & T_RETURNED) {
    if (thread->state & T_DETACHED) {
      CLEAR_KERNEL_FLAG;
      set_errno(ESRCH);
      return(-1);
    }

    if (status)
      *status = thread->result;

    thread->state |= T_DETACHED;

#if defined(MALLOC) || defined(STAND_ALONE)
    pthread_free(thread->stack_base);
#ifndef STAND_ALONE
    pthread_free(thread);
#endif /* !STAND_ALONE */
#else /* !(MALLOC || STAND_ALONE) */
    free(thread->stack_base);
    free(thread);
#endif /* MALLOC || STAND_ALONE */
    
    CLEAR_KERNEL_FLAG;
    return(0);
  }
  
  /*
   * clear error number before suspending
   */
  set_errno(0);

  pthread_q_sleep(&thread->joinq, PRIMARY_QUEUE);
  p->state |= T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
  
  if (get_errno() == EINTR)
    return(-1);

  /*
   * status was copied into result field of current TCB by other thread
   */
  if (status)
    *status = p->result;
  return(0);
}

/*------------------------------------------------------------*/
/* Function:
 *    pthread_yield -  Yield the processor to another thread.
 *    The current process is taken off the queue and another executes
 *    Simply put oneself at the tail of the queue.
 */
void pthread_yield(arg)
     any_t arg;
{
  if (arg == NULL && ready.head != ready.tail) {
    /*
     * Place ourself at the end of the ready queue.
     * This allows the other ready processes to execute thus
     * in effect yielding the processor.
     */
    SET_KERNEL_FLAG;
    pthread_q_primary_enq(&ready, pthread_q_deq_head(&ready, PRIMARY_QUEUE));
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_exit -  Save the exit status of the thread so that other 
 * threads joining with this thread can find it.
 */
void pthread_exit(status)
     any_t status;
{
  register pthread_t t = mac_pthread_self();
  t->result = status;
  pthread_terminate();
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_init - Initializes the attr (thread attribute object)
 * with the default values.
 */
int pthread_attr_init(attr)
     pthread_attr_t *attr;
{
  if (!attr) {
    set_errno(EINVAL);
    return(-1);
  }

  attr->flags = TRUE;
  attr->contentionscope = PTHREAD_SCOPE_LOCAL;
  attr->inheritsched = PTHREAD_DEFAULT_SCHED;
  attr->detachstate = 0;
#ifdef DEF_RR
  attr->sched = SCHED_RR;
#else
  attr->sched = SCHED_FIFO;
#endif
  attr->stacksize = DEFAULT_STACKSIZE;
  attr->prio = DEFAULT_PRIORITY;
#ifdef REAL_TIME
  NTIMERCLEAR(attr->starttime);
  NTIMERCLEAR(attr->deadline);
  NTIMERCLEAR(attr->period);
#endif /* REAL_TIME */
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_destroy - Destroys the thread attribute object.
 */
int pthread_attr_destroy(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
  attr->flags = FALSE;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_getschedattr - get the thread scheduling attributes
 */
int pthread_getschedattr(thread, attrs)
     pthread_t thread;
     pthread_attr_t *attrs;
{
  if (thread == NO_PTHREAD)  {
    set_errno(ESRCH);
    return(-1);
  }
  
  if (!attrs || !attrs->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  attrs->sched = thread->attr.sched;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  attrs->prio = thread->base_prio;
#else
  attrs->prio = thread->attr.prio;
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_setschedattr - Set the thread specific scheduling attributes
 */
int pthread_setschedattr(thread,attrs)
     pthread_t thread;
     pthread_attr_t attrs;
{
  pthread_t p = mac_pthread_self();
  pthread_queue_t q;
  int oldsched, run_prio;

  if (thread == NO_PTHREAD)  {
    set_errno(ESRCH);
    return(-1);
  }

  if (!attrs.flags) {
    set_errno(EINVAL);
    return(-1);
  }
  
#ifdef DEF_RR
  if (attrs.sched != SCHED_FIFO && attrs.sched != SCHED_RR) {

#else
  if (attrs.sched != SCHED_FIFO) {
#endif
    set_errno(ENOTSUP);
    return(-1);
  }

  if (attrs.prio < MIN_PRIORITY || attrs.prio > MAX_PRIORITY) {
    set_errno(EINVAL);
    return(-1);
  }

#ifdef REAL_TIME
  if (ISNTIMERSET(attrs.starttime) ||
      ISNTIMERSET(attrs.deadline) ||
      ISNTIMERSET(attrs.period)) {
    set_errno(EINVAL);
    return(-1);
  }
#endif /* REAL_TIME */

  SET_KERNEL_FLAG;
 
  if (thread->state & T_RETURNED) {
    CLEAR_KERNEL_FLAG;
    set_errno(EINVAL);
    return(-1);
  }

  oldsched = thread->attr.sched;
  thread->attr.sched = attrs.sched;
#ifdef DEF_RR
  if (attrs.sched != oldsched && p == thread)
    switch (oldsched) {
    case SCHED_FIFO:
      pthread_timed_sigwait(thread, NULL, RR_TIME, NULL, thread);
      break;
    case SCHED_RR:
      pthread_cancel_timed_sigwait(thread, FALSE, RR_TIME,
					 thread->queue != &ready);
      break;
    default:
      ;
    }
#endif
#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
  if (thread->new_prio == MUTEX_WAIT) {
    thread->new_prio = attrs.prio;
    CLEAR_KERNEL_FLAG;
    return (0);
  }
#endif
  run_prio = thread->attr.prio;
  thread->base_prio = attrs.prio;
  if (thread->max_ceiling_prio != NO_PRIO)
    thread->attr.prio = MAX(attrs.prio, thread->max_ceiling_prio);
  else
    thread->attr.prio = attrs.prio;
#else
  run_prio = thread->attr.prio;
  thread->attr.prio = attrs.prio;
#endif
  q = thread->queue;
  if (q->head != thread ||
      (q->head != q->tail && thread->attr.prio < run_prio &&
       thread->attr.prio < thread->next[PRIMARY_QUEUE]->attr.prio)) {
    pthread_q_deq(q, thread, PRIMARY_QUEUE);
    pthread_q_primary_enq(q, thread);
  }
 
  SIM_SYSCALL(TRUE);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setstacksize - Aligns the "stacksize" to double
 * word boundary and then sets the size of the stack to "stacksize"
 * in thread attribute object "attr".
 */
int pthread_attr_setstacksize(attr, stacksize)
     pthread_attr_t *attr;
     size_t stacksize;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  attr->stacksize = SA(stacksize); /* stack align, see asm_linkage.h */
  return(0);    
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getstacksize - gets the stacksize from an pthread 
 * attribute object.
 */
int pthread_attr_getstacksize(attr, stacksize)
     pthread_attr_t *attr;
     size_t *stacksize;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  *stacksize = attr->stacksize;    
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setscope - Set the contentionscope attribute in a 
 * thread attribute object.
 */
int pthread_attr_setscope(attr,contentionscope)
     pthread_attr_t *attr;
     int contentionscope;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
  if (contentionscope == PTHREAD_SCOPE_LOCAL) {
    attr->contentionscope=contentionscope;
    return(0);    
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setinheritsched - Set the inheritsched attribute.
 */
int pthread_attr_setinheritsched(attr, inherit)
     pthread_attr_t *attr;
     int inherit;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
  if (inherit == PTHREAD_INHERIT_SCHED || inherit == PTHREAD_DEFAULT_SCHED) {
    attr->inheritsched = inherit;
    return(0);    
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setsched - set the sched attribute
 */
int pthread_attr_setsched(attr, scheduler)
     pthread_attr_t *attr;
     int scheduler;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

#ifdef DEF_RR
  if (scheduler == SCHED_FIFO || scheduler == SCHED_RR) {
#else
  if (scheduler == SCHED_FIFO) {
#endif
    attr->sched = scheduler;
    return(0);    
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setprio - set the prio attribute
 */
int pthread_attr_setprio(attr, priority)
     pthread_attr_t *attr;
     int priority;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  if (priority >= MIN_PRIORITY && priority <= MAX_PRIORITY) {
    attr->prio=priority;
    return(0);    
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getscope - get the contentionscope attribute
 */
int pthread_attr_getscope(attr)
     pthread_attr_t attr;
{
  if (!attr.flags) {
    set_errno(EINVAL);
    return(-1);
  }
  return(attr.contentionscope);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getinheritsched - set the inheritsched attribute
 */
int pthread_attr_getinheritsched(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
  return(attr->inheritsched);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getsched - get the sched attribute
 */
int pthread_attr_getsched(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  return(attr->sched);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getprio - get the prio attribute
 */
int pthread_attr_getprio(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }

  return(attr->prio);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setstarttime_np - set the absolute start time
 */
int pthread_attr_setstarttime_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp)) {
    set_errno(EINVAL);
    return(-1);
  }

  attr->starttime.tv_sec = tp->tv_sec;
  attr->starttime.tv_nsec = tp->tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getstarttime_np - get the absolute start time
 */
int pthread_attr_getstarttime_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->starttime)) {
    set_errno(EINVAL);
    return(-1);
  }

  tp->tv_sec = attr->starttime.tv_sec;
  tp->tv_nsec = attr->starttime.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setdeadline_np - set the absolute deadline (XOR period)
 */
int pthread_attr_setdeadline_np(attr, tp, func)
     pthread_attr_t *attr;
     struct timespec *tp;
     pthread_func_t func;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  extern struct sigaction pthread_user_handler[NNSIG];

  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp) ||
      ISNTIMERSET(attr->period)) {
    set_errno(EINVAL);
    return(-1);
  }

  attr->deadline.tv_sec = tp->tv_sec;
  attr->deadline.tv_nsec = tp->tv_nsec;
  sigaddset(&handlerset, TIMER_SIG);
  pthread_user_handler[TIMER_SIG].sa_handler = (void(*)()) func;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getdeadline_np - get the absolute deadline
 */
int pthread_attr_getdeadline_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->deadline)) {
    set_errno(EINVAL);
    return(-1);
  }

  tp->tv_sec = attr->deadline.tv_sec;
  tp->tv_nsec = attr->deadline.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setperiod_np - set the relative period (XOR deadline)
 */
int pthread_attr_setperiod_np(attr, tp, func)
     pthread_attr_t *attr;
     struct timespec *tp;
     pthread_func_t func;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  extern struct sigaction pthread_user_handler[NNSIG];

  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp) ||
      ISNTIMERSET(attr->deadline)) {
    set_errno(EINVAL);
    return(-1);
  }

  attr->period.tv_sec = tp->tv_sec;
  attr->period.tv_nsec = tp->tv_nsec;
  sigaddset(&handlerset, TIMER_SIG);
  pthread_user_handler[TIMER_SIG].sa_handler = (void(*)()) func;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getperiod_np - get the relative period
 */
int pthread_attr_getperiod_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  set_errno(ENOSYS);
  return(-1);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->period)) {
    set_errno(EINVAL);
    return(-1);
  }

  tp->tv_sec = attr->period.tv_sec;
  tp->tv_nsec = attr->period.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_key_create - creates a new global key and spefies destructor call
 * returns -1 upon error with errno ENOMEM if name space exhausted,
 *                                  EAGAIN if insufficient memory.
 */
int pthread_key_create(key, destructor)
     pthread_key_t *key;
     void (*destructor)();
{
  SET_KERNEL_FLAG;

  if (n_keys >= _POSIX_DATAKEYS_MAX) {
    CLEAR_KERNEL_FLAG;
    set_errno(ENOMEM);
    return(-1);
  }

  key_destructor[n_keys] = destructor;
  *key = n_keys++;
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_setspecific - associate a value with a data key for some thread
 * returns -1 upon error with errno EINVAL if the key is invalid.
 */
int pthread_setspecific(key, value)
     pthread_key_t key;
     any_t value;
{
  if (key < 0 || key >= n_keys) {
    set_errno(EINVAL);
    return(-1);
  }

  mac_pthread_self()->key[key] = value;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_getspecific - retrieve a value from a data key for some thread
 * returns -1 upon error with errno EINVAL if the key is invalid.
 */
int pthread_getspecific(key, value)
     pthread_key_t key;
     any_t *value;
{
  if (key < 0 || key >= n_keys) {
    set_errno(EINVAL);
    return(-1);
  }

  *value = mac_pthread_self()->key[key];
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_cleanup_push - push function on current thread's cleanup stack
 * and execute it with the specified argument when the thread.
 * Notice: pthread_cleanup_push_body() receives the address of the first
 * cleanup structure in an additional parameter "new".
 * (a) exits;
 * (b) is cancelled;
 * (c) calls pthread_cleanup_pop(0 with a non-zero argument;
 * (d) NOT IMPLEMENTED, CONTROVERSIAL: when a longjump reaches past the scope.
 */
#ifdef CLEANUP_HEAP
int pthread_cleanup_push(func, arg)
#else
int pthread_cleanup_push_body(func, arg, new)
#endif
     void (*func)();
     any_t arg;
#ifdef CLEANUP_HEAP
{
  cleanup_t new;
#else
     cleanup_t new;
{
#endif
  pthread_t p = mac_pthread_self();

  if (!func) {
    set_errno(EINVAL);
    return(-1);
  }

#ifdef CLEANUP_HEAP
  if (!(new = (cleanup_t) malloc(sizeof(*new)))) {
    set_errno(ENOMEM);
    return(-1);
  }
#endif

  new->func = func;
  new->arg = arg;

  SET_KERNEL_FLAG;
  new->next = p->cleanup_top;
  p->cleanup_top = new;
  CLEAR_KERNEL_FLAG;

  return(0);
}


/*------------------------------------------------------------*/
/*
 * pthread_cleanup_pop - pop function off current thread's cleanup stack
 * and execute it with the specified argument if non-zero
 */
#ifdef CLEANUP_HEAP
int pthread_cleanup_pop(execute)
#else
int pthread_cleanup_pop_body(execute)
#endif
     int execute;
{
  pthread_t p = mac_pthread_self();
  cleanup_t new;

  SET_KERNEL_FLAG;
  if (!(new = p->cleanup_top)) {
    CLEAR_KERNEL_FLAG;
    set_errno(EINVAL);
    return(-1);
  }
  p->cleanup_top = new->next;
  CLEAR_KERNEL_FLAG;

  if (execute)
    (new->func)(new->arg);
#ifdef CLEANUP_HEAP
  free(new);
#endif

  return(0);
}

/*------------------------------------------------------------*/
/*
 * sched_get_priority_max - inquire maximum priority value (part of .4)
 */
int sched_get_priority_max(policy)
     int policy;
{
  return(MAX_PRIORITY);
}

/*------------------------------------------------------------*/
/*
 * sched_get_priority_min - inquire minimum priority value (part of .4)
 */
int sched_get_priority_min(policy)
     int policy;
{
  return(MIN_PRIORITY);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setdetachstate - Sets the detachstate attribute in 
 *                               attr object
 */
int pthread_attr_setdetachstate(attr, detachstate)
     pthread_attr_t *attr;
     int *detachstate;
{
  if (!attr || !attr->flags || (*detachstate > 1) || (*detachstate < 0)) {
    set_errno(EINVAL);
    return(-1);
  }
  attr->detachstate = *detachstate;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getdetachstate - Gets the value of detachstate attribute
 *                               from attr object
 */
int pthread_attr_getdetachstate(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) { 
    set_errno(EINVAL);
    return(-1);    
  } 
  return(attr->detachstate); 
}

/*------------------------------------------------------------*/
/*
 * pthread_once - The first call to this will call the init_routine()
 *                with no arguments. Subsequent calls to pthread_once()
 *                will not call the init_routine.
 */
int pthread_once(once_control, init_routine)
     pthread_once_t *once_control;
     void (*init_routine)();
{
  SET_KERNEL_FLAG;
  if (!once_control->init) {
    once_control->init = TRUE;
    pthread_mutex_init(&once_control->mutex, NULL);
  }
  CLEAR_KERNEL_FLAG;

  pthread_mutex_lock(&once_control->mutex);
  if (!once_control->exec) {
    once_control->exec = TRUE;
    (*init_routine)();
  }
  pthread_mutex_unlock(&once_control->mutex);
  return(0);
}

#if defined(MALLOC) || defined(STAND_ALONE)

/*------------------------------------------------------------*/
malloc_t malloc (size)
     size_t size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_malloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = pthread_malloc(size);
  CLEAR_KERNEL_FLAG;
  return(ret);
}

/*------------------------------------------------------------*/
#if defined (__cplusplus) || defined (__STDC__)
void free(ptr)
#else
int free (ptr)
#endif
     malloc_t ptr;
{
#if !defined (__FreeBSD__)
  int ret;
  extern int pthread_free();
#endif

  SET_KERNEL_FLAG;
#if !(defined (__cplusplus) || defined (__STDC__))
  ret = pthread_free(ptr);
#else
  pthread_free(ptr);
#endif
  CLEAR_KERNEL_FLAG;
#if !(defined (__cplusplus) || defined (__STDC__))
  return(ret);
#endif
}

#endif /* MALLOC || STAND_ALONE */

#ifdef MALLOC

/*------------------------------------------------------------*/
malloc_t realloc (ptr, size)
     malloc_t ptr;
     size_t size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_realloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = pthread_realloc(ptr, size);
  CLEAR_KERNEL_FLAG;
  return(ret);
}

/*------------------------------------------------------------*/
malloc_t calloc (nmemb, elsize)
     size_t nmemb, elsize;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_calloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (elsize == 0) 
    elsize = 1;
  if (nmemb == 0)
    nmemb = 1;

  SET_KERNEL_FLAG;
  ret = pthread_calloc(nmemb, elsize);
  CLEAR_KERNEL_FLAG;
  return(ret);
}

/*------------------------------------------------------------*/
void cfree (ptr)
     malloc_t ptr;
{
#if !defined (__FreeBSD__)
  extern pthread_cfree();
#endif

  SET_KERNEL_FLAG;
  pthread_cfree(ptr);
  CLEAR_KERNEL_FLAG;
}

/*------------------------------------------------------------*/
malloc_t memalign(alignment, size)
     size_t alignment, size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_memalign();
#endif

  SET_KERNEL_FLAG;
  ret = pthread_memalign(alignment, size);
  CLEAR_KERNEL_FLAG;
  return(ret);
}

/*------------------------------------------------------------*/
malloc_t valloc(size)
     size_t size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_valloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = pthread_valloc(size);
  CLEAR_KERNEL_FLAG;
  return(ret);
}

#endif /* MALLOC */

/*------------------------------------------------------------*/

#ifdef IO
/* foo - The following is intentionally dead code since foo is never called.
 * It ensures that the new versions of read and write are always linked in.
 */
static void pthread_dead_code()
{
  char *buf;

  READ(0, buf, 0);
  WRITE(1, buf, 0);
}
#endif /* IO */
