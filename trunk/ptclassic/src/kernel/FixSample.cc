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

static FixSample fixproto;

static Plasma fixPlasma(fixproto);

extern const DataType FIX = "FIX";

Particle* FixSample :: useNew () { LOG_NEW; return new FixSample;}

Particle* FixSample :: clone () {
	Particle* p = fixPlasma.get();
	*p << double(*this);
	return p;
}
void FixSample :: die () { fixPlasma.put(this);}

DataType FixSample :: readType() const {return FIX;}

// loadup saturates the values --
void FixSample :: operator << (int i) {
	FloatSample::operator<<(i);
	saturate();
}

void FixSample :: operator << (double f) {
	FloatSample::operator<<(f);
	saturate();
}

void FixSample :: operator << (const Complex& c) {
	FloatSample::operator<<(c);
	saturate();
}





