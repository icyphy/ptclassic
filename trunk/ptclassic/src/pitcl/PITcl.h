/**************************************************************************
Version identification:
$Id$

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
		       
 Programmer:  J. T. Buck
 Date of creation: 3/4/92

This file implements a class that adds Ptolemy-specific Tcl commands to
a Tcl interpreter.  Commands are designed to resemble those of the earlier
interpreter.

**************************************************************************/

#ifndef _PTcl_h
#define _PTcl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "tcl.h"
#include "NamedObj.h"

class InterpUniverse;
class InterpGalaxy;
class Target;
class Block;
class StringList;

// IUList is a list of InterpUniverses.  The destructor deletes
// all universes on the list.

class IUList : private NamedObjList {
    friend class IUListIter;
public:
	IUList() {}
	~IUList() { deleteAll();}
	void put(InterpUniverse& u);
	InterpUniverse* univWithName(const char* name) {
		return (InterpUniverse*)objWithName(name);
	}
	// delete the universe with name "name" on the list, if it exists.
	int delUniv(const char* name);
	NamedObjList::size;
	NamedObjList::initElements;
	NamedObjList::deleteAll;
};

// an iterator for IUList
class IUListIter : private NamedObjListIter {
public:
	IUListIter(IUList& sl) : NamedObjListIter (sl) {}
	InterpUniverse* next() {
		return (InterpUniverse*)NamedObjListIter::next();
	}
	InterpUniverse* operator++(POSTFIX_OP) { return next();}
	NamedObjListIter::reset;
};

class PTcl {

// Used in pigilib's kernel calls, so must be public - aok
public:
	// the current Ptolemy universe
	InterpUniverse* universe;

	// current galaxy being built
	InterpGalaxy* currentGalaxy;

	// target for the current galaxy
	Target* currentTarget;

	// the current domain
	const char* curDomain;

// These are not used outside PTcl. -aok
private:
	// the Tcl interpreter
	Tcl_Interp* interp;

	// runtime control
	double stopTime;
	double lastTime;

	// flag set while defining a galaxy
	short definingGal;

	// flag to indicate that interp is owned by me
	short myInterp;

	// the list of known universes
	IUList univs;

	// function to register extensions with the Tcl interpreter
	void registerFuncs();

// This are public for testing.  should be returned to private. - aok
// protected:
public:
	// these three functions are used to associate PTcl objects
	// with interpreters.
	static PTcl* findPTcl(Tcl_Interp*);
	void makeEntry();
	void removeEntry();

	// create a new universe with name nm and domain dom.
	void newUniv(const char* nm, const char* dom);

	// reset -- for backward compatibility
	void resetUniverse() { reset(1,0);}

	// delete the universe named nm.  return FALSE if no such univ.
	int delUniv(const char* nm);

	// return a usage error
	int usage(const char*);

	// find blocks for info-showing commands
	const Block* getBlock(const char*);

	// return a StringList result with proper cleanup as the
	// result of a Tcl command.
	int result(StringList&);

	// return a "static result".  Don't give this one a stringlist!
	int staticResult(const char*);

	// append a value to the result, using Tcl_AppendElement.
	void addResult(const char*);

	// compute the schedule
	int computeSchedule();

public:
// the active Tcl interpreter, for error reporting.
	static Tcl_Interp* activeInterp;

	PTcl(Tcl_Interp* interp = 0);
	~PTcl();

	// the dispatcher is called by Tcl to handle all extension
	// commands.
	static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);

// the following are the Tcl-callable functions.  Each returns TCL_OK
// or TCL_ERROR, and may set the Tcl result to return a string using
// Tcl_SetResult, Tcl_AppendElement, PTcl::result or PTcl::staticResult.
	int alias(int argc,char** argv);
	int animation(int argc,char** argv);
	int busconnect(int argc,char** argv);
	int connect(int argc,char** argv);
	int cont(int argc,char** argv);
	int curuniverse(int argc,char** argv);
	int defgalaxy(int argc,char** argv);
	int delnode(int argc,char** argv);
	int delstar(int argc,char** argv);
	int deluniverse(int argc,char** argv);
	int descriptor(int argc,char** argv);
	int disconnect(int argc,char** argv);
	int domain(int argc,char** argv);
	int domains(int argc,char** argv);
	int exit(int argc,char** argv);
	int knownlist(int argc,char** argv);
	int link(int argc,char** argv);
	int multilink(int argc,char** argv);
	int newstate(int argc,char** argv);
	int newuniverse(int argc,char** argv);
	int node(int argc,char** argv);
	int nodeconnect(int argc,char** argv);
	int numports(int argc,char** argv);
	int print(int argc,char** argv);
	int renameuniv(int argc,char** argv);
	int reset(int argc,char** argv);
	int run(int argc,char** argv);
	int schedule(int argc,char** argv);
	int seed(int argc,char** argv);
	int setstate(int argc,char** argv);
	int star(int argc,char** argv);
	int statevalue(int argc,char** argv);
	int target(int argc,char** argv);
	int targetparam(int argc,char** argv);
	int targets(int argc,char** argv);
	int topblocks(int argc,char** argv);
	int univlist(int argc,char** argv);
	int wrapup(int argc,char** argv);
};

#endif
