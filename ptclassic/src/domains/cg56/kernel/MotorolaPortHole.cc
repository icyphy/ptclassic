static const char file_id[] = "MotorolaPortHole.cc";
/******************************************************************
Version identification:
$Id$

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

 Programmer: E. A. Lee

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaPortHole.h"

// allocate portholes in X memory by default
// this can be changed by specifying, say, P_YMEM
// in the attributes block in a star.
MotorolaPortHole::MotorolaPortHole() { setAttributes(P_XMEM);}

int InMotorolaPort :: isItInput() const { return TRUE; }
int OutMotorolaPort :: isItOutput() const { return TRUE; }
int MultiInMotorolaPort :: isItInput() const { return TRUE; }
int MultiOutMotorolaPort :: isItOutput() const { return TRUE; }

// Dummy
int MultiMotorolaPort :: someFunc() { return 1; }

PortHole& MultiInMotorolaPort :: newPort () {
        LOG_NEW; InMotorolaPort& p = *new InMotorolaPort;
	p.setSDFParams(numberTokens,numberTokens-1);
        return installPort(p);
}

PortHole& MultiOutMotorolaPort :: newPort () {
	LOG_NEW; OutMotorolaPort& p = *new OutMotorolaPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(p);
        return installPort(p);
}
