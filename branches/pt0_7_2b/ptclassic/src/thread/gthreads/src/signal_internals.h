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

  @(#)signal_internals.h	3.1 10/11/96

*/

#ifndef _pthread_signal_internals_h
#define _pthread_signal_internals_h

#include <pthread/config.h>
#include <../src/config_internals.h>

#if (!defined(__signal_h) && !defined(_SIGNAL_H)) || defined(__linux) || defined(__dos__)

#if !defined(_ASM) && !defined(LOCORE)
#include <pthread/signal.h>
#endif

#ifdef DEBUG
#define TEMPSTACK_SIZE (104*SA(MINFRAME)+SA(WINDOWSIZE))
#else
#define TEMPSTACK_SIZE ( 54*SA(MINFRAME)+SA(WINDOWSIZE))
#endif

#ifdef SIGCANCEL
#undef SIGCANCEL
#endif

#ifdef __dos__
#define SIGCANCEL NSIG-1 /* To get arounnd a bug in djgpp v2 */
#else
#define SIGCANCEL NSIG
#endif

#define DIRECTED_AT_THREAD (struct context_t *) NULL

#define TIME_SLICE 20000

/*
 * MINUS_NTIME only works if src1 > src2
 */
#define MINUS_NTIME(dst, src1, src2) \
  MACRO_BEGIN \
    if ((src2).ts_nsec > (src1).ts_nsec) { \
      (dst).ts_sec = (src1).ts_sec - (src2).ts_sec - 1; \
      (dst).ts_nsec = ((src1).ts_nsec - (src2).ts_nsec) + 1000000000; \
    } \
    else { \
      (dst).ts_sec = (src1).ts_sec - (src2).ts_sec; \
      (dst).ts_nsec = (src1).ts_nsec - (src2).ts_nsec; \
    } \
  MACRO_END

#define PLUS_NTIME(dst, src1, src2) \
  MACRO_BEGIN \
    if (1000000000 - (src2).ts_nsec <= (src1).ts_nsec) { \
      (dst).ts_sec = (src1).ts_sec + (src2).ts_sec + 1; \
      (dst).ts_nsec = -1000000000 + (src1).ts_nsec + (src2).ts_nsec; \
    } \
    else { \
      (dst).ts_sec = (src1).ts_sec + (src2).ts_sec; \
      (dst).ts_nsec = (src1).ts_nsec + (src2).ts_nsec; \
    } \
  MACRO_END

#define GT_NTIME(t1, t2) \
      (t1.ts_sec > t2.ts_sec || \
       (t1.ts_sec == t2.ts_sec && \
	t1.ts_nsec > t2.ts_nsec))

#define GTEQ_NTIME(t1, t2) \
      ((t1).ts_sec > (t2).ts_sec || \
       ((t1).ts_sec == (t2).ts_sec && \
	(t1).ts_nsec >= (t2).ts_nsec))

#define LE0_NTIME(t1) \
      ((t1).ts_sec < 0 || \
       ((t1).ts_sec == 0 && \
	(t1).ts_nsec <= 0))

/*
 * MINUS_TIME only works if src1 > src2
 */
#define MINUS_TIME(dst, src1, src2) \
  MACRO_BEGIN \
    if ((src2).tv_usec > (src1).tv_usec) { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec - 1; \
      (dst).tv_usec = ((src1).tv_usec - (src2).tv_usec) + 1000000; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec; \
      (dst).tv_usec = (src1).tv_usec - (src2).tv_usec; \
    } \
  MACRO_END

#define PLUS_TIME(dst, src1, src2) \
  MACRO_BEGIN \
    if (1000000 - (src2).tv_usec <= (src1).tv_usec) { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec + 1; \
      (dst).tv_usec = -1000000 + (src1).tv_usec + (src2).tv_usec; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec; \
      (dst).tv_usec = (src1).tv_usec + (src2).tv_usec; \
    } \
  MACRO_END

#define GT_TIME(t1, t2) \
      ((t1).tv_sec > (t2).tv_sec || \
       ((t1).tv_sec == (t2).tv_sec && \
	(t1).tv_usec > (t2).tv_usec))

#define GTEQ_TIME(t1, t2) \
      ((t1).tv_sec > (t2).tv_sec || \
       ((t1).tv_sec == (t2).tv_sec && \
	(t1).tv_usec >= (t2).tv_usec))

#define LE0_TIME(t1) \
      ((t1).tv_sec < 0 || \
       ((t1).tv_sec == 0 && \
	(t1).tv_usec <= 0))

#define P2U_TIME(dst, src) \
  MACRO_BEGIN \
  (dst).tv_sec = (src).ts_sec; \
  (dst).tv_usec = (src).ts_nsec / 1000; \
  MACRO_END

#define U2P_TIME(dst, src) \
  MACRO_BEGIN \
  (dst).ts_sec = (src).tv_sec; \
  (dst).ts_nsec = (src).tv_usec * 1000; \
  MACRO_END

#define ISNTIMERSET(tp) ((tp).ts_sec || (tp).ts_nsec)
#define NTIMERCLEAR(tp) (tp).ts_sec = (tp).ts_nsec = 0

#ifdef SOLARIS

#define ILL_STACK 0x00
#include <vm/faultcode.h>
#define BUS_CODE(x) FC_CODE(x)

/*
 * Perform signal manipulation as macros for speed; no error handling!
 * We don't want error handling inside Pthreads since we use one more
 * signal (SIGCANCEL) which would cause any manipulations to fail otherwise.
 */

#define sigmask(n)              ((unsigned int)1 << (((n) - 1) & (32 - 1)))
#define sigword(n)              (((unsigned int)((n) - 1))>>5)

#define sigaddset(s, n)         ((s)->__sigbits[sigword(n)] |= sigmask(n))
#define sigdelset(s, n)         ((s)->__sigbits[sigword(n)] &= ~sigmask(n))
#define sigismember(s, n)       (sigmask(n) & (s)->__sigbits[sigword(n)])

#else /* !SOLARIS */
#ifdef SVR4

/*
 * should get most of the stuff from <signal.h>
 * but may still needed: BUS_CODE, ILL_STACK, etc.
 */

#else /* !SVR4 */

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__)

#undef sigmask
#undef sigemptyset
#undef sigfillset
#undef sigaddset
#undef sigdelset
#undef sigismember

#define sigmask(n)              ((unsigned int)1 << ((n) - 1))
#define sigemptyset(set)        (*(set) = (sigset_t) 0)
#define sigfillset(set)         (*(set) = (sigset_t) -1)
#define sigaddset(set, signo)   (*(set) |= sigmask(signo))
#define sigdelset(set, signo)   (*(set) &= ~sigmask(signo))
#define sigismember(set, signo) (*(set) & sigmask(signo))

#else /* !__FreeBSD__ */
#ifdef __dos__

/*
 * DJGPP requires far too many signals (>300!) to use a simple bit mask.
 * We have to live with the default non-macro operations.
 */

#else

#define sigemptyset(set)        (*(set) = (sigset_t) 0)
#define sigfillset(set)         (*(set) = (sigset_t) -1)
#define sigaddset(set, signo)   (*(set) |= sigmask(signo))
#define sigdelset(set, signo)   (*(set) &= ~sigmask(signo))
#define sigismember(set, signo) (*(set) & sigmask(signo))

#endif /* !__dos__ */
#endif /* !__FreeBSD__ */
#endif /* !SVR4 */
#endif /* !SOLARIS */

/*
 * define UNIX interface, de-multiplex overlayed function names via syscalls
 */
#define SIGPROCMASK          pthread_p_sigprocmask
#define SIGACTION            pthread_p_sigaction
#define SIGSUSPEND           pthread_p_sigsuspend
#ifdef IO
#define ACCEPT               pthread_p_accept
#define SELECT               pthread_p_select
#endif /* IO */

#ifdef SVR4

#define SCP_ONSTACK    (scp->sc_flags & UC_STACK)

#define SCP_CODE       (sip ? sip->si_code : 0)
#define SIG_ADDR       (NULL)

#define SIGSTACK_T     stack_t
#define SIGSTACK       sigaltstack
#define CLR_SS_ONSTACK ss.ss_flags = 0; ss.ss_size = TEMPSTACK_SIZE

#define STACK_OFFSET   TEMPSTACK_SIZE

#define GETPAGESIZE(x) sysconf(x)

#ifdef SYS_sigwait
#define SYS_SIGWAIT SYS_sigwait
#else
#define SYS_SIGWAIT SYS_sigtimedwait
#endif

#if !defined(_ASM) && !defined(LOCORE)
typedef void *malloc_t;
#endif /* !defined(_ASM) && !defined(LOCORE) */

#else /* !SVR4 */

#if defined(__FreeBSD__)

#define SCP_ONSTACK    (scp->sc_onstack)

#define CLR_SCP_ONSTACK scp->sc_onstack = FALSE
#define SCP_CODE       code
#define SIG_ADDR       (NULL)

#define SIGSTACK_T     struct sigstack
#define SIGSTACK       sigstack
#define CLR_SS_ONSTACK ss.ss_onstack = FALSE

#define GETPAGESIZE(x) getpagesize()

#define STACK_OFFSET   0

#elif defined(__linux__) || defined(__dos__)

#define SCP_ONSTACK    FALSE

#define CLR_SCP_ONSTACK /* NULL */
#define SCP_CODE       code
#define SIG_ADDR       scp.cr2

#define SIGSTACK_T     /* NULL */
#define SIGSTACK       /* NULL */
#define CLR_SS_ONSTACK /* NULL */

#define GETPAGESIZE(x) getpagesize()

#define STACK_OFFSET   0

#elif defined (_M_UNIX)

#define SCP_ONSTACK    (scp->sc_onstack)

#define CLR_SCP_ONSTACK scp->sc_onstack = FALSE
#define SCP_CODE       code
#define SIG_ADDR       (NULL)

#define SIGSTACK_T     struct sigstack
#define SIGSTACK       sigstack
#define CLR_SS_ONSTACK ss.ss_onstack = FALSE

#define GETPAGESIZE(x) getpagesize()

#define STACK_OFFSET   0

#else

#define SCP_ONSTACK    (scp->sc_onstack)

#define CLR_SCP_ONSTACK scp->sc_onstack = FALSE
#define SCP_CODE       code
#define SIG_ADDR       addr

#define SIGSTACK_T     struct sigstack
#define SIGSTACK       sigstack
#define CLR_SS_ONSTACK ss.ss_onstack = FALSE

#define GETPAGESIZE(x) getpagesize()

#define STACK_OFFSET   0
#ifndef SA_RESTART
#define SA_RESTART     0
#endif /* !SA_RESTART */

#endif /* !__FreeBSD__ */
#endif /* !SVR4 */

#endif /* !__signal_h && !_SIGNAL_H */

#endif /* !_pthread_signal_internals_h */
