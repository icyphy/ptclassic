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
//////// filename: NetworkCell.cc
//////// SCCS info: $Id$
static const char file_id[] = "NetworkCell.cc";

#ifdef __GNUG__
#pragma implementation
#endif


//////// include files
#include "NetworkCell.h"


//////// Class functions
NetworkCell::NetworkCell(Message& a, const int pri,
		const int dest, const int sz, const int src, const float ct,
		const float et):
		priority(pri), destAddr(dest), mySize(sz), srcAddr(src),
		createTime(ct), expireTime(et)
{ Envelope tmp(a); ourData = tmp; }


NetworkCell::NetworkCell(const int pri, const int dest, const int sz,
		const int src, const float ct, const float et):
		priority(pri), destAddr(dest), mySize(sz), srcAddr(src),
		createTime(ct), expireTime(et)
{ Envelope tmp; ourData = tmp; }


NetworkCell::NetworkCell(const NetworkCell& vc): priority(vc.priority),
		destAddr(vc.destAddr), mySize(vc.mySize), srcAddr(vc.srcAddr),
		createTime(vc.createTime), expireTime(vc.expireTime)
{ ourData = vc.ourData; }


StringList NetworkCell::print() const
{
	char* printBuf = new char[80]; // StringList d'tor frees this.
	sprintf(printBuf, "<%s> pri=%d dest=%d size=%d",
			dataType(), priority, destAddr, mySize);
	return (StringList(printBuf));
}

ISA_FUNC(NetworkCell, Message);
