/**************************************************************************
Version identification:
$Id$

Copyright (c) 1997 The Regents of the University of California.
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
		       
 Programmer:  E. A. Lee, J. T. Buck, Brian Evans
 Date of creation: 2/97

This file implements a class that adds Ptolemy-specific Itcl commands to
an Itcl interpreter.  Commands are designed to resemble those of the Ptolemy
kernel, mostly.  These commands are defined in the ::pitcl namespace.
Normally, these commands should not be used directly.  Use instead the
class interface defined in the ::ptolemy namespace.

**************************************************************************/

#ifndef _PTcl_h
#define _PTcl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "tcl.h"
#include "NamedObj.h"
#include "SimControl.h"

#undef setstate                 /* work around bug in Linux headers */

class InterpUniverse;
class InterpGalaxy;
class Target;
class Block;
class StringList;
class InfString;
class GenericPort;
class PortHole;

/////////////////////////////////////////////////////////////////////
//// IUList
// A list of InterpUniverses.  The destructor deletes
// all universes on the list.
//
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

/////////////////////////////////////////////////////////////////////
//// IUListIter
// an iterator for IUList.
//
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
//// TclAction 
// A simple class that contains the relevant information
// about an action that has been registered via Tcl.
//
class TclAction {
public:
	char* name;
	SimAction *action;
	char* tclCommand;

	// Destructor cancels the action and
	// frees the memory associated with the above pointers.
	~TclAction();
};

/////////////////////////////////////////////////////////////////////
//// TclActionList
// A list of pre or post actions that have been
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

/////////////////////////////////////////////////////////////////////
//// TclActionListIter
// An iterator for TclActionList.
//
class TclActionListIter : private ListIter {
public:
        TclActionListIter(TclActionList& sl);
        TclAction* next() { return (TclAction*)ListIter::next();}
        TclAction* operator++(POSTFIX_OP) { return next();}
        ListIter::reset;
};


///////////////////////////////////////////////////////////////////////////
//// PTcl
// This class is the main interface between the Ptolemy kernel and
// Itcl. Commands in this class should not be used directly. Instead,
// the classes defined in the ::ptolemy namespace should be used.
// Because interaction with Itcl is not as rich as it might be, this
// implementation preserves from its predecessor the notion of a
// current galaxy and current universe. This seems to be the simplest
// approach, and is hidden from the user by the classes in the
// ::ptolemy namespace. For commands that operate on a block, the block
// can usually be a star, galaxy, universe, wormhole, or target. Names
// of blocks can take any of the following forms:
// <ol>
// <li> _name_
// <li> _name1.name2_
// <li> _name1.name2...nameN_
// <li> _.name_
// <li> _.name1.name2_
// <li> _.name1.name2...nameN_
// <li> this
// <li> target
// </ol>
// The first can specify a member of the current galaxy, a master star,
// a master galaxy, or a target (names conflicts are resolved in that order).
// The second can specify a star within a galaxy within the current galaxy,
// or a star or galaxy within a master galaxy.
// The third is just the obvious generalization of this.
// The fourth specifies a universe.
// The fifth specifies a block (star or galaxy) within a universe.
// The sixth is the obvious generalization.
// The seventh specifies the current galaxy, and the last specifies
// the current target.
//
class PTcl {

public:    
    // Constructor takes a Tcl interpreter as an optional argument.
    PTcl(Tcl_Interp* interp = 0);
    
    // Destructor.
    ~PTcl();

    ///////////////////////////////////////////////////////////////////////
    ////                Itcl callable public methods                   ////

    // These methods are actually called by the dispatcher method, which
    // is registered repeatedly in Itcl under a procedure name matching
    // the method name below.  The ClientData field of the registered
    // procedure is used to tell this dispatcher which method is intended.
    // Each method returns TCL_OK or TCL_ERROR, and may set the Tcl result to
    // return a string using Tcl_SetResult, Tcl_AppendElement, or any
    // of several auxiliary methods provided below.

    int abort(int argc,char** argv);
    int addBlock(int argc,char** argv);
    int addUniverse(int argc,char** argv);
    int aliasDown(int argc,char** argv);
    int aliasUp(int argc,char** argv);
    int animation(int argc,char** argv);
    int blocks(int argc,char** argv);
    int cancelAction(int argc,char** argv);
    int connect(int argc,char** argv);
    int connected(int argc,char** argv);
    int cont(int argc,char** argv);
    int currentGalaxy(int argc,char** argv);
    int defGalaxy(int argc,char** argv);
    int defWormhole(int argc,char** argv);
    int delnode(int argc,char** argv);
    int disconnect(int argc,char** argv);
    int exit(int argc,char** argv);
    int galaxyPort(int argc,char** argv);
    int getAnnotation(int argc,char** argv);
    int getClassName(int argc,char** argv);
    int getDescriptor(int argc,char** argv);
    int getDomain(int argc,char** argv);
    int getFullName(int argc,char** argv);
    int getParent(int argc,char** argv);
    int halt(int argc,char** argv);
    int initialize(int argc,char** argv);
    int isGalaxy(int argc,char** argv);
    int isMultiPort(int argc,char** argv);
    int isWormhole(int argc,char** argv);
    int knownBlocks(int argc,char** argv);
    int knownDomains(int argc,char** argv);
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
    int node(int argc,char** argv);
    int nodeconnect(int argc,char** argv);
    int pragma(int argc,char** argv);
    int pragmaDefaults(int argc,char** argv);
    int print(int argc,char** argv);
    int ports(int argc,char** argv);
    int portsContained(int argc,char** argv);
    int remove(int argc,char** argv);
    int renameuniv(int argc,char** argv);
    int registerAction(int argc,char** argv);
    int reset(int argc,char** argv);
    int run(int argc,char** argv);
    int stoptime(int argc,char** argv);
    int schedtime(int argc,char** argv);
    int schedule(int argc,char** argv);
    int seed(int argc,char** argv);
    int setstate(int argc,char** argv);
    int statevalue(int argc,char** argv);
    int target(int argc,char** argv);
    int targetparam(int argc,char** argv);
    int targets(int argc,char** argv);
    int univlist(int argc,char** argv);
    int wrapup(int argc,char** argv);


    ////////////////////////////////////////////////////////////////////////
    ////                   auxiliary public methods                     ////

    // The active Tcl interpreter associated with the PTcl object.
    static Tcl_Interp* activeInterp;

    // Dispatch the methods called from Tcl.
    static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);
    
    // Given a Tcl interpreter, return the associated PTcl object.
    static PTcl* findPTcl(Tcl_Interp*);
    
    // Return the full name of the specified block with a leading period.
    static InfString fullName(const NamedObj*);

    // If there is a universe by the given name in this PTcl object,
    // return it.  Otherwise return NULL.
    InterpUniverse* univWithName(const char* name)
           { return univs.univWithName(name); }
    
protected:

    ////////////////////////////////////////////////////////////////////////
    ////                       protected methods                        ////

    // Append a string to the Tcl result as a list element.
    void addResult(const char*);
    
    // Compute the schedule for the current universe.
    int computeSchedule();

    // Iterate over the blocks inside a galaxy, adding names to the result.
    void galTopBlockIter (const Block* b, int deep, int fullname);

    // Return a pointer to a block given its name.
    const Block* getBlock(const char*);

    // Return a pointer to the block containing the named port or state.
    const Block* getParentBlock(const char*);
    
    // Return a pointer to the galaxy within which the named block would be.
    InterpGalaxy* getParentGalaxy(const char*);
    
    // Return a pointer to a port with the given name.
    GenericPort* getPort(const char*);
    
    // Add this object to the table of PTcl objects.
    void makeEntry();

    // Create a new universe with the specified name and domain.
    void newUniv(const char* nm, const char* dom);
    
    // Remove this object from the table of PTcl objects.
    void removeEntry();
    
    // Return a const char* or a StringList result to Tcl.
    int result(const char* str);
    
    // Return the name after the last period.
    const char* rootName(const char*);

    // Return a static string (quoted constant) to Tcl.
    int staticResult(const char*);
    
    // Return a usage error in standard form.
    int usage(const char*);
    
private:

    ////////////////////////////////////////////////////////////////////////
    ////                       private methods                          ////

    // Function to check error/abort status.
    int checkErrorAbort();

    // Add to the Tcl result a list of ports connected to the given port.
    int getFarPorts(PortHole*, int);
    
    // Recursively descend within the galaxy to get a block with dotted name.
    const Block* getSubBlock(const char*, const Block*);

    // Function to register extensions with the Tcl interpreter.
    void registerFuncs();
    
    ////////////////////////////////////////////////////////////////////////
    ////                       private variables                        ////

    // NOTE:  Annoyingly, C++ requires that these appear in the order in
    // which they will be initialized, which is not alphabetical.

    // the current Ptolemy universe
    InterpUniverse* universe;

    // The current domain.
    const char* curdomain;
    
    // The current galaxy or universe.
    InterpGalaxy* curgalaxy;
    
    // Target for the current galaxy or universe.
    Target* curtarget;
    
    // The Tcl interpreter associated with this object.
    Tcl_Interp* interp;
    
    // The time at which we want the current execution to terminate.
    double stopTime;

    // FIXME: What is this?
    double lastTime;

    // Flag set while defining a galaxy.  FIXME: needed?
    short definingGal;

    // The following are not initialized by the constructor.

    // If set, call the tcl procedure monitorPtcl every time a ptcl
    // command is invoked.
    static int monitor;

    // Flag to indicate that interp is owned by me.
    short myInterp;
    
    // The list of known universes.
    IUList univs;
};

#endif
