/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.
		       
 Programmer:  J. T. Buck
 Date of creation: 3/4/92

This file implements a class that adds Ptolemy-specific Tcl commands to
a Tcl interpreter.  Commands are designed to resemble those of the earlier
interpreter.

**************************************************************************/

#ifndef _PTcl_h
#define _PTcl_h 1
#include "tcl.h"

#ifdef __GNUG__
#pragma interface
#endif

class InterpUniverse;
class InterpGalaxy;
class Target;
class Block;
class StringList;

class PTcl {
private:
	// the Ptolemy universe
	InterpUniverse* universe;

	// current galaxy being built
	InterpGalaxy* currentGalaxy;

	// target for the current galaxy
	Target* currentTarget;

	// the Tcl interpreter
	Tcl_Interp* interp;

	// runtime control
	double stopTime;
	double lastTime;

	// flag set while defining a galaxy
	short definingGal;

	// flag to indicate that interp is owned by me
	short myInterp;

	// function to register extensions with the Tcl interpreter
	void registerFuncs();

protected:
	// these three functions are used to associate PTcl objects
	// with interpreters.
	static PTcl* findPTcl(Tcl_Interp*);
	void makeEntry();
	void removeEntry();

	// create a new universe
	void newUniverse();

	// recreate the univers
	void resetUniverse();

	// return a usage error
	int usage(const char*);

	// find blocks for info-showing commands
	const Block* getBlock(const char*);

	// return a StringList result with proper cleanup as the
	// result of a Tcl command.
	int result(StringList&);

	// return a "static result".  Don't give this one a stringlist!
	int staticResult(const char*);

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
	int busconnect(int argc,char** argv);
	int connect(int argc,char** argv);
	int cont(int argc,char** argv);
	int defgalaxy(int argc,char** argv);
	int delnode(int argc,char** argv);
	int delstar(int argc,char** argv);
	int descriptor(int argc,char** argv);
	int disconnect(int argc,char** argv);
	int domain(int argc,char** argv);
	int domains(int argc,char** argv);
	int knownlist(int argc,char** argv);
	int link(int argc,char** argv);
	int newstate(int argc,char** argv);
	int node(int argc,char** argv);
	int nodeconnect(int argc,char** argv);
	int numports(int argc,char** argv);
	int printVerbose(int argc,char** argv);
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
	int wrapup(int argc,char** argv);
};

#endif
