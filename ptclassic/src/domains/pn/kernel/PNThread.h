#ifndef _PNThread_h
#define _PNThread_h

/* 
Copyright (c) 1990-1994 The Regents of the University of California.
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
*/
/*  Version $Id$
    Author:	T. M. Parks
    Created:	14 February 1992
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "AweThread.h"

class DataFlowStar;
class PtCondition;

class PNThread : public AweThread
{
public:
    // Constructor.
    PNThread(unsigned int stackSize = defStackSize)
	: AweThread(stackSize) {}
};

// A Kahn process for dataflow actors.
class DataFlowProcess : public PNThread
{
public:
    // Constructor.
    DataFlowProcess(DataFlowStar& s, unsigned int stackSize = defStackSize)
	: star(s), PNThread(stackSize) {}

protected:
    /*virtual*/ void run();
    DataFlowStar& star;
};

// A synchronizing Kahn process for datflow actors.
class SyncDataFlowProcess : public DataFlowProcess
{
public:
    // Constructor.
    SyncDataFlowProcess(DataFlowStar& s, PtCondition& c,
	unsigned int stackSize = defStackSize)
	: start(c), DataFlowProcess(s, stackSize) {}

protected:
    /*virtual*/ void run();
    PtCondition& start;
};

#endif
