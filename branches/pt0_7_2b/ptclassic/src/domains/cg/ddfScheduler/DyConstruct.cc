/* 
Copyright (c) 1990-1994 The Regents of the University of California.
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
static const char file_id[] = "DyConstruct.cc";
// $Id$

#ifdef __GNUG__
#pragma implementation
#endif

#include "DyConstruct.h"
#include "CGMacroCluster.h"

int DyConstruct :: checkTopology (CGMacroClusterGal*) {return 0;}

void DyConstruct :: clearSchedules() {}

int DyConstruct :: compareType (CGStar& s, const char* t) {
	const char* nm = s.className();
	const char* p = nm + strlen(s.domain());
	int flag = strcmp(p,t);
	if (!flag) return TRUE;
	nm = s.readTypeName();
	return !strcmp(nm,t);
}

DyConstruct :: ~DyConstruct() {
#if defined(__GNUG__) && __GNUG__ == 1
	LOG_DEL; delete schedules;
#else
	LOG_DEL; delete [] schedules;
#endif
}

