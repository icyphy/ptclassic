static const char file_id[] = "CG56PortHole.cc";
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

#include "CG56PortHole.h"

int InCG56Port :: isItInput() const { return TRUE; }
int OutCG56Port :: isItOutput() const { return TRUE; }
int MultiInCG56Port :: isItInput() const { return TRUE; }
int MultiOutCG56Port :: isItOutput() const { return TRUE; }

PortHole& MultiInCG56Port :: newPort () {
        LOG_NEW; CG56PortHole& p = *new InCG56Port;
	p.setSDFParams(numberTokens,numberTokens-1);
        return installPort(p);
}

PortHole& MultiOutCG56Port :: newPort () {
	LOG_NEW; OutCG56Port& p = *new OutCG56Port;
	p.setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(p);
        return installPort(p);
}
