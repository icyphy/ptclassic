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

	const char* dataType() const { return "SeqATMCell"; }
	ISA_INLINE( SeqATMCell, Message ); // is_A() function;
	Message* clone() const {
		INC_LOG_NEW; return new SeqATMCell( *this );
	}
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
