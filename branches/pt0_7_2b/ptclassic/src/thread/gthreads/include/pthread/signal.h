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

  @(#)signal.h	3.1 10/11/96

*/

#ifndef _pthread_signal_h
#define _pthread_signal_h

#ifndef	__signal_h

#ifdef LOCORE
#undef LOCORE
#endif

#ifdef SVR4_NP
#include <siginfo.h>
#include <ucontext.h>
#else /* !SVR4_NP */
#include "stdtypes.h"
#endif /* !SVR4_NP */

#ifdef _M_UNIX
#ifdef __STDC__
#undef __STDC__
#include <sys/signal.h>
#define __STDC__ 10
#else /* !__STDC__ */
include <sys/signal.h>
#endif /* !__STDC__ */
#else /* !_M_UNIX */
#if defined(__linux__) && !defined(__KERNEL__)
#define __KERNEL__
#define KERNEL_NP
#endif /* __KERNEL__ */
#include <signal.h>
#if defined(__linux__) && defined(KERNEL_NP)
#undef __KERNEL__
#undef KERNEL_NP
#endif /* __KERNEL__ */
#endif /* !_M_UNIX */

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined (__dos__)
#if !defined(__signal_h) && !defined(_SIGNAL_H) && !defined(__SIGNAL_H)
#define __signal_h

#ifndef _sys_signal_h
typedef unsigned int sigset_t;
#endif

struct sigaction {    
        void            (*sa_handler)();
        sigset_t        sa_mask;
        int             sa_flags;
};
#endif /* __signal_h */
#endif /* __FreeBSD__ */

#ifndef NSIG
#define NSIG SIGMAX
#endif

#define NNSIG     NSIG+1

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0

#if defined(__FreeBSD__)
#include <sys/time.h>
#elif defined(__dos__)
#include <time.h>
#endif

#ifndef TIMEVAL_TO_TIMESPEC
#define ts_sec tv_sec
#define ts_nsec tv_nsec

#if !defined(__linux__) || !defined(_ASMi386_SIGCONTEXT_H)
struct timespec {
  time_t tv_sec;
  long   tv_nsec;
};
#endif /* !defined(__linux) || !defined(SIGCLD) */
#endif /* !TIMEVAL_TO_TIMESPEC */
#else /* CLOCK_REALTIME */
#define ts_sec tv_sec
#define ts_nsec tv_nsec
#endif /* CLOCK_REALTIME */

#define PTHREAD_INTR_ENABLE       SIG_UNBLOCK
#define PTHREAD_INTR_DISABLE      SIG_BLOCK
#define PTHREAD_INTR_CONTROLLED   0
#define PTHREAD_INTR_ASYNCHRONOUS 1

#ifdef si_value
#undef si_value
#endif

union p_sigval {
  int sigval_int;
  void *sigval_ptr;
};

struct p_siginfo {
  int si_signo;
  int si_code;
  union p_sigval si_value;
};

/*
 * This defines the implementation-dependent context structure provided
 * as the third parameter to user handlers installed by sigaction().
 * It should be a copy of the first part of the BSD sigcontext structure.
 * The second half should not be accessed since it is only present if
 * a _sigtramp instance is present right below the user handler on the
 * thread's stack. For SVR4, we will have to build this structure from scratch.
 */

#ifdef SOLARIS_NP

struct context_t {
  u_long  sc_flags;
  struct ucontext *sc_link;
  sigset_t sc_mask;  /* per-thread signal mask to be restored */
  stack_t  sc_stack;
  int sc_filler;
  greg_t  sc_psr;
  greg_t  sc_pc;     /* program counter to be restored */
  greg_t  sc_npc;    /* next pc (see below) */
  greg_t  sc_y;
  greg_t  sc_g1;
  greg_t  sc_g2;
  greg_t  sc_g3;
  greg_t  sc_g4;
  greg_t  sc_g5;
  greg_t  sc_g6;
  greg_t  sc_g7;
  greg_t  sc_o0;
  greg_t  sc_o1;
  greg_t  sc_o2;
  greg_t  sc_o3;
  greg_t  sc_o4;
  greg_t  sc_o5;
  greg_t  sc_sp;     /* stack pointer to be restored */
  greg_t  sc_o7;
};

#else /* !SOLARIS_NP */
#ifdef SVR4_NP

struct context_t {
  This needs to be defined !
  };

#else /* !SVR4_NP */

#if defined(__FreeBSD__)
#define context_t sigcontext
/*typedef struct sigcontext context_t;*/
#define p_sigval sigval
#define siginfo p_siginfo
typedef struct siginfo siginfo_t;
#elif defined(__dos__)
#include <setjmp.h>

/* Hate to do it, but since the dj include files don't define this
   in such a way that I can get to it, I have to repeat the definition
   here.  This is subject to change with new releases of djgpp; let the
   buyer beware. */

struct context_t {
  unsigned long __eax, __ebx, __ecx, __edx, __esi;
  unsigned long __edi, __ebp, __esp, __eip, __eflags;
  unsigned short __cs, __ds, __es, __fs, __gs, __ss;
  unsigned long __sigmask; /* for POSIX signals only */
  unsigned long __signum; /* for expansion */
  unsigned char __fpu_state[108]; /* for future use */
};

#define sc_sp   __esp
#define sc_fp   __ebp
#define sc_pc   __eip
#define sc_ps   __eflags
#define sc_mask __sigmask
#define p_sigval sigval
#define siginfo p_siginfo
typedef struct siginfo siginfo_t;
#elif defined(__linux__)
#define context_t sigcontext_struct
#define sc_sp   esp
#define sc_fp   ebp
#define sc_pc   eip
#define sc_ps   eflags
#define sc_mask oldmask
#define p_sigval sigval
#define siginfo p_siginfo
#define FC_PROT 1
typedef struct siginfo siginfo_t;
#elif defined (_M_UNIX)
#define context_t sigcontext
#define p_sigval sigval
#define siginfo p_siginfo
#ifndef __M_UNIX__
typedef struct siginfo siginfo_t;
#endif

#if 0
/*
 * Location of the users' stored registers relative to EAX.
 * Usage is u.u_ar0[XX].
 *
 * NOTE: ERR is the error code.
 */

#define SCO_SS      18
#define SCO_UESP    17
#define SCO_EFL     16
#define SCO_CS      15
#define SCO_EIP     14
#define SCO_ERR     13
#define SCO_TRAPNO  12
#define SCO_EAX     11
#define SCO_ECX     10
#define SCO_EDX     9
#define SCO_EBX     8
#define SCO_ESP     7
#define SCO_EBP     6
#define SCO_ESI     5
#define SCO_EDI     4
#define SCO_DS      3
#define SCO_ES      2
#define SCO_FS      1
#define SCO_GS      0
#endif


#  define sc_sp sc_UESP
#  define sc_fp sc_EBP
#  define sc_pc sc_EIP
#  define sc_ps sc_EFL

struct  sigcontext {
  unsigned int   sc_GS;   /*  0 */
  unsigned int   sc_FS;   /*  1 */
  unsigned int   sc_ES;   /*  2 */
  unsigned int   sc_DS;   /*  3 */
  unsigned int   sc_EDI;  /*  4 */
  unsigned int   sc_ESI;  /*  5 */
  unsigned int   sc_EBP;  /*  6 */
  unsigned int   sc_ESP;  /*  7 */
  unsigned int   sc_EBX;  /*  8 */
  unsigned int   sc_EDX;  /*  9 */
  unsigned int   sc_ECX;  /* 10 */
  unsigned int   sc_EAX;  /* 11 */
  unsigned int   sc_TRAPNO; /* 12 */
  unsigned int   sc_ERR;  /* 13 */
  unsigned int   sc_EIP;  /* 14 */
  unsigned int   sc_CS;   /* 15 */
  unsigned int   sc_EFL;  /* 16 */
  unsigned int   sc_UESP; /* 17 */
  unsigned int   sc_SS;   /* 18 */
  unsigned int   *piFPContext;
  unsigned int   *weitek_offset;
  sigset_t       sc_mask;
};


/*
 * Send an interrupt to process.
 * Pass the signal number as an argument to the user signal handler.
 * Also pushed on the user stack is all of the user registers and the
 * ret addr for a piece of code that calls back into the kernel
 * (after executing the user's signal handler) to restore the
 * context of the user process.  Last, but not least, put on the
 * signal mask, for sigclean to restore after execution of the
 * handler (POSIX).
 */

/*
** From Nick Logan:
** After the signal number, the register are pushed onto the user stack in an
** array indexed by the values in /usr/include/sys/reg.h. If an FPU is used the
** FP registers are then saved on the stack.
**
** Note that this could change in any release, and you use it at your
** own risk!
*/
#else
#define p_sigval sigval
#define siginfo p_siginfo
typedef struct siginfo siginfo_t;

typedef int     greg_t;

struct context_t {
  greg_t sc_onstack;/* ignored */
  sigset_t sc_mask; /* per-thread signal mask to be restored */
  greg_t sc_sp;     /* stack pointer to be restored */
  greg_t sc_pc;     /* program counter to be restored */
  greg_t sc_npc;    /* next pc, only used if _sigtramp present
                     * on thread's stack, ignored o.w.
		     * should usually be pc+4
		     */
  greg_t sc_g1;
  greg_t sc_o0;
};

#endif
#endif /* !SVR4_NP */
#endif /* !SOLARIS_NP */

#ifndef SA_SIGINFO
#define SA_SIGINFO 0
#endif

#ifndef SA_ONESHOT
#define SA_ONESHOT 0
#endif

#ifndef SA_NOMASK
#define SA_NOMASK 0
#endif

#ifndef SA_ONSTACK
#define SA_ONSTACK SV_ONSTACK
#endif /* !SA_ONSTACK */

#ifndef BUS_OBJERR
#define BUS_OBJERR FC_OBJERR
#endif /* !BUS_OBJERR */

#ifndef BUS_CODE
#define BUS_CODE(x) FC_CODE(x)
#endif

#endif /* __signal_h */

#endif /* !_pthread_signal_h */
