/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
// $Id$
// A macro for generating isA functions in derived classes

#ifndef _isa_h
#define _isa_h 1
#ifdef __STDC__
#define quote(x) #x
#else
#define quote(x) "x"
#endif

extern "C" int strcmp(const char*,const char*);

#define ISA_FUNC(me,parent)\
int me :: isA (const char* cname) const {\
	if (strcmp(cname,quote(me)) == 0) return 1;\
	else return parent::isA(cname);\
}

// Alternate form for use within a class definition body
#define ISA_INLINE(me,parent)\
int isA (const char* cname) const {\
	if (strcmp(cname,quote(me)) == 0) return 1;\
	else return parent::isA(cname);\
}

#endif
