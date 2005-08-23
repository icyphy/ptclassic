#ifndef _C50PortHole_h
#define _C50PortHole_h 1
/******************************************************************
Version identification:
@(#)C50PortHole.h	1.2	8/15/96

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
Date of Creation: 28 January 1994

Modeled after code by J. Buck.

These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "TIPortHole.h"
#include "TIAttributes.h"

class ProcMemory;
class AsmGeodesic;

#define C50PortHole     TIPortHole

class InC50Port : public C50PortHole {
	int isItInput() const;
};

class OutC50Port: public C50PortHole {
	int isItOutput() const;
};

#define MultiC50Port	MultiTIPort

class MultiInC50Port : public MultiTIPort {
public:
	PortHole& newPort();
	int isItInput() const;
};

class MultiOutC50Port : public MultiTIPort {
public:
	PortHole& newPort();
	int isItOutput() const;
};

#endif
