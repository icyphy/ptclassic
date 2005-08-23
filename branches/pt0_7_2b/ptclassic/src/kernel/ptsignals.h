/* 
Signal interfaces for Sim Control
@(#)ptsignals.h	1.5 08/16/97
*/
/*
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*
Programmer: Alan Kamas
*/

void ptSafeSig( int SigNum ) ;

void ptBlockSig (int SigNum) ;

void ptReleaseSig (int SigNum);

// Special support for libgthreads under SunOS and Solaris.
#if defined(PTSUN4) || defined(PTSOL2)
#if defined(PTSOL2)
typedef void (*SIG_PT)(int);
#else
typedef void (*SIG_PT)();
#endif
extern "C" SIG_PT ptSignal(int, SIG_PT);
#else
#define ptSignal(sig, handler) signal(sig, handler)
#endif

#if defined(PTIRIX5) 
typedef void (*SIG_PT)(int);
#endif 

#if defined(PT_NT4VC)
typedef void (*SIG_PT)(int);
#endif
