static const char file_id[] = "CGDDFCode.cc";

/******************************************************************
Version identification:
@(#)CGDDFCode.cc	1.3	7/5/96

Copyright (c) 1995 Seoul National University
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

 Programmer: Soonhoi ha

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGDDFCode.h"
#include "MultiTarget.h"
#include "Error.h"

// Start a case construct, e.g. switch(c)
void CGDDFCode :: startCode_Case(CGStar*, Geodesic*, CGTarget*) {
	Error::abortRun(*owner, "startCode_Case() method is not defined.");
}

// Continue a case construct, e.g. code like } else if (..) {
void CGDDFCode :: middleCode_Case(int, CGTarget*)  {
	Error::abortRun(*owner, "middleCode_Case() method is not defined.");
}

// End a case construct, e.g. close brace
void CGDDFCode :: endCode_Case(CGTarget*) {
	Error::abortRun(*owner, "endCode_Case() method is not defined.");
}

// Start a do-while construct
void CGDDFCode :: startCode_DoWhile(Geodesic*, Geodesic*, CGTarget*) { 
	Error::abortRun(*owner, "startCode_DoWhile() method is not defined.");
}

// End a do-while construct
void CGDDFCode :: endCode_DoWhile(CGStar*, Geodesic*, CGTarget*) {
	Error::abortRun(*owner, "endCode_DoWhile() method is not defined.");
	// add final code(e.g. close bracket)
}

// Start a for-loop
void CGDDFCode :: startCode_For(CGStar*, Geodesic*, CGTarget*) { 
	Error::abortRun(*owner, "startCode_For() method is not defined.");
}

// Continue a for-loop: add some code like } else if (..) {
void CGDDFCode :: middleCode_For(CGStar*, CGStar*, int, int, CGTarget*) {
	Error::abortRun(*owner, "middleCode_For() method is not defined.");
}

// End a for-loop: add final code(e.g. close bracket)
void CGDDFCode :: endCode_For(CGTarget*) {
	Error::abortRun(*owner, "endCode_For() method is not defined.");
}

// Start recursion: define the recursive function
void CGDDFCode :: startCode_Recur(Geodesic*, PortHole*,
					const char*, CGTarget*) { 
	Error::abortRun(*owner, "startCode_Recur() method is not defined.");
}

// Continue recursion: function calls...
void CGDDFCode :: middleCode_Recur(Geodesic*,Geodesic*,
					const char*,CGTarget*) {
	Error::abortRun(*owner, "middleCode_Recur() method is not defined.");
}

// End recursion: final code (e.g. close bracket) and final result copy
void CGDDFCode :: endCode_Recur(Geodesic*, const char*, CGTarget*) {
	Error::abortRun(*owner, "endCode_Recur() method is not defined.");
}

void CGDDFCode :: prepCode(MultiTarget*, Profile*, int, int) {}
void CGDDFCode :: signalCopy(int) {}
