static const char file_id[] = "ParticleVector.cc";
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

Methods for class ParticleVector
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "ParticleVector.h"

void ParticleVector::init(int l, Particle **srcData, const char** fields) {
	len = l;
	LOG_NEW;
	data = new ParticlePointer[l];
	fieldName = fields;

	for (int i = 0; i < l; i++) {
		data[i] = *srcData++;
	}
}

// constructor: makes an uninitialized array
ParticleVector::ParticleVector(int l) : len(l) {
        data = new ParticlePointer[l];
	fieldName = new const char *[l];
}

// constructor: makes an initialized array from a ParticleVector array
ParticleVector::ParticleVector(int l,Particle** srcData, const char** fields) 
{
  init(l, srcData, fields);
}

// copy constructor
ParticleVector::ParticleVector(const ParticleVector& src) 
{
  init(src.length(), src.Data(), src.Fields());
}

// insert particle and corresponding field name into appropriate arrays
void ParticleVector::insert(int slot, const char* field, Particle* input) {

  if ((slot > len) || (slot < 0)) {
    // error message
    return;
  }

  data[slot-1] = input;
  fieldName[slot-1] = field;
}

// retrieve a particle given the field name
Particle* ParticleVector::retrieve(const char* field) const {

  for (int i =0; i < len; i++) {
    if (strcmp(fieldName[i], field) == 0)
      return data[i];
  }
  return NULL;
}

const char* ParticleVector::dataType() const { return "ParticleVector";}

ISA_FUNC(ParticleVector,Message);

Message* ParticleVector::clone() const { 
  LOG_NEW; 
  return new ParticleVector(*this);
}

ParticleVector::~ParticleVector() { 
  delete [] data;
  delete [] fieldName;
}

StringList ParticleVector::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i]->print();
		out += ", ";
	}
	if (len > 0) out += data[i]->print();
	out += "}";
	return out;
}

		
