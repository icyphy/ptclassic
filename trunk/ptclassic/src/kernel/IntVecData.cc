static const char file_id[] = "IntVecData.cc";
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

Methods for class IntVecData
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "IntVecData.h"

void IntVecData::init(int l,const int *srcData) {
	len = l;
	LOG_NEW; data = new int[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

// constructor: makes an uninitialized array
IntVecData::IntVecData(int l) : len(l) {
	LOG_NEW; data = new int[l];
}

// constructor: makes an initialized array from a int array
IntVecData::IntVecData(int l,int *srcData) { init(l,srcData);}

// copy constructor
IntVecData::IntVecData(const IntVecData& src) { init(src.len,src.data);}

const char* IntVecData::dataType() const { return "IntVecData";}

ISA_FUNC(IntVecData,Message);

Message* IntVecData::clone() const { LOG_NEW; return new IntVecData(*this);}

IntVecData::~IntVecData() { LOG_DEL; delete [] data;}

StringList IntVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		out += ", ";
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}

		
