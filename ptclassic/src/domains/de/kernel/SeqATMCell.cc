// filename: BitArray.cc
// SCCS info: $Id$
//
// This file contains the member functions of the
// BitArray class.

static const char file_id[] = "BitArray.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BitArray.h"

// default constructor
BitArray::BitArray( const int sequenceNo ) : seqNo( sequenceNo ),
		wordSize( 16 ), size( 424 ) {
	// wordWidth is minimum number of unsigned short needed
	// to hold size
	typedef unsigned short bitArrayType;
	int wordWidth = ( size + wordSize - 1 ) / wordSize;
	LOG_NEW;
	cell = new bitArrayType[ wordWidth ];
	for ( int i = 0; i < wordWidth; ++i ) // initialize all bits to 0
		cell[ i ] = 0;
}

// copy constructor
BitArray::BitArray( const BitArray& ba ) : wordSize( 16 ), size( 424 ) {
	typedef unsigned short bitArrayType;
	int wordWidth = ( size + wordSize - 1 ) / wordSize;
	LOG_NEW;
	cell = new bitArrayType[ wordWidth ];
	for ( int i = 0; i < wordWidth; ++i )
		cell[ i ] = ba.cell[ i ];
	this->seqNo = ba.seqNo;
}

// returns word which contains the ith bit
inline int BitArray::getIndex( const int ith ) const {
	return( ith / wordSize );
}

// returns position of ith bit in a word
inline int BitArray::getOffSet( const int ith ) const {
	return( ith % wordSize );
}

// turns on the ith bit
BitArray& BitArray::set( const int ith ) {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	cell[ index ] = cell[ index ] | ( 1 << offset );
	return *this;
}

// turns off the ith bit
BitArray& BitArray::unset( const int ith ) {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	cell[ index ] = cell[ index ] & ( ~( 1 << offset ) );
	return *this;
}

// returns TRUE if ith bit is ON
int BitArray::isON( const int ith ) const {
	int index = getIndex( ith );
	int offset = getOffSet( ith );
	return( cell[ index ] & ( 1 << offset ) );
}

// returns TRUE if ith bit is OFF
int BitArray::isOFF( const int ith ) const {
	return( !( this->isON( ith ) ) );
}

StringList BitArray::print() const {
	StringList out;
	out << "\nBitArray: seqNo = ";
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
