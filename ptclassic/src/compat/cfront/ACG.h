/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
// $Id$
#ifndef _ACG_h
#define _ACG_h 1
// This is a fake-out of the libg++ random-number interface.
// The difference is that there is only a single random-number
// stream, with seed set by srandom and values read by random.
// This works fine within Ptolemy.

#if defined(hppa) || defined(SVR4) || defined(SYSV) || defined(__sgi)
#ifndef __GNUG__
extern "C" {
  void srand48(long int);
  long int mrand48(void);
};
class ACG {
public:
	ACG(unsigned seed = 1) {srand48(seed);}
	asLong() {return (unsigned long)mrand48();}
};
#endif /* !__GNUG__ */
#else /* hppa */
extern "C" {
  void srandom(unsigned);
  long random();
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
#endif /* hppa */
#endif
