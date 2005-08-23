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

  @(#)mutex.c	3.1 10/11/96

*/

/* 
 * Functions for the support of mutual exclusion - mutexes and their 
 * attributes.  
 */

#include "internals.h"
#include "mutex.h"

/*------------------------------------------------------------*/
/*
 * pthread_mutex_lock - Checks are made to see if the mutex is 
 * currently in use or not.  If the mutex is not in use, then its 
 * locked. Otherwise the currently executing thread is put in the 
 * wait queue and a new thread is selected. (Fast mutex_lock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_lock(mutex)
#else
int pthread_mutex_lock(mutex)
#endif
     pthread_mutex_t *mutex;
{
  register pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX) {
    set_errno(EINVAL);
    return(-1);
  }

  if (mutex->owner == p){
    set_errno(EDEADLK);
    return(-1);
  }

#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PRIO_PROTECT && p->attr.prio > mutex->prio_ceiling) {
    set_errno(EINVAL);
    return(-1);
  }
#endif /* _POSIX_THREADS_PRIO_PROTECT */
#endif /* !NOERR_CHECK */

  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->lock);
  mac_mutex_lock(mutex, p);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_trylock - Just try to lock the mutex. If 
 * lock succeeds return. Otherwise return right away without 
 * getting the lock. (Fast mutex_trylock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_trylock(mutex)
#else
int pthread_mutex_trylock(mutex)
#endif
     pthread_mutex_t *mutex;
{

pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX) {
    set_errno(EINVAL);
    return(-1);
  }

#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PRIO_PROTECT && p->attr.prio > mutex->prio_ceiling) {
    set_errno(EINVAL);
    return(-1);
  }
#endif /* _POSIX_THREADS_PRIO_PROTECT */
#endif /* !NOERR_CHECK */
 
  SET_KERNEL_FLAG;
  if (mutex->lock) {
    CLEAR_KERNEL_FLAG;
    set_errno(EBUSY);
    return(-1);
  }
 
  mutex->lock = TRUE;
  mutex->owner = p;
#ifdef NOERR_CHECK 
  mac_change_lock_prio(mutex, p); 
#else 
#ifdef _POSIX_THREADS_PRIO_PROTECT 
 if (mutex->protocol == PRIO_PROTECT) 
   mac_change_lock_prio(mutex, p);
#endif                  
#endif
 
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_unlock - Called by the owner of mutex to release
 * the mutex. (Fast mutex_unlock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_unlock(mutex)
#else
int pthread_mutex_unlock(mutex)
#endif
     pthread_mutex_t *mutex;
{
pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX || mutex->owner != mac_pthread_self()) {
    set_errno(EINVAL);
    return(-1);
  }
#endif

  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->queue.head != mutex->queue.tail);
  mac_mutex_unlock(mutex, p,  /* NULL */);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_init - Initialize the mutex and at the same time 
 * ensure that it's usaable.  Set the attribute values from attr 
 * specified. No check is made to see if the attributes are right 
 * as yet.
 */
int pthread_mutex_init(mutex, attr)
     pthread_mutex_t *mutex;
     pthread_mutexattr_t *attr;
{
  if (mutex == NO_MUTEX) {
    set_errno(EINVAL);
    return(-1);
  }

  if (!attr)
    attr = &pthread_mutexattr_default;

  mutex->owner = NO_PTHREAD;
  mutex->flags = attr->flags;
  pthread_queue_init(&mutex->queue);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  mutex->prio_ceiling = attr->prio_ceiling;
  mutex->protocol = attr->protocol;
#endif
  mutex->lock = FALSE;
  return(0);
}  

/*------------------------------------------------------------*/
/*
 * pthread_mutex_destroy - Destroys the mutex.
 */
int pthread_mutex_destroy(mutex) 
     pthread_mutex_t *mutex;
{
  if (mutex == NO_MUTEX) {
    set_errno(EINVAL);
    return(-1);
  }

  /*
   * free mutex only if not locked and not associated with any cond var
   */
#ifdef NOERR_CHECK
  if (pthread_test_and_set(&mutex->lock)) {
    set_errno(EBUSY);
    return(-1);
  }
#else /* !NOERR_CHECK */
  SET_KERNEL_FLAG;
  if (mutex->lock) {
    CLEAR_KERNEL_FLAG;
    set_errno(EBUSY);
    return(-1);
  }
  CLEAR_KERNEL_FLAG;
#endif /* !NOERR_CHECK */

  mutex->flags = 0;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_setprio_ceiling - locks the mutex, changes the
 * mutex's priority ceiling and releases the mutex.
 */
int pthread_mutex_setprio_ceiling(mutex, prio_ceiling)
pthread_mutex_t *mutex;
int prio_ceiling;
{
int oldprio;

  if (mutex == NO_MUTEX || !mutex->flags) {
    set_errno(ESRCH);
    return(-1);
  }
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (prio_ceiling >= MIN_PRIORITY && prio_ceiling <= MAX_PRIORITY) {
    if (!pthread_mutex_trylock(mutex)) {
      oldprio = mutex->prio_ceiling;
      mutex->prio_ceiling = prio_ceiling;
      pthread_mutex_unlock(mutex);
      return(oldprio);
    }
    else {
      set_errno(EPERM);
      return(-1);
    }
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
#else
  set_errno(ENOTSUP);
  return(-1);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_getprio_ceiling - Returns the current priority
 * ceiling of the mutex.
 */
int pthread_mutex_getprio_ceiling(mutex)
pthread_mutex_t mutex;
{

  if (!mutex.flags) {
    set_errno(ESRCH);
    return(-1);
  }
  
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex.prio_ceiling >= MIN_PRIORITY &&
      mutex.prio_ceiling <= MAX_PRIORITY)
    return(mutex.prio_ceiling);
  else {
    set_errno(EINVAL);
    return(-1);
  }
#else
  set_errno(ENOTSUP);
  return(-1);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_init - Initializes the mutex attribute object
 * with default values.
 */
int pthread_mutexattr_init(attr)
pthread_mutexattr_t *attr;
{
  if (!attr)
  {
    set_errno(EINVAL);
    return(-1);
  }

  attr->flags = TRUE;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  attr->prio_ceiling = DEFAULT_PRIORITY;
  attr->protocol = NO_PRIO_INHERIT;
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_destroy - Destroys the mutex attribute object.
 */
int pthread_mutexattr_destroy(attr)
pthread_mutexattr_t *attr;
{

  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
  attr->flags = FALSE;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  attr->prio_ceiling = DEFAULT_PRIORITY;
  attr->protocol = NO_PRIO_INHERIT;
#endif  
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setprotocol - Sets the protocol (value can be
 * NO_PRIO_INHERIT, PRIO_INHERIT or PRIO_PROTECT) for the mutex attr.
 */
int pthread_mutexattr_setprotocol(attr,protocol)
pthread_mutexattr_t *attr;
pthread_protocol_t protocol;
{
  
  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
#ifndef _POSIX_THREADS_PRIO_PROTECT
  set_errno(ENOSYS);
  return(-1);
#else
  if (protocol < NO_PRIO_INHERIT || protocol > PRIO_PROTECT) {
    set_errno(EINVAL);
    return(-1);
  }
  if (protocol == PRIO_INHERIT) {
    set_errno(ENOTSUP);
    return (-1);
  }
  attr->protocol = protocol;
  return(0);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getprotocol - Gets the current protcol.
 */
pthread_protocol_t pthread_mutexattr_getprotocol(attr)
pthread_mutexattr_t attr;
{
  if (!attr.flags) {
    set_errno(EINVAL);
    return(-1);
  }
#ifdef _POSIX_THREADS_PRIO_PROTECT
  return(attr.protocol);
#else
  set_errno(ENOSYS);
  return(-1);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setprio_ceiling - Sets the priority ceiling
 * for the mutex attribute object.
 */
int pthread_mutexattr_setprio_ceiling(attr,prio_ceiling)
pthread_mutexattr_t *attr;
int prio_ceiling;
{

  if (!attr || !attr->flags) {
    set_errno(EINVAL);
    return(-1);
  }
#ifndef _POSIX_THREADS_PRIO_PROTECT
  set_errno(ENOSYS);
  return(-1);
#else
  if (prio_ceiling > MAX_PRIORITY || prio_ceiling < MIN_PRIORITY) {
    set_errno(EINVAL);
    return(-1);
  }
  attr->prio_ceiling = prio_ceiling;
  return(0);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getprio_ceiling - Gets the current priority
 * ceiling of the mutex attribute object.
 */
int pthread_mutexattr_getprio_ceiling(attr)
pthread_mutexattr_t attr;
{
  if (!attr.flags) {
    set_errno(EINVAL);
    return(-1);
  }
#ifdef _POSIX_THREADS_PRIO_PROTECT
  return(attr.prio_ceiling);
#else
  set_errno(ENOSYS);
  return(-1);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getpshared - Not Implemented. Returns ENOSYS.
 */
 
int pthread_mutexattr_getpshared(attr, pshared)
pthread_mutexattr_t *attr;
int *pshared;
{
  set_errno(ENOSYS);
  return(-1);
}
 
/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setpshared - Not Implemented. Returns ENOSYS.
 */
 
int pthread_mutexattr_setpshared(attr, pshared)
pthread_mutexattr_t *attr;
int pshared;
{
  set_errno(ENOSYS);
  return(-1);
}
