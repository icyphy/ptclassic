static const char file_id[] = "DDFScheduler.cc";
#ifdef __GNUG__
#pragma implementation
#endif
/**********************************************************************
Version identification:
@(#)DDFScheduler.cc	2.45 10/18/96

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

 Programmers: Soonhoi Ha, Joe Buck
 The authors gratefully acknowledge the contributions of Richard Stevens.

 Richard Stevens is an employee of the U.S. Government whose contributions
 to this computer program fall within the scope of 17 U.S.C. A7 105

 Date of creation: 7/20/94

 Methods for the DDF Scheduler

***********************************************************************/

#include "DDFScheduler.h"
	
extern const char DDFdomainName[];

// my domain
const char* DDFScheduler :: domain () const { return DDFdomainName ;}
