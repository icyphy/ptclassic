/*
Copyright (c) 1990-1999 The Regents of the University of California.
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
// @(#)ACG.h	1.13 10/06/99
#ifndef _ACG_h
#define _ACG_h 1
// This is a fake-out of the libg++ random-number interface.
// The difference is that there is only a single random-number
// stream, with seed set by srandom and values read by random.
// This works fine within Ptolemy.

#if defined(hppa) || defined(SVR4) || defined(SYSV) || defined(__sgi) || defined(linux)
#if !defined(__GNUG__) || defined(PT_EGCS)
extern "C" {
  void srand48(long int);
  long int mrand48(void);
};
class ACG {
public:
	ACG(unsigned seed = 1) {srand48(seed);}
	unsigned long asLong() {return (unsigned long)mrand48();}
};
#endif /* !__GNUG__ || PT_EGCS */
#else /* hppa ...*/
#ifdef PT_USE_RAND
extern "C" {
#include <stdlib.h>
};
class ACG {
public:
	ACG(unsigned seed = 1) {srand(seed);}
	unsigned long asLong() {return (unsigned long)rand();}
};

#else /* PT_USE_RAND */
extern "C" {
#if defined(PTNT)
#include <stdlib.h>
#else
  void srandom(unsigned);
  long random();
#endif
}
class ACG {
public:
	ACG(unsigned seed = 1) {srandom(seed);}
	unsigned long ACG::asLong() {
	  long res = random();
	  if ((random() & 1) != 0) res = ~res; // (not -res, as -0 = 0)
	    return (unsigned long) res;
	}

};
#endif /* PT_USE_RAND */
#endif /* hppa ....*/
#endif
