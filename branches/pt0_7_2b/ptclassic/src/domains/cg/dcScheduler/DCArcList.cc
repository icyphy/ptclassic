static const char file_id[] = "DCArcList.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: G.C. Sih
Modifier: Soonhoi Ha
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCArcList.h"

// Constructor
DCArc::DCArc(DCNode *src, DCNode *sink, int fst, int scd, int thd) {
	srcnode = src;
	sinknode = sink;
	first = fst; second = scd; third = thd;
	parent = 0;
}

// Reverses the srcnode and sinknode entries of the arc
void DCArc::reverse() {
	DCNode *temp = sinknode;
	sinknode = srcnode;
	srcnode = temp;
}

// Prints information about the arc
StringList DCArc::print() {
	StringList out;
	out += "[";
	out += srcnode->print();
	out += ", ";
	out += sinknode->print();
	out += " (";
	out += first; out += ", ";
	out += second; out += ", ";
	out += third;
	out += ")]\n";
	return out;
}

///////////////////////////////////////////////////////////////////////

// Constructor for initialization by a DCArcList
DCArcList::DCArcList(DCArcList& arclist) {
	DCArcIter iter(arclist);
	DCArc *arc;

	initialize();

	while ((arc = iter++) != 0) {
		LOG_NEW; DCArc *newarc = new DCArc(arc->getSrc(), 
			arc->getSink(), arc->getF(), arc->getS(), arc->getT());
		append(newarc);
	}
}

// Returns 1 if the given arc is a member of the DCArcList, 0 otherwise
int DCArcList::member(DCArc *arc) {
	DCArcIter iter(*this);
	DCArc *listArc;

	while ((listArc = iter++) != 0) {
		if (*arc == *listArc) return TRUE;
        }
        return FALSE;
}

// destructor
DCArcList::~DCArcList() {
	DCArcIter iter(*this);
	DCArc *arc;
	DCArc *prev = 0;
	while ((arc = iter++) != 0) {
		LOG_DEL; delete prev;
		prev = arc;
	}
	LOG_DEL; delete prev;
}

StringList DCArcList::print() {
	StringList out;
	DCArcIter iter(*this);
	DCArc *arc;

	out += "\n";
	while ((arc = iter++) != 0) {
		out += arc->print();
	}
	out += "\n";
	return out;
}
