// filename: BitArray.h
// SCCS info: $Id$
//
// Default values set for BitArray to contain 53 bytes with
// 8 bits/byte or 424 bits. The variable "cell" will be a pointer
// to an array of (unsigned short)-types. The array will be large
// enough to contain designated number of bits. The BitArray idea,
// as well as much of the following code, was taken from
// C++ PRIMER by Stanley B. Lippman, Addison Weslsey, 2nd ed., 1991.

#ifndef BitArray_h
#define BitArray_h
#include "Message.h"

#ifdef __GNUG__
#pragma interface
#endif

class BitArray : public Message {
public:
	BitArray( const int sequenceNo = 0 );
	BitArray( const BitArray& ); // copy constructor
	~BitArray() { LOG_DEL; delete [] cell; }

	BitArray& set( const int );
	BitArray& unset( const int );
	int isON( const int ) const;
	int isOFF( const int ) const;
	void setSeq( int num ) { seqNo = num; }
	int readSeq() const { return( seqNo ); }

	const char* dataType() const { return "BitArray"; }
	ISA_INLINE( BitArray, Message ); // is_A() function;
	Message* clone() const {
		INC_LOG_NEW; return new BitArray( *this );
	}
	double asFloat() const { return( double( seqNo ) ); }
	StringList print() const;

private:
	unsigned short* cell;
	int seqNo;
	int getIndex( const int ) const; // returns word containing int
	int getOffSet( const int ) const; // returns pos of bit in word
	const int wordSize; // number of bits in unsigned short
	const int size; // number of bits desired in BitArray
};

#endif
