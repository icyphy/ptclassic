static const char file_id[] = "FloatVecData.cc";
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
Date of creation: 8/17/91

Methods for class FloatVecData
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "FloatVecData.h"

void FloatVecData::init(int l,const float *srcData) {
	len = l;
	LOG_NEW; data = new float[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

// constructor: makes an uninitialized array
FloatVecData::FloatVecData(int l) : len(l) {
	LOG_NEW; data = new float[l];
}

// constructor: makes an initialized array from a float array
FloatVecData::FloatVecData(int l,const float *srcData) { init(l,srcData); }

// copy constructor
FloatVecData::FloatVecData(const FloatVecData& src) { init(src.len,src.data);}

// constructor: makes an initialized array from a double array
FloatVecData::FloatVecData(int l,const double *srcData) : len(l) {
	LOG_NEW; data = new float[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

	// assignment operator
const FloatVecData& FloatVecData::operator=(const FloatVecData& src) {
	if (data != src.data) {
		LOG_DEL; delete [] data;
		init(src.len,src.data);
	}
	return *this;
}

const char* FloatVecData::dataType() const { return "FloatVecData";}

ISA_FUNC(FloatVecData,Message);

Message* FloatVecData::clone() const { LOG_NEW; return new FloatVecData(*this);}

FloatVecData::~FloatVecData() { LOG_DEL; delete [] data;}

StringList FloatVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		out += ", ";
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}
