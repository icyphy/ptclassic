static const char file_id[] = "FixSample.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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

Particle* FixParticle :: useNew () { LOG_NEW; return new FixParticle;}

Particle* FixParticle :: clone () {
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





