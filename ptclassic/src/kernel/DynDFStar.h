#ifndef _DynDFStar_h
#define _DynDFStar_h 1
#ifdef __GNUG__
#pragma interface
#endif
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1993 The Regents of the University of California.
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

 Programmer:  J. T. Buck

 DynDFStar (dynamic dataflow star) is the base class for dataflow stars
 that have dynamic behavior.

*******************************************************************/

#include "DataFlowStar.h"

// class DynDFStar
// dataflow star that may be dynamic.  BDFStar, DDFStar, CG-stars
// are inherited from this.

class DynDFStar : public DataFlowStar {
public:
	DynDFStar();

	// waitPort returns a pointer to a port that requires more
	// data if the star is to be run (if such a port exists).
	/* virtual */ DFPortHole* waitPort() const;

	// waitTokens returns the number of additional tokens
	// required on waitPort.
	/* virtual */ int waitTokens() const;

	// class identification.
	/* virtual */ int isA(const char* className) const;

	/* virtual */ void initialize();

	/* virtual */ int isSDF() const;

protected:
	// methods for manipulating waitPort.
	void waitFor(DFPortHole& port,int howMany = 1);
	void clearWaitPort();
private:
	DFPortHole* myWaitPort;
	int myWaitCount;
	unsigned char sdfFlag;
};

#endif
