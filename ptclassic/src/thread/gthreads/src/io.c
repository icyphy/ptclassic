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

  @(#)io.c	3.1 10/11/96
  
*/

#ifdef IO

#include <pthread/config.h>

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)

#include "signal_internals.h"
#include "internals.h"
#include "fd.h"
#include <fcntl.h>
#include <sys/uio.h>

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

#ifndef ssize_t
#define ssize_t int
#endif

#else /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */


#include "internals.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*
 * aioread/write may cause a stack overflow in the UNIX kernel which cannot
 * be caught by sighandler. This seems to be a bug in SunOS. We get
 * around this problem by deliberately trying to access a storage
 * location on stack about a page ahead of where we are. This will
 * cause a premature stack overflow (SIGBUS) which *can* be caught
 * by sighandler.
 */
  extern int pthread_page_size;
#define ACCESS_STACK \
  MACRO_BEGIN \
    if (*(int *) (pthread_get_sp() - pthread_page_size)) \
      ; \
  MACRO_END

#else /* !STACK_CHECK || !SIGNAL_STACK */
#define ACCESS_STACK
#endif /* !STACK_CHECK || !SIGNAL_STACK */

#endif /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(SVR4)
/*------------------------------------------------------------*/
/*
 * pthread_fds_set - initialization of file descriptor
 */
void pthread_fds_set(l, r)
     fd_set *l, *r;
{
  if (l)
    if (r)
      *l = *r;
    else 
      FD_ZERO(l);
}

/*------------------------------------------------------------*/
/*
 * pthread_fds_union - bit-wise union of file descriptor fields
 */
void pthread_fds_union(l, r, width)
     fd_set *l,*r;
     int width;
{
  int i;

  for (i = 0; i < howmany(width, NFDBITS); i++)
    (*l).fds_bits [i] = (*l).fds_bits [i] | (*r).fds_bits [i];
}

/*------------------------------------------------------------*/
/*
 * accept - Same as BSD accept except that it blocks only the current thread
 * rather than entire process.
 */
int accept(s, addr, addrlen)
     int s;
     struct sockaddr *addr;
     int *addrlen;
{
  int mode;
  struct iovec iov[1];
  pthread_t p;
  struct timeval timeout;
  int result;

  /*
   * If the mode is O_NDELAY perform a Non Blocking accept & return 
   * immediately.
   */
  if ((mode = fcntl(s, F_GETFL, 0)) & (O_NDELAY|O_NONBLOCK)) {
    return (ACCEPT(s, addr, addrlen));
  }


#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
  ACCESS_STACK;
#endif

  /*
   * Else issue an asynchronous request
   */

  mode = fcntl(s, F_GETFL, 0);
  if (fcntl(s, F_SETFL, (mode|FNDELAY|FASYNC)) < 0)
    return (-1);
  if (fcntl(s, F_SETOWN, getpid()) < 0) {
    fcntl (s, F_SETFL, mode);
    return(-1);
  }

  p = mac_pthread_self();
  SET_KERNEL_FLAG; /* No preemption */

  while (TRUE) {
#ifdef DEBUG
    printf("Try to accept\n");
#endif
    result = ACCEPT(s, addr, addrlen);
    if (result != -1) {
      CLEAR_KERNEL_FLAG;
      if ((fcntl(result, F_SETFL, mode) != -1) &&
          (fcntl(s, F_SETFL, mode) != -1))
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      CLEAR_KERNEL_FLAG;
      fcntl(s, F_SETFL, mode);
      return(-1);
    };
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}
#endif /* !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(SVR4) */

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__)
volatile int fd_started = FALSE;
bin_sem_t fd_server_sem;
fd_queue_t fd_wait_read, fd_wait_write;
static fd_set fd_set_read, fd_set_write;

static struct timespec sleeptime = {0, 100000000}; /* 100ms */

/*
 *  bin_sem_init -
 */
void bin_sem_init(bin_sem_t *s)
{
  pthread_mutex_init(&s->lock, NULL);
  pthread_cond_init(&s->cond, NULL);
  s->flag = FALSE;
}

/*
 *  count_sem_init -
 */
void count_sem_init(count_sem_t *s)
{
  pthread_mutex_init(&s->lock, NULL);
  pthread_cond_init(&s->cond, NULL);
  s->count = 0;
}

/*
 *  fd_enq -
 */
fd_queue_elem_t *fd_enq (fd_queue_t q,
                              int fd)
{
  fd_queue_elem_t *new_elem;

  for (new_elem = q->head; new_elem; new_elem = new_elem->next) {
    if (new_elem->fd == fd) {
      new_elem->count++;
      return(new_elem);
    }
  }

  new_elem = (fd_queue_elem_t *) malloc (sizeof(fd_queue_elem_t));

  bin_sem_init(&new_elem->sem);
  new_elem->fd   = fd;
  new_elem->next = q->head;

  if ((q->head) == NULL) {
    q->tail = new_elem;
  }
  q->head = new_elem;
  new_elem->count = 1;
  return(new_elem);
}  

/*
 *  fd_deq -
 */
void fd_deq_elem (fd_queue_t q,
                  fd_queue_elem_t *q_elem)
{
  fd_queue_elem_t *prev = NULL;
  fd_queue_elem_t *next = q->head;
  
  while (next != NULL && next != q_elem) {
    prev = next;
    next = next->next;
  }

  if (next == NULL) {
#ifdef DEBUG
    fprintf(stderr, "fd_deq_elem(elem %x) failed\n", q_elem);
#endif
    return;
  }

  /*
   * other threads still waiting on this element
   */
  if (--next->count != 0)
    return;

  if (prev == NULL) {
    q->head = next->next;
  } else {
    prev->next = next->next;
  }

  if (next->next == NULL) {
    q->tail = prev;
  }
  /* cannot do free(next) since semaphore still used */
}

/*
 *  fd_server - 
 */
void * fd_server (void* arg)
{
  struct timeval    timeout = { 0, 0 };
  struct timespec   state_changed_timeout;
  fd_queue_elem_t   *q_elem, *q_elem_tmp;   
  int               count, ret;

  while (TRUE) {
    if (fd_wait_read->head || fd_wait_write->head) {

      FD_ZERO(&fd_set_read);
      FD_ZERO(&fd_set_write);

      pthread_mutex_lock (&fd_server_sem.lock);
      
      for (q_elem = fd_wait_read->head; 
           q_elem; 
           q_elem = q_elem->next){
        FD_SET(q_elem->fd, &fd_set_read);
      }
      for (q_elem = fd_wait_write->head; 
           q_elem; 
           q_elem  = q_elem->next) {
        FD_SET(q_elem->fd, &fd_set_write);
      }
      pthread_mutex_unlock (&fd_server_sem.lock);

      /* test, if some of the filedescriptors in fd_set__read/ fd_set_write
       * are ready, timeout immediately
       */
      while ((count = select(FD_SETSIZE, &fd_set_read, &fd_set_write, NULL, 
                          &timeout)) < OK) {
        if (errno != EINTR)
          abort();
      }
      
      pthread_mutex_lock(&fd_server_sem.lock);
      q_elem = fd_wait_read->head;
      while (q_elem && count) {
        
        if (FD_ISSET(q_elem->fd, &fd_set_read)) {
          FD_CLR(q_elem->fd, &fd_set_read);

          q_elem_tmp = q_elem->next;

          pthread_mutex_lock (&(q_elem->sem.lock));
          q_elem->sem.flag = TRUE;
          fd_deq_elem (fd_wait_read, q_elem);
          pthread_mutex_unlock (&(q_elem->sem.lock));          
          pthread_cond_signal (&(q_elem->sem.cond));

          q_elem = q_elem_tmp;
	  count--;
        } else
          q_elem = q_elem->next;
      }
      
      q_elem = fd_wait_write->head;
      while (q_elem && count) {
        
        if (FD_ISSET(q_elem->fd, &fd_set_write)) {
          FD_CLR(q_elem->fd, &fd_set_write);

          q_elem_tmp = q_elem->next;

          pthread_mutex_lock (&(q_elem->sem.lock));
          q_elem->sem.flag = TRUE;
          fd_deq_elem (fd_wait_write, q_elem);
          pthread_mutex_unlock (&(q_elem->sem.lock));          
          pthread_cond_signal (&(q_elem->sem.cond));

          q_elem = q_elem_tmp;
	  count--;
        } else
          q_elem = q_elem->next;
      }
      pthread_mutex_unlock(&fd_server_sem.lock);
    }
        
    pthread_mutex_lock(&fd_server_sem.lock);
    if (fd_wait_read->head || fd_wait_write->head) {
          clock_gettime (CLOCK_REALTIME, &state_changed_timeout);
      PLUS_NTIME(state_changed_timeout, state_changed_timeout, sleeptime);

      ret = 0;
      while (!fd_server_sem.flag && ret != -1) {
        ret = pthread_cond_timedwait(&fd_server_sem.cond,
                                     &fd_server_sem.lock,
                                     &state_changed_timeout);
      }
    }  else {
      /* since fd_read and fd_write are empty, wait, until
       * one of them has changed
       */
      while (!fd_server_sem.flag) {
        pthread_cond_wait(&fd_server_sem.cond,
                          &fd_server_sem.lock);
      }
    }
    fd_server_sem.flag = FALSE;
    pthread_mutex_unlock(&fd_server_sem.lock);
  }
}

/*
 *  fd_init - 
 */
void fd_init(void)
{
  int i;
  pthread_t server_thread;
  void * args;
  pthread_attr_t attrs;

  if (fd_started)
    return;

  fd_started = TRUE;

  bin_sem_init(&fd_server_sem);

  fd_wait_read  = (fd_queue_t) malloc (sizeof (struct fd_queue));
  fd_wait_read->head = NULL;
  fd_wait_read->tail = NULL;

  fd_wait_write = (fd_queue_t) malloc (sizeof (struct fd_queue));
  fd_wait_write->head = NULL;
  fd_wait_write->tail = NULL;

  pthread_attr_init( &attrs );
  if (!pthread_attr_setprio(&attrs, sched_get_priority_max(0)))
    pthread_create(&server_thread, &attrs, fd_server, (void *) args);     
}

/*
 * read - 
 */
ssize_t read(int    fd, 
             void   *buf, 
             size_t nbytes)
{
  int ret, last;
  int flags, blocking;
  fd_queue_elem_t *q_elem;
  extern int pthread_sys_read();

  flags = fcntl(fd, F_GETFL, 0);
  if (flags == NOTOK) {
    return(NOTOK);
  }
  blocking = (flags & O_NONBLOCK);
  if (!blocking)
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < OK) {
      return(NOTOK);
    }

  while ((ret = pthread_sys_read(fd, buf, nbytes)) < OK) {
    if (get_errno() == EWOULDBLOCK || get_errno() == EAGAIN) {
      pthread_mutex_lock (&fd_server_sem.lock);
      q_elem = fd_enq (fd_wait_read, fd);
      fd_server_sem.flag = TRUE;
      pthread_mutex_unlock (&fd_server_sem.lock);
      pthread_cond_signal (&fd_server_sem.cond);

      pthread_mutex_lock(&(q_elem->sem.lock));
      while (!(q_elem->sem.flag)) {
        pthread_cond_wait (&(q_elem->sem.cond), &(q_elem->sem.lock));
      }
      q_elem->sem.flag = FALSE;
      last = (q_elem->count == 0);
      pthread_mutex_unlock(&(q_elem->sem.lock));
      if (last)
        free(q_elem); /* q_elem semaphore used up until here */
    }
    else
      return(NOTOK);
  }      

  if (!blocking)
    if (fcntl(fd, F_SETFL, flags) < OK) {
      return(NOTOK);
    }

  return(ret);
}

/*
 * write - 
 */
ssize_t write(int        fd, 
              const void *buf, 
              size_t     nbytes)
{
  int ret, last;
  int flags, blocking;
  fd_queue_elem_t *q_elem;
  extern int pthread_sys_write();

  flags = fcntl(fd, F_GETFL, 0);
  if (flags == NOTOK) {
    return(NOTOK);
  }
  blocking = (flags & O_NONBLOCK);
  if (!blocking)
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < OK) {
      return(NOTOK);
    }

  while ((ret = pthread_sys_write(fd, buf, nbytes)) < OK) {
    if (get_errno() == EWOULDBLOCK || get_errno() == EAGAIN) {
      pthread_mutex_lock (&fd_server_sem.lock);
      q_elem = fd_enq (fd_wait_write, fd);
      fd_server_sem.flag = TRUE;
      pthread_mutex_unlock (&fd_server_sem.lock);
      pthread_cond_signal (&fd_server_sem.cond);

      pthread_mutex_lock(&(q_elem->sem.lock));
      while (!(q_elem->sem.flag)) {
        pthread_cond_wait (&(q_elem->sem.cond), &(q_elem->sem.lock));
      }
      q_elem->sem.flag = FALSE;
      last = (q_elem->count == 0);
      pthread_mutex_unlock(&(q_elem->sem.lock));
      if (last)
        free(q_elem); /* q_elem semaphore used up until here */
    }
    else
      return(NOTOK);
  }      

  if (!blocking)
    if (fcntl(fd, F_SETFL, flags) < OK)
      return(NOTOK);

  return(ret);
}

#else /* !(__FreeBSD__ || _M_UNIX || __linux__) */

/*------------------------------------------------------------*/
/*
 * read - Same as POSIX.1 read except that it blocks only the current thread
 * rather than entire process.
 */
int READ(fd, buf, nbytes)
     int fd;
     void *buf;
     IO_SIZE_T nbytes;
{
  int mode;
  struct iovec iov[1];
  pthread_t p;
  struct timeval timeout;


  /*
   * If the mode is O_NDELAY perform a Non Blocking read & return immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY|O_NONBLOCK)) {
    iov[0].iov_base = buf;
    iov[0].iov_len = nbytes;
    return(readv(fd, iov, 1));
  }

  ACCESS_STACK;

  /*
   * Else issue an asynchronous request for nbytes.
   */
  timeout.tv_sec        = 0;
  timeout.tv_usec       = 0;
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
  p->resultp.aio_return = AIO_INPROGRESS;

  if (aioread(fd, buf, nbytes, 0l, SEEK_CUR,
	      (struct aio_result_t *) &p->resultp) < 0) {
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  if (p->resultp.aio_return != AIO_INPROGRESS) {
    if (p->resultp.aio_return != -1)
      lseek(fd, p->resultp.aio_return, SEEK_CUR);
    else
      set_errno (p->resultp.aio_errno);
    p->state |= T_IO_OVER;
    CLEAR_KERNEL_FLAG;
    aiowait(&timeout);
    return(p->resultp.aio_return);
  }
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) && !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * The case when the read() is interrupted (when the thread receives a
   * signal other than SIGIO), read() returns -1 and errno is set to EINTR
   * (this is done in signal.c when the thread is woken up).
   */
  switch (p->resultp.aio_return) {
  case AIO_INPROGRESS:
    aiocancel((struct aio_result_t *) &p->resultp);
    return (-1);
  case -1:
    aiowait(&timeout);
    set_errno (p->resultp.aio_errno);
    return (-1);
  default:
    aiowait(&timeout);
    lseek(fd, p->resultp.aio_return, SEEK_CUR);
    return(p->resultp.aio_return);
  }
}

/*------------------------------------------------------------*/
/*
 * write - Same as POSIX.1 write except that it blocks only the current
 * thread rather than entire process.
 */
int WRITE(fd, buf, nbytes)
     int fd;
     const void *buf;
     IO_SIZE_T nbytes;
{

  int mode;
  struct iovec iov[1];
  pthread_t p;
  struct timeval timeout;

  /*
   * If the mode is O_NDELAY perform a Non Blocking write & return immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY|O_NONBLOCK)) {
    iov[0].iov_base = (caddr_t) buf;
    iov[0].iov_len = nbytes;
    return (writev(fd, iov, 1));
  }

  ACCESS_STACK;

  /*
   * Else issue an asynchronous request for nbytes.
   */
  timeout.tv_sec        = 0;
  timeout.tv_usec       = 0;
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
  p->resultp.aio_return = AIO_INPROGRESS;

  if (aiowrite(fd, (char *) buf, nbytes, 0l, SEEK_CUR,
	       (struct aio_result_t *) &p->resultp) < 0) {
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  if (p->resultp.aio_return != AIO_INPROGRESS) {
    if (p->resultp.aio_return != -1)
      lseek(fd, p->resultp.aio_return, SEEK_CUR);
    else
      set_errno(p->resultp.aio_errno);
    p->state |= T_IO_OVER;
    CLEAR_KERNEL_FLAG;
    aiowait(&timeout);
    return(p->resultp.aio_return);
  }
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT; 
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
  switch (p->resultp.aio_return) {
  case AIO_INPROGRESS:
    aiocancel((struct aio_result_t *) &p->resultp);
    return(-1);
  case -1:
    aiowait(&timeout);
    set_errno (p->resultp.aio_errno);
    return(-1);
  default:
    aiowait(&timeout);
    lseek(fd, p->resultp.aio_return, SEEK_CUR);
    return(p->resultp.aio_return);
  }
}

#ifdef TRASH
/*------------------------------------------------------------*/
/*
 * select - Same as BSD select except that it blocks only the current thread
 * rather than entire process.
 */
int select(width, readfds, writefds, exceptfds, timeout)
     int width;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
{
  static struct timeval mintimeout = {0, 0};
  pthread_t p;
  int result;
  struct timespec p_timeout;
  
  ACCESS_STACK;
  
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
  
  pthread_fds_set(&(p->readfds), readfds);
  pthread_fds_set(&(p->writefds), writefds);
  pthread_fds_set(&(p->exceptfds), exceptfds); 
  
  result = SELECT(width, readfds, writefds, exceptfds, &mintimeout);
  if (result != 0) {
    CLEAR_KERNEL_FLAG;
    return result;
  }
  
  p->width = width;
  gwidth = MAX(gwidth, width);
  pthread_fds_union(&greadfds, &(p->readfds), width);
  pthread_fds_union(&gwritefds, &(p->writefds), width);
  pthread_fds_union(&gexceptfds, &(p->exceptfds), width);
  p->wait_on_select = TRUE;
  
  if (timeout && timerisset(timeout)) {
    U2P_TIME(p_timeout, *timeout);
    if (pthread_timed_sigwait(p, p_timeout, REL_TIME, NULL, NULL) == -1) {
      CLEAR_KERNEL_FLAG;
      return(-1);
    }
    p->state |= T_SYNCTIMER;
  }
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  if (p->wait_on_select)
    return(0);

  pthread_fds_set(readfds, &(p->readfds));  
  pthread_fds_set(writefds, &(p->writefds)); 
  pthread_fds_set(exceptfds, &(p->exceptfds));
  return(p->how_many);
}
#endif /* !SVR4 */

/*------------------------------------------------------------*/
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*
 * pthread_io_end - dummy function used to do pc mapping to check
 *                  stack_overflow.
 */
void pthread_io_end()
{
  return;
}
/*------------------------------------------------------------*/
#endif /* STACK_CHECK && SIGNAL_STACK */

#endif /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */

#endif /* IO */
