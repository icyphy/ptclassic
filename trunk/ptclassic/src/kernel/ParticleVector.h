#ifndef _ParticleVector_h
#define _ParticleVector_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1996-%Q% The Regents of the University of California.
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

Programmer:  R. P. Gunturi
Date of creation: 9/19/96

This is a simple particle vector body. It stores an array of particles
of arbitrary length. This length is specified by the constructor.
**************************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "Message.h"
#include "Particle.h"

typedef Particle* ParticlePointer;

class ParticleVector : public Message {
private:
	int len;
	Particle **data;
	const char     **fieldName;
	void init(int l, Particle **srcData, const char** fields);

public:

	int length() const { return len;}
	const char* dataType() const;
	int isA(const char*) const;

	Particle** Data() const {return data;}
	const char** Fields() const {return fieldName;}

	// constructor: makes an uninitialized array
	ParticleVector(int l);

	// constructor: makes an initialized array from a Particle array
	ParticleVector(int l, Particle **srcData, const char** fields);

	// copy constructor
	ParticleVector(const ParticleVector& src);
	
	// insert a particle
	void insert(int slot, const char* field, Particle* input);

	// retrieve a particle given the field name
	Particle* retrieve(const char* field) const;

	// clone
	Message* clone() const;

	// print
	StringList print() const;

	// destructor
	~ParticleVector();
};
#endif

