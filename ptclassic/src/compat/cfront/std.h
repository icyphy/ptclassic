/* Copyright (c) 1990-1994 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/* Version Identification:
 * $Id$
 */
#ifndef _std_h
#define _std_h
/*  _std_h     is defined by the g++ std.h file */

#if defined(hppa)
#include <stdlib.h>
#include <sys/unistd.h>
#include "../../kernel/miscFuncs.h"
//extern void srand48(long int);
//extern long int lrand48(void);
#define random() lrand48()
#define srandom(a) srand48(a)
//#define random() ((long)(rand()))
//#define srandom(seed) (srand( (unsigned int)(seed))
#else
// Sun
#include <stdlib.h>
#include <sysent.h>
#include "../../kernel/miscFuncs.h"
// we could include <strings.h> too, but Sun Cfront 2.1 has an
// incorrect prototype for strcasecmp.
extern "C" int strcasecmp(const char*, const char*);
#endif
#endif /* _std_h */
