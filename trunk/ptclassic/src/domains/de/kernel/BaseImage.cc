/* 
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
*/
static const char file_id[] = "BaseImage.cc";
// filename:		BaseImage.cc
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$

#ifdef __GNUG__
#pragma implementation
#endif

#include "BaseImage.h"

BaseImage* BaseImage::fragment(int, int) const
	{ return 0;}
void BaseImage::assemble(const BaseImage*)
	{}
const char* BaseImage::dataType() const
	{ return "BaseImage";}
Message* BaseImage::clone() const
	{ LOG_NEW; return new BaseImage(*this); }
Message* BaseImage::clone(int a) const
	{ LOG_NEW; return new BaseImage(*this, a); }


void BaseImage::copy(int len, float* into, const float* from) const
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
}

void BaseImage::copy(int len, char* into, const char* from) const
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
}

void BaseImage::copy(int len, unsigned char* into,
		unsigned const char* from) const
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
} // end BaseImage::copy()

ISA_FUNC(BaseImage,Message);
