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
// filename: SeqATMCell.h
// SCCS info: $Id$
//
// Default values set for SeqATMCell to contain 53 bytes with
// 8 bits/byte or 424 bits. The variable "cell" will be a pointer
// to an array of (unsigned short)-types. The array will be large
// enough to contain designated number of bits. The SeqATMCell idea,
// as well as much of the following code, was taken from
// C++ PRIMER by Stanley B. Lippman, Addison Weslsey, 2nd ed., 1991.

#ifndef SeqATMCell_h
#define SeqATMCell_h
#include "Message.h"

#ifdef __GNUG__
#pragma interface
#endif

class SeqATMCell : public Message {
public:
	SeqATMCell( const int sequenceNo = 0 );
	SeqATMCell( const SeqATMCell& ); // copy constructor
	~SeqATMCell() { LOG_DEL; delete [] cell; }

	SeqATMCell& set( const int );
	SeqATMCell& unset( const int );
	int isON( const int ) const;
	int isOFF( const int ) const;
	void setSeq( int num ) { seqNo = num; }
	int readSeq() const { return( seqNo ); }

	const char* dataType() const;
	int isA(const char*) const;
	Message* clone() const;
	double asFloat() const { return( double( seqNo ) ); }
	StringList print() const;

private:
	unsigned short* cell;
	int seqNo;
	int getIndex( const int ) const; // returns word containing int
	int getOffSet( const int ) const; // returns pos of bit in word
	const int size; // number of bits desired in SeqATMCell
};

#endif
