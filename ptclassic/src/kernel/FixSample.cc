static const char file_id[] = "FixSample.cc";
/**************************************************************************
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

 Programmer:  J. T. Buck
 Date of creation: 10/17/91
 Revisions:

 A FixSample is like a FloatSample, but its legal values range from -1
 to 1.  Thus it is not REALLY fixed point.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "FixSample.h"
#include "Plasma.h"

static FixParticle fixproto;

static Plasma fixPlasma(fixproto);

extern const DataType FIX = "FIX";

Particle* FixParticle :: useNew () const { LOG_NEW; return new FixParticle;}

Particle* FixParticle :: clone () const {
	Particle* p = fixPlasma.get();
	*p << double(*this);
	return p;
}
void FixParticle :: die () { fixPlasma.put(this);}

DataType FixParticle :: type() const {return FIX;}

// loadup saturates the values --
void FixParticle :: operator << (int i) {
	FloatParticle::operator<<(i);
	saturate();
}

void FixParticle :: operator << (double f) {
	FloatParticle::operator<<(f);
	saturate();
}

void FixParticle :: operator << (const Complex& c) {
	FloatParticle::operator<<(c);
	saturate();
}





