#ifndef _CGCGeodesic_h
#define _CGCGeodesic_h 1
/******************************************************************
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

 Programmer: S. Ha and E. A. Lee

 Geodesic for C code generation.
 In addition to fork management provide by CG Geodesics,
 they manage definition and naming of buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"

class CGCGeodesic : public CGGeodesic {
public:
	CGCGeodesic() : bufName(0), myWaste(2.0) {}
	~CGCGeodesic() {LOG_DEL; delete bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(char* n) {bufName = n;}
	char* getBufName() const;

	// make large the wasteFactor
	void preferLinearBuf(int i) { if (i) myWaste = 1e9; }

protected:
	// Redefine: decide the wasteFactor for linear buffering.
	// Look at the comment in CGGeodesic class.
	double wasteFactor() const;

private:
	// Name of the C object used as a buffer
	char* bufName;
	// wasteFactor
	double myWaste;
};

#endif
