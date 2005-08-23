static const char file_id[] = "C50PortHole.cc";

/*
Version:
@(#)C50PortHole.cc	1.2	8/15/96

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

Programmer: Andreas Baensch
Date of creation: 10 December 1992

Modeled after code by T.M. Parks dated 5 January 1992.

These classes are portholes for stars that generate assembly language code.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "C50PortHole.h"

int InC50Port :: isItInput() const { return TRUE; } 
int OutC50Port :: isItOutput() const { return TRUE; }
int MultiInC50Port :: isItInput() const { return TRUE; }
int MultiOutC50Port :: isItOutput() const { return TRUE; }

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutC50Port :: newPort(){
	LOG_NEW; OutC50Port& p = *new OutC50Port;
        p.setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(p);
	return installPort(p);
}

PortHole& MultiInC50Port :: newPort(){
	LOG_NEW; C50PortHole& p = *new InC50Port;
        p.setSDFParams(numberTokens,numberTokens-1);
	return installPort(p);
}
