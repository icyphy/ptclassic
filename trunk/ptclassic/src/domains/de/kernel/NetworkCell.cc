/* 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		createTime(ct), expireTime(et), destAddr(dest), 
		srcAddr(src), mySize(sz), priority(pri)
{ Envelope tmp(a); ourData = tmp; }


NetworkCell::NetworkCell(const int pri, const int dest, const int sz,
		const int src, const float ct, const float et):
		createTime(ct), expireTime(et),
		destAddr(dest), srcAddr(src), mySize(sz), priority(pri)
{ Envelope tmp; ourData = tmp; }


NetworkCell::NetworkCell(const NetworkCell& vc):
		createTime(vc.createTime), expireTime(vc.expireTime),
		destAddr(vc.destAddr), srcAddr(vc.srcAddr), mySize(vc.mySize), 
  		priority(vc.priority)
{ ourData = vc.ourData; }


StringList NetworkCell::print() const
{
	StringList printBuf = "<";
	printBuf << dataType();
	printBuf << ">";
	printBuf << " pri=" << priority;
	printBuf << " dest=" << destAddr;
	printBuf << " size=" << mySize;
	return printBuf;
}

ISA_FUNC(NetworkCell, Message);
