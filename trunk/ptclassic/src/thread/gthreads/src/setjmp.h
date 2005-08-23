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

  @(#)setjmp.h	3.1 10/11/96

*/

#ifndef _pthread_pthread_setjmp_h
#define _pthread_pthread_setmp_h

#if defined(__FreeBSD__)
#include <pthread/fre_setjmp.h>
#endif
#if defined(__dos__)
#include <pthread/dos_setjmp.h>
#endif
#if defined(__linux__)
#include <pthread/lin_setjmp.h>
#endif
#if defined(_M_UNIX)
#include <pthread/sco_setjmp.h>
#endif

#ifdef TIMEVAL_TO_TIMESPEC
#define env_use environment[0]._jb
#define env_decl environment
#else /* !TIMEVAL_TO_TIMESPEC */
#define env_use  environment
#define env_decl environment
#endif /* !TIMEVAL_TO_TIMESPEC */

#ifdef _M_UNIX
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#include <setjmp.h>
#undef _POSIX_SOURCE
#else /* _POSIX_SOURCE */
#include <setjmp.h>
#endif /* _POSIX_SOURCE */
#else /* !_M_UNIX */
#if defined(__linux__) && !defined(__KERNEL__)
#define __KERNEL__
#define KERNEL_NP
#endif /* __KERNEL__ */
#include <setjmp.h>
#if defined(__linux__) && defined(KERNEL_NP)
#undef __KERNEL__
#undef KERNEL_NP
#endif /* __KERNEL__ */
#endif /* !_M_UNIX */

#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)

/*
 * for this version, the index of JB_SP must be even !!!
 * This way, we can speed up the context switch (using std).
 */
#define JB_SVMASK 3
#define JB_SP     0
#define JB_PC     1
#define JB_MASK   4

#else /* !defined(ASM_SETJMP) && defined(C_CONTEXT_SWITCH) */
#ifdef SOLARIS

#define JB_SVMASK 0
#define JB_SP     1
#define JB_PC     2
#define JB_MASK   12

#else /* !SOLARIS */
#ifdef SVR4

#include <setjmp.h> /* We hope that this defines JB_xxx */

#else /* !SVR4 */
#if defined(__FreeBSD__)

#define JB_SVMASK 6
#define JB_SP     2
#define JB_PC     0
#define JB_MASK   7
#define JB_BP     3

#elif defined(__linux__) || defined(__dos__)

#define JB_SVMASK 6
#define JB_SP     4
#define JB_PC     5
#define JB_MASK   7
#define JB_BP     3

#elif defined (_M_UNIX)

#define JB_SVMASK 6
#define JB_SP     4
#define JB_PC     5
#define JB_MASK   7
#define JB_BP     3

#else

#define JB_SVMASK 0
#define JB_SP     3
#define JB_PC     4
#define JB_MASK   2

#endif
#endif /* !SVR4 */
#endif /* !SOLARIS */
#endif /* !defined(ASM_SETJMP) && defined(C_CONTEXT_SWITCH) */

#endif /* !_pthread_setjmp_internals_h */
