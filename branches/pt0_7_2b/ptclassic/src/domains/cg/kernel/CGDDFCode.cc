static const char file_id[] = "CGDDFCode.cc";

/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
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

 Programmer: Soonhoi ha

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGDDFCode.h"
#include "MultiTarget.h"
#include "Error.h"

        //////////////////////////////
        // Case contruct
        //////////////////////////////

void CGDDFCode :: startCode_Case(CGStar*, Geodesic*, CGTarget*) { 
	Error::abortRun(*owner, ": startCode_Case() method is not defined.");
}

void CGDDFCode :: middleCode_Case(int, CGTarget*)  {
	Error::abortRun(*owner, ": middleCode_Case() method is not defined.");
	// add some code like } else if (..) {
}

void CGDDFCode :: endCode_Case(CGTarget*) {
	Error::abortRun(*owner, ": endCode_Case() method is not defined.");
	// add final code(e.g. close bracket)
}

        //////////////////////////////
        // DoWhile contruct
        //////////////////////////////

void CGDDFCode :: startCode_DoWhile(Geodesic*, Geodesic*, CGTarget*) { 
     Error::abortRun(*owner, ": startCode_DoWhile() method is not defined.");
}

void CGDDFCode :: endCode_DoWhile(CGStar*, Geodesic*, CGTarget*) {
	Error::abortRun(*owner, ": endCode_DoWhile() method is not defined.");
	// add final code(e.g. close bracket)
}

        //////////////////////////////
        // For contruct
        //////////////////////////////

void CGDDFCode :: startCode_For(CGStar*, Geodesic*, CGTarget*) { 
	Error::abortRun(*owner, ": startCode_For() method is not defined.");
}

void CGDDFCode :: middleCode_For(CGStar*, CGStar*, int, int, CGTarget*) {
	Error::abortRun(*owner, ": middleCode_For() method is not defined.");
	// add some code like } else if (..) {
}

void CGDDFCode :: endCode_For(CGTarget*) {
	Error::abortRun(*owner, ": endCode_For() method is not defined.");
	// add final code(e.g. close bracket)
}

        //////////////////////////////
        // Recur contruct
        //////////////////////////////

void CGDDFCode :: startCode_Recur(Geodesic*, PortHole*,
					const char*, CGTarget*) { 
	Error::abortRun(*owner, ": startCode_Recur() method is not defined.");
	// function definition....
}

void CGDDFCode :: middleCode_Recur(Geodesic*,Geodesic*,
					const char*,CGTarget*) {
	Error::abortRun(*owner, ": middleCode_Recur() method is not defined.");
	// function calls...
}

void CGDDFCode :: endCode_Recur(Geodesic*, const char*, CGTarget*) {
	Error::abortRun(*owner, ": endCode_Recur() method is not defined.");
	// add final code(e.g. close bracket) and final result copy.
}

void CGDDFCode :: prepCode(MultiTarget*, Profile*, int, int) {}
void CGDDFCode :: signalCopy(int) {}

