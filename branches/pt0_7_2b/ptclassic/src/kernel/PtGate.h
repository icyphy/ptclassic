#ifndef _PtGate_h
#define _PtGate_h 1
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

 Programmer:  J. T. Buck and T. M. Parks
 Date of creation: 6/14/93

This file defines classes that support multi-threading in the kernel.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

// PtGate is an abstract baseclass; derived classes typically provide the
// desired semantics for use with a particular threads library (such
// as Sun lightweight processes, Posix threads, etc.  The PtGate object
// provides a "lock" and an "unlock" primitive.  Also, "clone" is a sort
// of virtual constructor, permitting more identical PtGate objects to
// be constructed given a prototype.

class PtGate
{
    friend class CriticalSection;
public:
	virtual ~PtGate();
	// make a new, identical PtGate
	virtual PtGate* clone() const = 0;
protected:
	// obtain exclusive use of the lock
	virtual void lock() = 0;
	// release the lock
	virtual void unlock() = 0;
};

// A CriticalSection object is used only for the side effects of its
// constructor and destructor.  The constructor optionally sets the lock
// and the destructor releases it; the effect is that, if the PtGate pointer
// is non-null, measures will be taken to ensure that only one section of
// code controlled by a given PtGate is active at once.

// Multithreading performance is best if code regions protected by a
// CriticalSection are short.  It is assumed that the PtGate object will
// never be deleted during the lifetime of the CriticalSection.

class GateKeeper;

class CriticalSection
{
public:
// the three constructors all supply a PtGate somehow.  The CriticalSection
// does nothing if the gate pointer is null.
	CriticalSection(PtGate* g) : mutex(g)
	{
		if (mutex) mutex->lock();
	}
	CriticalSection(PtGate& g) : mutex(&g) { mutex->lock();}

	// definition of this constructor follows GateKeeper def.
	inline CriticalSection(GateKeeper& gk);

	~CriticalSection() { if (mutex) mutex->unlock();}
private:
	PtGate* mutex;
};

// GateKeeper provides a means of registering a number of PtGate pointers
// in a list, together with a way of creating or deleting a series of
// PtGates all at once.
// A GateKeeper object should be declared only at file scope.

// WARNING: do not invoke GateKeeper::deleteAll while a CriticalSection
// is alive, or Bad Things will happen.

// magic value to assure validity
const int GateKeeper_cookie = 0xdeadbeef;

class GateKeeper
{
    friend class CriticalSection;
public:
	// constructor adds object to master list.  The argument is a
	// reference to a PtGate pointer; this pointer is modified when
	// the GateKeeper "enables" or "disables" the PtGate pointer.
	GateKeeper(PtGate*& gateToKeep);

	// destructor: remove from master list, zap the gate object.
	~GateKeeper();

	// create a PtGate for each GateKeeper on the list.
	static void enableAll(const PtGate& master);

	// destroy all gates in GateKeeper objects.
	static void disableAll();

	// return TRUE if the GateKeeper is enabled (has a PtGate).
	int enabled() const { return (gate != 0);}
private:
	// reference to the "kept" PtGate pointer
	PtGate * & gate;

	// next GateKeeper on the list.
	GateKeeper *next;

	// flag to indicate that construction is complete.
	// as a static object it will be 0; constructor sets it to
	// GateKeeper_cookie.
	int valid;

	// class-wide list head
	static GateKeeper *listHead;
};

// A KeptGate is a GateKeeper that contains its own PtGate pointer.
class KeptGate : public GateKeeper {
public:
	KeptGate() : myGate(0), GateKeeper(myGate) {}
private:
	PtGate* myGate;
};

// CriticalSection using GateKeeper.  It is inactive if the GateKeeper
// is not fully constructed, hence the test for "valid" (this solves the
// order-of-constructors problem).

inline CriticalSection::CriticalSection(GateKeeper& gk)
: mutex(gk.valid == GateKeeper_cookie ? gk.gate : 0)
{
	if (mutex) mutex->lock();
}

#endif
