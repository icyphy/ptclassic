static const char file_id[] = "Message.cc";

#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
@(#)FileParticle.cc	1.1 1/27/96

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


FileParticle::FileParticle(const Envelope& p) : data(p) {}

FileParticle::FileParticle() {}

DataType FileParticle::type() const { return FILEMSG;}

FileParticle::operator int () const {
	return data.asInt();
}

FileParticle::operator double () const {
	return data.asFloat();
}

FileParticle::operator float () const {
	return float(data.asFloat());
}

FileParticle::operator Complex () const {
	return data.asComplex();
}

FileParticle::operator Fix () const {
	return data.asFix();
}


StringList FileParticle::print() const {
	return data.print();
}

// get the Message from the FileParticle.
void FileParticle::accessMessage (Envelope& p) const {
	p = data;
}

// get the Message and remove the reference (by setting data to dummy)
void FileParticle::getMessage (Envelope& p) {
	p = data;
	data = dummy;
}

Particle& FileParticle::initialize() { data = dummy; return *this;}

// Initialize a given ParticleStack with the values in the delay
// string, obtaining other Particles from the given Plasma.  
// Returns the number of total Particles initialized, including
// this one.  This should be redefined by the specific message class.
// 3/2/94 added
int FileParticle::initParticleStack(Block* /*parent*/,
				       ParticleStack& /*pstack*/,
				       Plasma* /*myPlasma*/, 
				       const char* /*delay*/) {
  Error::abortRun("delays with initial values not supported by general Messages");
  return 0;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FileParticle::operator << (int) { errorAssign("int");}
void FileParticle::operator << (double) { errorAssign("double");}
void FileParticle::operator << (const Complex&) { errorAssign("complex");}
void FileParticle::operator << (const Fix&) { errorAssign("fix");}

// only loader that works.
void FileParticle::operator << (const Envelope& p) { data = p;}

// particle copy
Particle& FileParticle::operator = (const Particle& p) {
	if (compareType(p)) {
		const FileParticle& ps = *(const FileParticle*)&p;
		data = ps.data;
	}
	return *this;
}

// particle compare: considered equal if Message addresses
// are the same.
int FileParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Envelope pkt;
	p.accessMessage(pkt);
	return data.myData() == pkt.myData();
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

void FileParticle::errorAssign(const char* argType) const {
	StringList msg = "Attempt to assign type ";
	msg += argType;
	msg += " to a FileParticle";
	Error::abortRun(msg);
}

