static const char file_id[] = "TIPortHole.cc";
/******************************************************************
Version identification:
@(#)TIPortHole.cc	1.2	8/15/96

@Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer: A. Baensch

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "TIPortHole.h"

// allocate portholes in B1 memory by default
// this can be changed by specifying, say, P_UMEM
// in the attributes block in a star.
TIPortHole::TIPortHole() { setAttributes(P_BMEM);}

int InTIPort :: isItInput() const { return TRUE; }
int OutTIPort :: isItOutput() const { return TRUE; }
int MultiInTIPort :: isItInput() const { return TRUE; }
int MultiOutTIPort :: isItOutput() const { return TRUE; }

// Dummy
int MultiTIPort :: someFunc() { return 1; }

PortHole& MultiInTIPort :: newPort () {
        LOG_NEW; InTIPort& p = *new InTIPort;
	p.setSDFParams(numberTokens,numberTokens-1);
        return installPort(p);
}

PortHole& MultiOutTIPort :: newPort () {
	LOG_NEW; OutTIPort& p = *new OutTIPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(p);
        return installPort(p);
}

