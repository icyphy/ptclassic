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
#ifndef _FixSample_h
#define _FixSample_h 1
#ifdef __GNUG__
#pragma interface
#endif
#include "Particle.h"

extern const DataType FIX;

class FixParticle : public FloatParticle {
protected:
	void saturate() {
		double d = *this;
		if (d > 1.0) *this << 1.0;
		else if (d < -1.0) *this << -1.0;
	}
public:
	DataType type() const;
	Particle* useNew();
	Particle* clone();
	void die();
	void operator<<(int);
	void operator<<(double);
	void operator<<(const Complex&);
};
#endif

