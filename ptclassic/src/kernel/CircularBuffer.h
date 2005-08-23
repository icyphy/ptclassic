#ifndef _CircularBuffer_h
#define _CircularBuffer_h 1
/**************************************************************************
Version identification:
@(#)CircularBuffer.h	1.8	04/17/98

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

 Programmer:  E. A. Lee and D. G. Messerschmitt, J. Buck
 Date of creation: 11/1/90

class CircularBuffer manages a circular buffer (array with modulo
addressing) containing pointers to Particles.  It is used to store
Particle*s corresponding to past input or output Particles.

******************************************************************/
#ifdef __GNUG__
#pragma interface "CircularBuffer.h"
#endif

class Particle;

	/////////////////////////////////////////
	// class CircularBuffer
	/////////////////////////////////////////


class CircularBuffer
{
public:
        // Argument is the dimension of the array to allocate
        CircularBuffer(int);
        ~CircularBuffer();

        // Reset to the beginning of the buffer
        void reset() {current=0;}

	// ZERO out the contents of the buffer
	void initialize();

        // Return current Pointer on the buffer
        Particle** here() const { return buffer+current; }

        // Return next Pointer on the buffer
        Particle** next() { if (++current >= dimen) current = 0;
			    return here();}

        // Back up one Pointer on the buffer
        Particle** last() { if (--current < 0) current = dimen - 1;
			    return here();}
 
	// Back up the buffer by n positions.  Will not work correctly
	// if n is larger than dimen.  n is assumed positive.
	void backup(int n) {
		current -= n;
		if (current < 0) current += dimen;
	}

	// Advance the buffer by n positions.  Will not work correctly
	// if n is larger than dimen.  n is assumed positive.
	void advance(int n) {
		current += n;
		if (current >= dimen) current -= dimen;
	}

        // Access buffer relative to current
        Particle** previous(int) const;
 
        // Size of the buffer
        int size() const {return dimen;}

	// last error msg
	static const char* errMsg() { return errMsgString;}
private:
        // Number of Particles on the buffer
        int dimen;
        // Index of the current Pointer
        int current;
        // Pointer array
        Particle** buffer;
	// An error string for the class
	static const char* errMsgString;
};

#endif
