#ifndef _BDFRelIter_h
#define _BDFRelIter_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

Programmer:  J. Buck

This file defines an iterator for finding all BDFClustPorts that are
the same as, or the complement of, a given BDFClustPort, using BDF_SAME
and BDF_COMPLEMENT relations.

**************************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

struct BDFRelWork;

class BDFClustPortRelIter {
public:
	// constructor: argument is the port.
	BDFClustPortRelIter(BDFClustPort& p);

	// destructor: clear all history entries.
	~BDFClustPortRelIter() { clearHistory();}

	// return the next matching port and its relation to the master.
	BDFClustPort* next(BDFRelation&);

	// go back to the beginning.
	void reset() { clearHistory(); init(); }
private:
	// make assignment, copy ctor private to prevent their use.
	// we will not define them.
	BDFClustPortRelIter(const BDFClustPortRelIter&);
	BDFClustPortRelIter& operator = (const BDFClustPortRelIter&);

	void init();		// initializations for constructor, reset.
	void pushFar();		// defer processing of far() pointer.
	void popFar();		// set up for processing of defered far() ptr.
	void clearHistory();	// delete stacks (for reset and destructor)

	BDFClustPort* pos;	// where we are
	BDFClustPort* start;	// where we started
	BDFRelation curRel;	// relation of pos to start
	BDFRelWork* workStack;	// stack of additional ptrs to process
	SequentialList visited;	// list of all visited ports.
};

#endif
