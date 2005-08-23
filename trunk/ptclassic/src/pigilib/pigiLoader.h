#ifndef _PIGILOADER_H
#define _PIGILOADER_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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
   @(#)pigiLoader.h	1.4 3/10/96

   This is a C include file, and NOT a C++ include file.
*/

/* Define the ARGS macro */
#include "compat.h"

extern void KcLoadInit ARGS((const char* argv0));
extern void KcDoStartupLinking();
extern int FindStarSourceFile ARGS((const char* dir, const char* dom,
				    const char* base, char* buf));
extern int KcCompileAndLink ARGS((const char* name, const char* idomain,
				  const char* srcDir, int permB,
				  const char* linkArgs));
extern int KcLoad ARGS((const char* iconName, int permB,
			const char* linkArgs));

#endif  /* _PIGILOADER_H */
