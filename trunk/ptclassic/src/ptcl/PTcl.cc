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
static const char file_id[] = "PTcl.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "PTcl.h"
#include "InterpUniverse.h"
#include "Target.h"
#include "KnownTarget.h"
#include "KnownBlock.h"
#include "Domain.h"
#include <ACG.h>
#include "Linker.h"
#include "textAnimate.h"
#include "SimControl.h"
#include "ConstIters.h"

// we want to be able to map Tcl_interp pointers to PTcl objects.
// this is done with a mapping table.

// for now, we allow up to MAX_PTCL PTcl objects at a time.
const int MAX_PTCL = 10;

struct PMap {
	Tcl_Interp* interp;
	PTcl* pobj;
};

static PMap ptable[MAX_PTCL];

/////////////////////////////////////////////////////////////////////

// this is a static function.
PTcl* PTcl::findPTcl(Tcl_Interp* arg) {
	for (int i = 0; i < MAX_PTCL; i++)
		if (ptable[i].interp == arg) return ptable[i].pobj;
	return 0;
}

void PTcl::makeEntry() {
	int i = 0;
	while (ptable[i].interp != 0 && i < MAX_PTCL) i++;
	if (i >= MAX_PTCL) return;
	ptable[i].interp = interp;
	ptable[i].pobj = this;
}

void PTcl::removeEntry() {
	for (int i = 0; i < MAX_PTCL; i++) {
		if (ptable[i].pobj == this) {
			ptable[i].interp = 0;
			ptable[i].pobj = 0;
		}
	}
}
/////////////////////////////////////////////////////////////////////

// constructor
PTcl::PTcl(Tcl_Interp* i) : interp(i), universe(0), currentGalaxy(0),
currentTarget(0), definingGal(0)
{
	newUniverse();
	if (!interp) {
		interp = Tcl_CreateInterp();
		myInterp = TRUE;
	}
	else myInterp = FALSE;
	makeEntry();
	registerFuncs();
}

// destructor
PTcl::~PTcl() {
	LOG_DEL; delete universe;
	removeEntry();
	if (myInterp) { LOG_DEL; delete interp;}
}

// Attach the PTcl to a new universe
void PTcl::newUniverse () {
	LOG_NEW; universe = new InterpUniverse;
	currentGalaxy = universe;
}

// Delete the universe and make another
void PTcl::resetUniverse () {
	LOG_DEL; delete universe;
	newUniverse ();
}

// Return a "usage" error
int PTcl::usage(const char* msg) {
	strcpy(interp->result,"wrong # args: should be \"");
	strcat(interp->result,msg);
	strcat(interp->result,"\"");
	return TCL_ERROR;
}

// Return a static result.  Typical usage: return staticResult("foo")
int PTcl::staticResult(const char* value) {
	Tcl_SetResult(interp,(char*)value,TCL_STATIC);
	return TCL_OK;
}

// Return a StringList result.
// We arrange for Tcl to copy the value.
int PTcl::result(StringList& value) {
	const char* str = value;
	// VOLATILE will tell Tcl to copy the value, so it is safe
	// if the StringList is deleted soon.
	Tcl_SetResult(interp, (char*)str,TCL_VOLATILE);
	return TCL_OK;
}

void PTcl::addResult(const char* value) {
	// cast-away-const needed to interface with Tcl.
	Tcl_AppendElement(interp,(char*)value,0);
}

const Block* PTcl::getBlock(const char* name) {
// if name is blank or "this", return current galaxy.  "target"
// matches current target.  Otherwise, first search top-blocks,
// then the known list, finally known targets.

	if (!name || *name == 0 || strcmp(name,"this") == 0)
		return currentGalaxy;
	if (strcmp(name,"target") == 0)
		return definingGal ? currentTarget : universe->myTarget();
	const Block* b = currentGalaxy->blockWithDottedName(name);
	if (!b) b = KnownBlock::find(name);
	if (!b) b = KnownTarget::find(name);
	if (!b) {
		strcpy(interp->result,"No such block: ");
		strcat(interp->result,name);
	}
	return b;
}

// descriptor: print the descriptor of the current galaxy, or of
// a block in the galaxy or on the known list.

int PTcl::descriptor(int argc,char** argv) {
	if (argc > 2) return usage("descriptor [<block-or-classname]");
	const Block* b = getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	return staticResult(b->descriptor());
}

int PTcl::print(int argc,char** argv) {
	if (argc > 2) return usage("print [<block-or-classname>]");
	const Block* b = getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	return result(b->print(0));
}

// star: create a new instance of star within galaxy
int PTcl::star(int argc,char** argv) {
	if (argc != 3) return usage("star <name> <class>");
	if (!currentGalaxy->addStar(argv[1], argv[2]))
		return TCL_ERROR;
	return TCL_OK;
}

// delstar: delete a star from the galaxy
int PTcl::delstar(int argc,char** argv) {
	if (argc != 2) return usage("delstar <name>");
	if (! currentGalaxy->delStar(argv[1]))
		return TCL_ERROR;
	return TCL_OK;
}

// connect:
int PTcl::connect(int argc,char** argv) {
	if (argc != 5 && argc != 6)
		return usage("connect <src> <port> <dst> <port> [<delay>]");
	if (!currentGalaxy->connect(argv[1],argv[2],argv[3],argv[4],argv[5]))
		return TCL_ERROR;
	return TCL_OK;
}

// busconnect
int PTcl::busconnect(int argc,char** argv) {
	if (argc != 6 && argc != 7)
		return usage("busconnect <src> <port> <dst> <port> <width> [<delay>]");
	if (!currentGalaxy->busConnect(argv[1],argv[2],argv[3],
				       argv[4],argv[5],argv[6]))
		return TCL_ERROR;
	return TCL_OK;
}

int PTcl::alias(int argc,char** argv) {
	if (argc != 4)
		return usage("alias <galport> <star> <starport>");
	if (!currentGalaxy->alias(argv[1],argv[2],argv[3]))
		return TCL_ERROR;
	return TCL_OK;
}

int PTcl::numports(int argc,char** argv) {
	if (argc != 4)
		return usage("numports <star> <multiport> <how-many>");
	int num = atoi(argv[3]);
	if (!currentGalaxy->numPorts (argv[1], argv[2], num))
		return TCL_ERROR;
	return TCL_OK;
}

// node: create a new node for netlist-style connections
int PTcl::node(int argc,char ** argv) {
	if (argc != 2)
		return usage("node <nodename>");
	if (!currentGalaxy->addNode (argv[1]))
		return TCL_ERROR;
	return TCL_OK;
}

// delnode: delete a node
int PTcl::delnode(int argc,char ** argv) {
	if (argc != 2)
		return usage("delnode <nodename>");
	if (!currentGalaxy->delNode (argv[1]))
		return TCL_ERROR;
	return TCL_OK;
}

// nodeconnect: connect a porthole to a node
int PTcl::nodeconnect(int argc,char ** argv) {
	if (argc != 4 && argc != 5)
		return usage("nodeconnect <star> <port> <node> [<delay>]");
	if (!currentGalaxy->nodeConnect(argv[1],argv[2],argv[3],argv[4]))
		return TCL_ERROR;
	return TCL_OK;
}

// disconnect: disconnect a porthole
int PTcl::disconnect(int argc,char ** argv) {
	if (argc != 3)
		return usage("disconnect <star> <port>");
	if(! currentGalaxy->disconnect(argv[1],argv[2]))
		return TCL_ERROR;
	return TCL_OK;
}

// state: create a new instance of state within galaxy
int PTcl::newstate(int argc,char ** argv) {
	if (argc != 4)
		return usage("newstate <name> <type> <defvalue>");
	if (!currentGalaxy->addState(argv[1],argv[2],argv[3]))
		return TCL_ERROR;
	return TCL_OK;
}

// setstate: set (change) a state within galaxy
int PTcl::setstate(int argc,char ** argv) {
	if (argc != 4)
		return usage("state <star> <name> <value>\nor state this <name> <value>");
	if (!currentGalaxy->setState(argv[1],argv[2],argv[3]))
		return TCL_ERROR;
	return TCL_OK;
}

// statevalue: return a state value
// syntax: statevalue block state [current/initial]
// default is current value
int PTcl::statevalue(int argc,char ** argv) {
	if (argc < 3 || argc > 4 ||
	    (argc == 4 &&argv[3][0] != 'c' && argv[3][0] != 'i'))
		return usage ("statevalue block state [current/initial]");
	Block* b = (Block*)getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	const State* s = b->stateWithName(argv[2]);
	if (!s) {
		sprintf (interp->result, "No state named '%s' in block '%s'",
			 argv[2], argv[1]);
		return TCL_ERROR;
	}
	// return initial value if asked.
	if (argc == 4 && argv[3][0] == 'i')
		return staticResult(s->initValue());
	else
		return result(s->currentValue());
}

// defgalaxy: define a galaxy
// syntax: defgalaxy <galname> { <galaxy-building-commands> }
int PTcl::defgalaxy(int argc,char ** argv) {
	if (argc != 3)
		return usage("defgalaxy <galname> {galaxy-building-stuff}");
	if (definingGal) {
		Tcl_SetResult(interp, "already defining a galaxy!", TCL_STATIC);
		return TCL_ERROR;
	}
	const char* galname = hashstring(argv[1]);
	const char* outerDomain = KnownBlock::domain();
	definingGal = TRUE;	// prevent recursive defgalaxy
	LOG_NEW; currentGalaxy = new InterpGalaxy(galname);
	currentGalaxy->setBlock (galname, universe);
	currentTarget = 0;
	if (Tcl_Eval(interp, argv[2], 0, 0) != TCL_OK) {
		LOG_DEL; delete currentGalaxy;
	}
	else currentGalaxy->addToKnownList(outerDomain,currentTarget);
	currentGalaxy = universe;
	definingGal = FALSE;
	return TCL_OK;
}

int PTcl::schedule(int argc,char **) {
	if (argc > 1)
		return usage("schedule");
	// should arrange so that previously computed schedule can
	// be returned
	universe->initTarget();
	if (SimControl::haltRequested()) {
		Error::error("Error setting up the schedule.");
		return TCL_ERROR;
	}
	return result(universe->displaySchedule());
}

int PTcl::run(int argc,char ** argv) {
	if (argc > 2)
		return usage("run <stoptime>");

	universe->initTarget();
	if (SimControl::haltRequested()) {
		Error::error("Error setting up the schedule.");
		return TCL_ERROR;
	}
	stopTime = 0.0;
	lastTime = 1.0;
	return cont(argc,argv);
}

int PTcl::cont(int argc,char ** argv) {
	if (argc > 2)
		return usage("cont <stoptime>");
	if (argc == 2)
		lastTime = atof (argv[1]);
	stopTime += lastTime;
	universe->setStopTime(stopTime);
	universe->run();
	return SimControl::haltRequested() ? TCL_ERROR : TCL_OK;
}

int PTcl::wrapup(int argc,char **) {
	if (argc > 1)
		return usage("wrapup");
	universe->wrapup();
	return SimControl::haltRequested() ? TCL_ERROR : TCL_OK;
}

// domains: list domains
int PTcl::domains(int argc,char **) {
	if (argc > 1)
		return usage("domains");
	int n = Domain::number();
	for (int i = 0; i < n; i++) {
		addResult(Domain::nthName(i));
	}
	return TCL_OK;
}

// read the seed of the random number generation
int PTcl::seed(int argc,char ** argv) {
	extern ACG* gen;
	if (argc > 2)
		return usage ("seed [<seed>]");
	int val = 1;
	if (argc == 2) val = atoi(argv[1]);
	LOG_DEL; delete gen;
	LOG_NEW; gen = new ACG(val);
	return TCL_OK;
}

// animation control
int PTcl::animation (int argc,char** argv) {
	const char* t = "";
	int c;
	if (argc == 2) t = argv[1];
	if (argc > 2 ||
	    (argc == 2 && (c=strcmp(t,"on"))!=0 && strcmp(t,"off")!=0))
		return usage ("animation ?on/off?");
	if (argc != 2) {
		strcpy(interp->result, (c==0) ? "on" : "off");
	}
	else if (c == 0)
		textAnimationOn();
	else
		textAnimationOff();
	return TCL_OK;
}

// knownlist shows the available blocks in the current domain, or (if an
// argument is given) the supplied domain.
int PTcl::knownlist (int argc,char** argv) {
	if (argc > 3)
		return usage ("knownlist [<domain>]");
	KnownBlockIter nextB(argv[1]);
	const Block* b;
	while ((b = nextB++) != 0)
		addResult(b->name());
	return TCL_OK;
}

int PTcl::topblocks (int argc,char ** argv) {
	if (argc > 2)
		return usage ("topblocks [<block-or-classname>]");
	const Block* b = getBlock(argv[1]);
	if (!b->isItAtomic()) {
		CGalTopBlockIter nextb(b->asGalaxy());
		const Block* b;
		while ((b = nextb++) != 0)
			addResult(b->name());
	}
	// empty value returned for atomic blocks
	return TCL_OK;
}

int PTcl::reset(int argc,char**) {
	if (argc > 1)
		return usage ("reset");
	resetUniverse();
	return TCL_OK;
}

// display or change the domain.  If on top level, target always changes
// to the default for that domain.  Thus, if both a domain and a target
// are specified, domain must be first.
int PTcl::domain(int argc,char ** argv) {
	if (argc > 2)
		return usage ("domain [newdomain]");
	if (argc == 1) {
		strcpy(interp->result,KnownBlock::domain());
		return TCL_OK;
	}
	if (! currentGalaxy->setDomain (argv[1]))
		return TCL_ERROR;
	if (!definingGal) universe->newTarget ();
	return TCL_OK;
}

// display or change the target
int PTcl::target(int argc,char ** argv) {
	if (argc == 1) {
		Target* tp = definingGal ? currentTarget : universe->myTarget();
		return staticResult(tp->name());
	}
	else if (argc > 2) {
		return usage("target [<targetname>]");
	}
	else {
		const char* tname = hashstring(argv[1]);
		int status;
		if (!definingGal)
			status = universe->newTarget(tname);
		else {
		// shouldn't need this test. Compiler bug?
			if (currentTarget) { LOG_DEL; delete currentTarget; }
			currentTarget = KnownTarget::clone(tname);
			status = (currentTarget != 0);
		}
		return status ? TCL_OK : TCL_ERROR;
	}
}

int PTcl::targets(int argc,char** argv) {
	if (argc > 2) return usage("targets [<domain>]");
	const char* domain;
	if (argc == 2) domain = argv[1];
	else domain = KnownBlock::domain();
	const int MAX_NAMES = 40;
	const char *names[MAX_NAMES];
	int n = KnownTarget::getList (KnownBlock::domain(), names, MAX_NAMES);
	for (int i = 0; i < n; i++)
		addResult(names[i]);
	return TCL_OK;
}

// change target states
int PTcl::targetparam(int argc,char ** argv) {
	if (argc != 2 && argc != 3)
		return usage("targetparam <name> [<value>]");
	Target* t = universe->myTarget();
	if (definingGal && currentTarget) t = currentTarget;
	State* s = t->stateWithName(argv[1]);
	if (!s) {
		Error::error("No such target parameter: ", argv[1]);
		return TCL_ERROR;
	}
	if (argc == 1) {
		return staticResult(s->initValue());
	}
	s->setInitValue(hashstring(argv[2]));
	return TCL_OK;
}

// Run dynamic linker to load a file
int PTcl::link(int argc,char ** argv) {
	if (argc != 2) return usage("link <objfile>");
	if (!Linker::linkObj (argv[1]))
		return TCL_ERROR;
	return TCL_OK;
}

// Link multiple files.  If invoked as "permlink", the link will
// be permanent.

int PTcl::multilink(int argc,char ** argv) {
	if (argc == 1) return usage("multilink <file1> [<file2>...]");
	return Linker::multiLink(argc,argv) ? TCL_OK : TCL_ERROR;
}

// Override tcl exit function with one that does cleanup of the universe.
int PTcl::exit(int argc,char ** argv) {
	int estatus = 0;
	if (argc > 2) return usage("exit ?returnCode?");
	if (argc == 2 && Tcl_GetInt(interp,argv[1], &estatus) != TCL_OK) {
		return TCL_ERROR;
	}
	LOG_DEL; delete universe;
	::exit (estatus);
	return TCL_ERROR;	// should not get here
}

// An InterpFuncP is a pointer to an PTcl function that takes an argc-argv
// argument list and returns TCL_OK or TCL_ERROR.

typedef int (PTcl::*InterpFuncP)(int,char**);

struct InterpTableEntry {
	char* name;
	InterpFuncP func;
};

// Here is the function table and dispatcher function.
// These macros define entries for the table

#define ENTRY(verb) { quote(verb), PTcl::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), PTcl::action }

// Here is the table.  Make sure it ends with "0,0"
static InterpTableEntry funcTable[] = {
	ENTRY(alias),
	ENTRY(animation),
	ENTRY(busconnect),
	ENTRY(connect),
	ENTRY(cont),
	ENTRY(defgalaxy),
	ENTRY(delnode),
	ENTRY(delstar),
	ENTRY(descriptor),
	ENTRY(disconnect),
	ENTRY(domain),
	ENTRY(domains),
	ENTRY(exit),
	ENTRY(knownlist),
	ENTRY(link),
	ENTRY(multilink),
	ENTRY(newstate),
	ENTRY(node),
	ENTRY(nodeconnect),
	ENTRY(numports),
	ENTRY2(permlink,multilink),
	ENTRY(print),
	ENTRY(reset),
	ENTRY(run),
	ENTRY(schedule),
	ENTRY(seed),
	ENTRY(setstate),
	ENTRY(star),
	ENTRY(statevalue),
	ENTRY(target),
	ENTRY(targetparam),
	ENTRY(targets),
	ENTRY(topblocks),
	ENTRY(wrapup),
	0, 0
};

// register all the functions.
void PTcl::registerFuncs() {
	int i = 0;
	while (funcTable[i].name) {
		Tcl_CreateCommand (interp, funcTable[i].name,
				   PTcl::dispatcher, (ClientData)i, 0);
		i++;
	}
}

// static member: tells which Tcl interpreter is "innermost"
Tcl_Interp* PTcl::activeInterp = 0;

// dispatch the functions.
int PTcl::dispatcher(ClientData which,Tcl_Interp* interp,int argc,char* argv[])
			   {
	PTcl* obj = findPTcl(interp);
	if (obj == 0) {
		strcpy(interp->result,
		       "Internal error in PTcl::dispatcher!");
		return TCL_ERROR;
	}
	int i = int(which);
	// this code makes an effective stack of active Tcl interpreters.
	Tcl_Interp* save = activeInterp;
	activeInterp = interp;
	int status = (obj->*(funcTable[i].func))(argc,argv);
	activeInterp = save;
	return status;
}
