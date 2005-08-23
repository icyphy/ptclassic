#ifndef _FileParticle_h
#define _FileParticle_h 1

#ifdef __GNUG__
#pragma interface
#endif

/**************************************************************************
Version identification:
@(#)FileParticle.h	1.1 1/27/96

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

This is a Particle class that contains FileMessages (accessed via
Envelopes). The class exists in order to make File messages
"first-class" datatypes.

I consider this a temporary measure until we can rationalise the
way Message particles work. This class inherits from Particle
(duplicating a lot of code in MessageParticle) just so I don't
have to change MessageParticle (by making member functions virtual).


**************************************************************************/
#include "Message.h"

extern const DataType FILEMSG;

class FileParticle : public Particle {
public:
	DataType type() const;

	operator int () const;
	operator float () const;
	operator double () const;
	operator Complex () const;
	operator Fix () const;
	StringList print() const;

	void getMessage (Envelope& p);
	void accessMessage (Envelope& p) const;

	// fill in remaining functions for Particle classes
	FileParticle(const Envelope& p);
	FileParticle();
	Particle& initialize();

	// The next two methods return an error.  Should be redefined by
	// derived classes.
 
        // Initialize a given ParticleStack with the values in the delay
        // string, obtaining other Particles from the given Plasma.  
        // Returns the number of total Particles initialized, including
        // this one.  This should be redefined by the specific message class.
	// 3/2/94 added
        /*virtual*/ int initParticleStack(Block* parent, ParticleStack& pstack,
                                          Plasma* myPlasma, 
					  const char* delay = 0);

	// load with data -- these cause errors except for a Envelope argument.

	void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Fix& c);
	void operator << (const Envelope& p);

	// particle copy
	Particle& operator = (const Particle& p);

	// compare particles
	int operator == (const Particle&);

	// clone, useNew, die analogous to other particles.

	Particle* clone() const;

	Particle* useNew() const;
	void die();

private:
	void errorAssign(const char*) const;
	Envelope data;
};


#endif
