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
