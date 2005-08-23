static const char file_id[] = "FSMPortHole.cc";

/*  @(#)FSMPortHole.cc	1.4 12/09/97

@Copyright (c) 1996-1997 The Regents of the University of California.
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

    Programmer:		Bilung Lee
    Date of creation:	3/3/96

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "FSMPortHole.h"

// Class identification.
ISA_FUNC(FSMPortHole,PortHole);

void InFSMPort :: receiveData () { getParticle(); }

// Input/output identification.
int InFSMPort::isItInput() const { return TRUE; }

void OutFSMPort :: receiveData () { clearParticle(); }

void OutFSMPort :: sendData () { putParticle(); }

// Input/output identification.
int OutFSMPort::isItOutput() const { return TRUE; }

// Input/output identification.
int MultiInFSMPort::isItInput() const { return TRUE; }

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInFSMPort::newPort() {
    LOG_NEW; PortHole& p = *new InFSMPort;
    return installPort(p);
}

// Input/output identification.
int MultiOutFSMPort::isItOutput() const { return TRUE; }

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutFSMPort::newPort() {
    LOG_NEW; PortHole& p = *new OutFSMPort;
    return installPort(p);
}
