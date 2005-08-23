/*
@Version: @(#)ACSGeodesic.h	1.9 09/08/99
@Copyright (c) 1998 The Regents of the University of California.
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
*/

#ifndef _ACSGeodesic_h
#define _ACSGeodesic_h

#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"
class ACSPortHole;

class ACSGeodesic : public CGGeodesic {
public:
	ACSGeodesic() : bufName(0), myWaste(2.0) {}
	~ACSGeodesic() {LOG_DEL; delete [] bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(const char* n) {
		delete [] bufName; bufName = savestring(n);
	}
	char* getBufName() const;

	// make large the wasteFactor
	void preferLinearBuf(int i) { if (i) myWaste = 1e9; }

	// Return the size of the "local buffer" connected to this
	// PortHole.  This returns zero for cases where no separate
	// buffer is allocated, e.g. fork outputs (all destinations
	// of the fork share the same buffer, whose size is returned
	// by bufSize).
	// virtual int localBufSize() const;

protected:
//	ACSPortHole* src();
//	const ACSPortHole* src() const;
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
