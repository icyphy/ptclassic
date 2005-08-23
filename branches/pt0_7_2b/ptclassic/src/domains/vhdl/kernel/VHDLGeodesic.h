#ifndef _VHDLGeodesic_h
#define _VHDLGeodesic_h 1
/******************************************************************
Version identification:
@(#)VHDLGeodesic.h	1.7 02/17/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: Edward A. Lee, Michael C. Williamson

 Geodesic for VHDL code generation.
 In addition to fork management provided by CG Geodesics,
 they manage definition and naming of buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"

class VHDLGeodesic : public CGGeodesic {
public:
	VHDLGeodesic() : bufName(0) {}
	~VHDLGeodesic() {LOG_DEL; delete [] bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(const char* n) {
	    LOG_DEL; delete [] bufName; bufName = savestring(n);
	}
    
	char* getBufName() const;

	// Initialize the geodesic.
	void initialize();

	// Update token put position by specified number of tokens.
	void putTokens(int);

	// Update token get position by specified number of tokens.
	void getTokens(int);

	// Return the index of the first token addition to the queue.
	int firstPut() { return firstIn; }
	// Return the index of the first token removal from the queue.
	int firstGet() { return firstOut; }
	// Return the index of the next token addition to the queue.
	int nextPut() { return nextIn; }
	// Return the index of the next token removal from the queue.
	int nextGet() { return nextOut; }

protected:

private:
	// Name of the VHDL object used as a buffer
	char* bufName;

	// Index of first reference to a token placed on the queue.
	int firstIn;
	// Index of first reference to a token removed from the queue.
	int firstOut;
	// Index of next reference to a token placed on the queue.
	int nextIn;
	// Index of next reference to a token removed from the queue.
	int nextOut;
};

#endif
