static const char file_id[] = "Message.cc";

#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
@(#)FileParticle.cc	1.3 3/7/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  John Reekie
 Date of creation: 1/27/96

**************************************************************************/
#include "FileParticle.h"
#include "Plasma.h"
#include "Error.h"

static FileParticle fpproto;
static Plasma fileMessagePlasma(fpproto);
static PlasmaGate fileMessageGate(fileMessagePlasma);


extern const DataType FILEMSG = "FILEMSG";


// The contents of an "empty" file message. This never gets initialized,
// but apparently it works in the matrix classes...
static Envelope dummy;


DataType FileParticle::type() const { return FILEMSG;}

// particle copy
Particle& FileParticle::operator = (const Particle& p) {
	if (compareType(p)) {
		const FileParticle& ps = *(const FileParticle*)&p;
		data = ps.data;
	}
	return *this;
}

// clone, useNew, die analogous to other particles.

Particle* FileParticle::clone() const {
	Particle * p = fileMessagePlasma.get();
	((FileParticle*)p)->data = data;
	return p;
}

Particle* FileParticle::useNew() const {
	LOG_NEW; return new FileParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other Message objects and allows them
// to be deleted when no longer required.
void FileParticle::die() {
	data = dummy;
	fileMessagePlasma.put(this);
}
