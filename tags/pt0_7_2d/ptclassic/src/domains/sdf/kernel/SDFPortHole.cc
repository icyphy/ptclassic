static const char file_id[] = "SDFPortHole.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFStar.h"
#include "SDFPortHole.h"
#include "CircularBuffer.h"
#include "Geodesic.h"

/**************************************************************************
Version identification:
@(#)SDFPortHole.cc	2.19	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer:  E. A. Lee, J. Buck, D. G. Messerschmitt
 Date of creation: 5/29/90

Code for SDF-type portholes and multiportholes.

**************************************************************************/

int InSDFPort :: isItInput () const { return TRUE;}

void InSDFPort :: receiveData () { getParticle();}

int OutSDFPort :: isItOutput () const { return TRUE;}

void OutSDFPort :: receiveData () { clearParticle();}

void OutSDFPort :: sendData () { putParticle();}

int MultiInSDFPort :: isItInput () const { return TRUE;}
int MultiOutSDFPort :: isItOutput () const { return TRUE;}

PortHole& MultiInSDFPort :: newPort () {
	LOG_NEW; SDFPortHole& p = *new InSDFPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	return installPort(p);
}
 
 
PortHole& MultiOutSDFPort :: newPort () {
	LOG_NEW; SDFPortHole& p = *new OutSDFPort;
	p.setSDFParams(numberTokens,numberTokens-1);
	return installPort(p);
}

// isA
ISA_FUNC(SDFPortHole,DFPortHole);
