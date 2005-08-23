/**************************************************************************
Version identification:
@(#)PTcl.h	1.36	02/04/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

#include <tcl.h>
#include "NamedObj.h"
#include "SimControl.h"

#undef setstate                 /* work around bug in Linux headers */

class InterpUniverse;
class InterpGalaxy;
class Target;
class Block;
class StringList;

/////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////
//				Action handling
// TclAction is a simple class that contains the relevant information
// about an action that has been registered via Tcl.
class TclAction {
public:
	// Constructor was added to initialize data members.
	TclAction();

	// Destructor cancels the action and
	// frees the memory associated with the pointers below.
	~TclAction();

	char* name;
	SimAction *action;
	char* tclCommand;
};

// TclActionList is a list of pre or post actions that have been
// registered via Tcl.  Each action has a name that can be used to
// refer to it from Tcl.
//
class TclActionList : private SequentialList {
	friend class TclActionListIter;
public:
	TclActionList();

	// Add object to list
        void put(TclAction& s) {SequentialList::put(&s);}

        // export size and initialize functions
        SequentialList::size;
        SequentialList::initialize;

        // find the object with the given name and return pointer
        // or NULL if it does not exist.
        TclAction* objWithName(const char* name);

        // head of list
        TclAction* head() {return (TclAction*)SequentialList::head();}

	// Remove object from the list.  Note this does not delete it.
	// Returns TRUE if the object is removed.
	int remove(TclAction* o) { return SequentialList::remove(o);}
};

// an iterator for NamedObjList
class TclActionListIter : private ListIter {
public:
        TclActionListIter(TclActionList& sl);
        TclAction* next() { return (TclAction*)ListIter::next();}
        TclAction* operator++(POSTFIX_OP) { return next();}
        ListIter::reset;
};


/////////////////////////////////////////////////////////////////////
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

	// function to check error/abort status
	int checkErrorAbort();

	// This is added to support tycho - eal
	// The following flag determines whether ptcl will call the tcl
	// procedure monitorPtcl every time a ptcl command is invoked.
	// This procedure can be redefined in Tcl to monitor all commands
	// executed.
	static int monitor;

protected:
	// these two functions are used to associate PTcl objects
	// with interpreters.
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

	// return a const char* or a StringList result to Tcl
	int result(const char* str);

	// return a "static result" to Tcl.  Don't give this one a StringList!
	int staticResult(const char*);

	// append a value to the result, using Tcl_AppendElement.
	void addResult(const char*);

	// compute the schedule
	int computeSchedule();

public:
	// This function is used to associate PTcl objects
	// with interpreters.
	static PTcl* findPTcl(Tcl_Interp*);

	// the active Tcl interpreter, for error reporting.
	static Tcl_Interp* activeInterp;

	PTcl(Tcl_Interp* interp = 0);
	~PTcl();

	// If there is a universe by the given name in this PTcl object,
	// return it.  Else return NULL.
	InterpUniverse* univWithName(const char* name)
	  { return univs.univWithName(name); }

	// dispatcher is called by Tcl to handle all extension commands.
	static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);

// the following are the Tcl-callable functions.  Each returns TCL_OK
// or TCL_ERROR, and may set the Tcl result to return a string using
// Tcl_SetResult, Tcl_AppendElement, PTcl::result or PTcl::staticResult.
	int abort(int argc,char** argv);
	int alias(int argc,char** argv);
	int animation(int argc,char** argv);
	int busconnect(int argc,char** argv);
	int cancelAction(int argc,char** argv);
	int connect(int argc,char** argv);
	int cont(int argc,char** argv);
	int curuniverse(int argc,char** argv);
	int defgalaxy(int argc,char** argv);
	int delnode(int argc,char** argv);
	int delstar(int argc,char** argv);
	int deluniverse(int argc,char** argv);
	int descriptor(int argc,char** argv);
	int description(int argc,char** argv);
	int disconnect(int argc,char** argv);
	int domain(int argc,char** argv);
	int domains(int argc,char** argv);
	int exit(int argc,char** argv);
	int halt(int argc,char** argv);
	int initialize(int argc,char** argv);
	int isgalaxy(int argc,char** argv);
	int iswormhole(int argc,char** argv);
	int knownlist(int argc,char** argv);
	int link(int argc,char** argv);
	int listobjs(int argc,char** argv);
	int matlab(int argc,char** argv);
	int mathematica(int argc,char** argv);

	// added to support tycho - eal
	int monitorOff(int argc,char** argv);
	int monitorOn(int argc,char** argv);
	int monitorPtcl(int argc,char** argv);

	int multilink(int argc,char** argv);
	int newstate(int argc,char** argv);
	int newuniverse(int argc,char** argv);
	int node(int argc,char** argv);
	int nodeconnect(int argc,char** argv);
	int numports(int argc,char** argv);
	int pragma(int argc,char** argv);
	int pragmaDefaults(int argc,char** argv);
	int print(int argc,char** argv);
	int renameuniv(int argc,char** argv);
	int registerAction(int argc,char** argv);
	int reset(int argc,char** argv);
	int run(int argc,char** argv);
        int stoptime(int argc,char** argv);
	int schedtime(int argc,char** argv);
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
#ifdef PT_PTCL_WITH_TK
	int updateTk(int argc, char **argv);
#endif
	int wrapup(int argc,char** argv);
};

#endif
