#ifndef _KnownTarget_h
#define _KnownTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 9/23/91

The KnownTarget class.

This class provides a list of all known targets.  There is a single
list, so all unlike blocks, all target names must be unique.

The idea is that each target that is "known to the system"
should add an instance of itself to the known list by code something
like

static MyType proto;
static KnownTarget entry(proto,"MyType");

Then the static method KnownTarget::clone(name) can produce a new
instance of the named class in the current domain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

class Target;
class KnownTargetEntry;

class KnownTarget {
private:
	static KnownTargetEntry* findEntry(const char* name);
public:
// The constructor takes a target and a name, and adds a corresponding
// entry to the known list.
	KnownTarget (Target &target, const char* name) {
		addEntry (target, name, 0);
	}

// This function actually adds the target to the list.  If onHeap is true,
// the target will be destroyed when the entry is removed or replaced from
// the list.
	static void addEntry (Target &target, const char* name, int onHeap);

// The find method returns a pointer to the appropriate target, or
// NULL if not found.
	static const Target* find (const char* name);

// The clone method takes a string, finds the appropriate target, and
// returns a clone of that target.
	static Target* clone (const char* name);

// This list returns a list of targets that are legal for a particular
// Block.  The return value gives the number of matches.
// "names" is an array of strings that can hold up to nMax strings.
	static int getList (const Block& b, const char** names, int nMax);

// This version is the same except that it takes a domain as argument.
	static int getList (const char* dom, const char** names, int nMax);

// Return true if the named target is dynamically linked.
	static int isDynamic (const char* type);

// Return the default target name for a domain (default: current domain)
	static const char* defaultName(const char* dom = 0);
};

class KnownTargetIter {
private:
	KnownTargetEntry* pos;
public:
	KnownTargetIter() { reset();}
	void reset();
	const Target* next();
	const Target* operator++() { return next();}
};

#endif
