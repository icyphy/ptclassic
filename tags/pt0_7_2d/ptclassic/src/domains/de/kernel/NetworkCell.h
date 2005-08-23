/* 
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
*/
// filename:	NetworkCell.h
// author:		Paul Haskell
// creation:	7/1/91
// SCCS info:	@(#)NetworkCell.h	1.6 03/02/95

#ifndef _NetworkCell_h
#define _NetworkCell_h
#ifdef __GNUG__
#pragma interface
#endif


////////////////////////////////////////////////////////////////////////
// This file defines a Video Cell (packet-like) type that can be used to
// store and transmit objects of any type derived from Message.
// The data() function returns a const pointer to the stored Message
// object. NOTE!! Stars that call data() can LOOK at the resulting
// object's data, but should NOT modify it. Stars should call
// writableData() if they want a pointer to a Message object that they
// can modify.
//
// The variable "ourData" stores the Message that a NetworkCell
// object holds. By using "ourData", the reference count for the stored
// image does not fall to zero while the NetworkCell still exists, so
// the NetworkCell's contents will not be deleted.
//
// Priorities increase from a low of zero (0). The operator int()
// function returns the priority.
////////////////////////////////////////////////////////////////////////

#include "Message.h"


class NetworkCell: public Message {
public:
	//	NetworkCell(Message, priority, destination, size, source,
	//		createTime, expireTime);
	//
	// Second constructor creates empty NetworkCell.
	//
	NetworkCell(Message&, const int = 0, const int = 0,
			const int = 0, const int = 0, const float = 0.0,
			const float = 0.0);
	NetworkCell(const int = 0, const int = 0, const int = 0,
			const int = 0, const float = 0.0, const float = 0.0);

	NetworkCell(const NetworkCell& vc);

	virtual ~NetworkCell() { }


	int dest() const
			{ return destAddr; }
	const Message* data() const
			{ return ourData.myData(); }
	Message* writableData()
			{ return ourData.writableCopy(); }
	virtual int size() const
			{ return mySize; }
	operator int() const
			{ return priority; }
	int contentsType(const char* type) const
			{ return ( ourData.typeCheck(type) ); }

////// For backwards compatibility--going away soon!
	const Message* retData() const { return data(); }
	virtual int retSize() const { return size(); }
	int nextDest() { return dest(); }


////// Message-like stuff
	virtual const char* dataType() const
			{ return("NetworkCell"); }
	virtual Message* clone() const
			{ INC_LOG_NEW; return new NetworkCell(*this); }
	int isA(const char*) const;
	StringList print() const;


protected:
	Envelope ourData;

	float createTime;
	float expireTime;
	int destAddr;
	int srcAddr;
	int mySize;
	int priority;
};


#endif // #ifndef _NetworkCell_h
