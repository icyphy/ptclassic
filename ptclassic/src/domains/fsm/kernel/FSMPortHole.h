#ifndef _FSMPortHole_h
#define _FSMPortHole_h

/* $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

    Programmer:		Bilung Lee
    Date of creation:	3/3/96

    Definitions of domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "EventHorizon.h"

class FSMPortHole : public PortHole {
public:
    // Class identification.
    virtual int isA(const char* className) const;
};

class InFSMPort : public FSMPortHole {
public:
    // Get Particles from input Geodesic
    virtual void receiveData();

    // Input/output identification.
    virtual int isItInput() const;
};


class OutFSMPort : public FSMPortHole {
public:
    // Initialize the current Particles in the PortHole.
    virtual void receiveData();

    // Put the Particles that were generated into the output Geodesic.
    virtual void sendData();

    // Input/output identification.
    virtual int isItOutput() const;
};

 
class MultiFSMPort : public MultiPortHole {};

class MultiInFSMPort : public MultiFSMPort {
public:
    // Input/output identification.
    virtual int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};
 
class MultiOutFSMPort : public MultiFSMPort {     
public:
    // Input/output identification.
    virtual int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};

/*
// Iterators for MultiFSMPorts -- they aren't required but make coding
// stars a bit cleaner.  They "know" that MultiInFSMPorts have InFSMPorts
// and MultiOutFSMPorts have OutFSMPorts.

class InFSMMPHIter : public MPHIter {
public:
	InFSMMPHIter(MultiInFSMPort& mph) : MPHIter(mph) {}
	InFSMPort* next() { return (InFSMPort*) MPHIter::next();}
	InFSMPort* operator++(POSTFIX_OP) { return next();}
};

class OutFSMMPHIter : public MPHIter {
public:
	OutFSMMPHIter(MultiOutFSMPort& mph) : MPHIter(mph) {}
	OutFSMPort* next() { return (OutFSMPort*) MPHIter::next();}
	OutFSMPort* operator++(POSTFIX_OP) { return next();}
};
*/

#endif
