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
// filename: SeqATMCell.cc
// SCCS info: $Id$
//
// This file contains the member functions of the
// SeqATMCell class.

static const char file_id[] = "SeqATMCell.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SeqATMCell.h"

// 424 = 8 * 53, and there are 53 bytes in the proposed broadband
// integrated serviced digital network (BISDN) cell.
// Asynchronous transfer mode (ATM) is the switching method used
// by the BISDN.
static const int BISDNCellSize = 424;


// default constructor
SeqATMCell::SeqATMCell( const int sequenceNo ) : seqNo( sequenceNo ),
		size( BISDNCellSize )
{
// 'wordWidth' is minimum number of unsigned short needed
// to hold 'size' bits.
	const int wordWidth = ( size + 8*sizeof(short) - 1 ) /
			8*sizeof(short);
	LOG_NEW;
	cell = new unsigned short[ wordWidth ];
	for ( int i = 0; i < wordWidth; ++i ) // initialize all bits to 0
		cell[ i ] = 0;
}

// copy constructor
SeqATMCell::SeqATMCell( const SeqATMCell& ba ) : size( BISDNCellSize )
{
	const int wordWidth = ( size + 8*sizeof(short) - 1 ) /
			8*sizeof(short);
	LOG_NEW;
	cell = new unsigned short[ wordWidth ];
	for ( int i = 0; i < wordWidth; ++i )
		cell[ i ] = ba.cell[ i ];
	this->seqNo = ba.seqNo;
}

// returns word which contains the ith bit
inline int SeqATMCell::getIndex( const int ith ) const {
	return( ith / 8*sizeof(short) );
}

// returns position of ith bit in a word
inline int SeqATMCell::getOffSet( const int ith ) const {
	return( ith % 8*sizeof(short) );
}

// turns on the ith bit
SeqATMCell& SeqATMCell::set( const int ith ) {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	cell[ index ] = cell[ index ] | ( 1 << offset );
	return *this;
}

// turns off the ith bit
SeqATMCell& SeqATMCell::unset( const int ith ) {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	cell[ index ] = cell[ index ] & ( ~( 1 << offset ) );
	return *this;
}

// returns TRUE if ith bit is ON
int SeqATMCell::isON( const int ith ) const {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	return( cell[ index ] & ( 1 << offset ) );
}

// returns TRUE if ith bit is OFF
int SeqATMCell::isOFF( const int ith ) const {
	return( !( this->isON( ith ) ) );
}

StringList SeqATMCell::print() const {
	StringList out;
	out << "\nSeqATMCell: seqNo = ";
	out << readSeq() << "\n";
	for ( int i = 0; i < size; ++i ) {
		if ( isON( i ) )
			out << 1;
		else out << 0;
		if ( ( i + 1 ) % 8 == 0 )
			out << "\n";
	}
	return out;
}

// class identification
ISA_FUNC(SeqATMCell,Message);

Message* SeqATMCell::clone() const {
	LOG_NEW; return new SeqATMCell( *this );
}

const char* SeqATMCell::dataType() const { return "SeqATMCell"; }
