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
#include "Scheduler.h"

// we want to be able to map Tcl_interp pointers to PTcl objects.
// this is done with a table storing all the PTcl objects.

// for now, we allow up to MAX_PTCL PTcl objects at a time.
const int MAX_PTCL = 10;

static PTcl* ptable[MAX_PTCL];

/////////////////////////////////////////////////////////////////////

// this is a static function.
PTcl* PTcl::findPTcl(Tcl_Interp* arg) {
	for (int i = 0; i < MAX_PTCL; i++)
		if (ptable[i]->interp == arg) return ptable[i];
	return 0;
}

void PTcl::makeEntry() {
	int i = 0;
	while (ptable[i] != 0 && i < MAX_PTCL) i++;
	if (i >= MAX_PTCL) return;
	ptable[i] = this;
}

void PTcl::removeEntry() {
	for (int i = 0; i < MAX_PTCL; i++) {
		if (ptable[i] == this) {
			ptable[i] = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////

// constructor
PTcl::PTcl(Tcl_Interp* i) : interp(i), universe(0), currentGalaxy(0),
currentTarget(0), definingGal(0)
{
	// perhaps default domain should be obtained some other way.
	curDomain = KnownBlock::defaultDomain();
	reset(1,0);
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
	removeEntry();
	if (myInterp) {
		Tcl_DeleteInterp(interp);
		interp = 0;
	}
}

// Create a new universe with name name and domain dom and make it the
// current universe.  Whatever universe was previously the current universe
// is not affected, unless it was named <name>.

void PTcl::newUniv(const char* name, const char* dom) {
	univs.delUniv(name);
	LOG_NEW; universe = new InterpUniverse(name, dom);
	currentGalaxy = universe;
	curDomain = universe->domain();
	currentTarget = universe->myTarget();
	univs.put(*universe);
}

// Delete the universe and make another
int PTcl::delUniv (const char* nm) {
	int remake = (strcmp(nm,universe->name()) == 0);
	if (!univs.delUniv(nm)) return FALSE;
	if (remake) newUniv("main", curDomain);
	return TRUE;
}

// Return a "usage" error
int PTcl::usage(const char* msg) {
	Tcl_AppendResult(interp,"incorrect usage: should be \"",msg,"\"",
			(char*)NULL);
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
// matches current target.  Otherwise, first search the current galaxy,
// then the known list, finally known targets.

	if (!name || *name == 0 || strcmp(name,"this") == 0)
		return currentGalaxy;
	if (strcmp(name,"target") == 0)
		return definingGal ? currentTarget : universe->myTarget();
	const Block* b = currentGalaxy->blockWithDottedName(name);
	if (!b) b = KnownBlock::find(name, curDomain);
	if (!b) b = KnownTarget::find(name);
	if (!b) Tcl_AppendResult(interp,"No such block: ",name,(char*)NULL);
	return b;
}

// descriptor: print the descriptor of the current galaxy, or of
// a block in the galaxy or on the known list.

int PTcl::descriptor(int argc,char** argv) {
	if (argc > 2) return usage("descriptor ?<block-or-classname>?");
	const Block* b = getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	return staticResult(b->descriptor());
}

int PTcl::print(int argc,char** argv) {
	if (argc > 2) return usage("print ?<block-or-classname>?");
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
		return usage("connect <src> <port> <dst> <port> ?<delay>?");
	if (!currentGalaxy->connect(argv[1],argv[2],argv[3],argv[4],argv[5]))
		return TCL_ERROR;
	return TCL_OK;
}

// busconnect
int PTcl::busconnect(int argc,char** argv) {
	if (argc != 6 && argc != 7)
		return usage("busconnect <src> <port> <dst> <port> <width> ?<delay>?");
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
	int num;
	if (Tcl_GetInt(interp, argv[3], &num) != TCL_OK)
		return TCL_ERROR;
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
		return usage("nodeconnect <star> <port> <node> ?<delay>?");
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

// newstate: create a new instance of state within galaxy
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
		return usage("setstate <star> <name> <value>\nor setstate this <name> <value>");
	if (!currentGalaxy->setState(argv[1],argv[2],argv[3]))
		return TCL_ERROR;
	return TCL_OK;
}

static const State* findState(const Block* b, const char* nm) {
	CBlockStateIter next(*b);
	const State* s;
	while ((s = next++)) {
		if (strcmp(nm,s->name()) == 0) return s;
	}
	return 0;
}

// statevalue: return a state value
// syntax: statevalue <block> <state> ?current/initial?
// default is current value
int PTcl::statevalue(int argc,char ** argv) {
	if (argc < 3 || argc > 4 ||
	    (argc == 4 &&argv[3][0] != 'c' && argv[3][0] != 'i'))
		return usage ("statevalue <block> <state> ?current/initial?");
	const Block* b = getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	const State* s = findState(b, argv[2]);
	if (!s) {
		Tcl_AppendResult(interp, "No state named ", argv[2],
					 " in block ", argv[1], (char*)NULL);
		return TCL_ERROR;
	}
	// return initial value if asked.
	if (argc == 4 && argv[3][0] == 'i')
		return staticResult(s->initValue());
	else
		return result(s->currentValue());
}

// defgalaxy: define a galaxy
int PTcl::defgalaxy(int argc,char ** argv) {
	if (argc != 3)
		return usage("defgalaxy <galname> {<galaxy-building-commands>}");
	if (definingGal) {
		Tcl_SetResult(interp, "already defining a galaxy!", TCL_STATIC);
		return TCL_ERROR;
	}
	const char* galname = hashstring(argv[1]);
	const char* outerDomain = curDomain;
	definingGal = TRUE;	// prevent recursive defgalaxy
	LOG_NEW; currentGalaxy = new InterpGalaxy(galname,curDomain);
	currentGalaxy->setBlock (galname, 0);
	currentTarget = 0;
	int status;  // return value
	if ((status = Tcl_Eval(interp, argv[2], 0, 0)) != TCL_OK) {
		LOG_DEL; delete currentGalaxy;
		Error::error("Error in defining galaxy ", galname);
	}
	else currentGalaxy->addToKnownList(outerDomain,currentTarget);
	currentGalaxy = universe;
	definingGal = FALSE;
	curDomain = outerDomain;
	return status;
}

int PTcl::computeSchedule() {
	SimControl::clearHalt();
	universe->initTarget();
	if (SimControl::flagValues() & SimControl::error) {
		Error::error("Error setting up the schedule.");
		return FALSE;
	}
	return TRUE;
}

int PTcl::schedule(int argc,char **) {
	if (argc > 1)
		return usage("schedule");
	// should arrange so that previously computed schedule can
	// be returned
	if (computeSchedule()) {
		return result(universe->displaySchedule());
	}
	else return TCL_ERROR;
}

// return the current time from the top-level scheduler of the current
// universe.  If the target has a state named "schedulePeriod", the
// returned time is divided by this value, unless a second argument
// with the value "actual" appears.

int PTcl::schedtime(int argc,char **argv) {
	int actualFlag = (argc == 2 && strcmp(argv[1], "actual") == 0);
	if (argc > 2 || argc == 2 && !actualFlag)
		return usage("schedtime [actual]");
	Scheduler *sch = universe->scheduler();
	if (sch) {
		double time = sch->now();
		if (!actualFlag) {
			State* s = universe->myTarget()->stateWithName
				("schedulePeriod");
			if (s) {
				time /= atof(s->currentValue());
			}
		}
		sprintf (interp->result, "%g", time);
		return TCL_OK;
	}
	else {
		strcpy (interp->result, "no scheduler!");
		return TCL_ERROR;
	}
}

int PTcl::run(int argc,char ** argv) {
	if (argc > 2)
		return usage("run ?<stoptime>?");
	if (!computeSchedule())
		return TCL_ERROR;
	stopTime = 0.0;
	lastTime = 1.0;
	return cont(argc,argv);
}

int PTcl::cont(int argc,char ** argv) {
	if (argc > 2)
		return usage("cont ?<lasttime>?");
	if (argc == 2) {
		double d;
		if (Tcl_GetDouble(interp, argv[1], &d) != TCL_OK)
			return TCL_ERROR;
		lastTime = d;
	}
	stopTime += lastTime;
	universe->setStopTime(stopTime);
	universe->run();
	return (SimControl::flagValues() & SimControl::error) ?
		TCL_ERROR : TCL_OK;
}

int PTcl::wrapup(int argc,char **) {
	if (argc > 1)
		return usage("wrapup");
	universe->wrapup();
	return (SimControl::flagValues() & SimControl::error) ?
		TCL_ERROR : TCL_OK;
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

// set (change) the seed of the random number generation
int PTcl::seed(int argc,char ** argv) {
	extern ACG* gen;
	if (argc > 2)
		return usage ("seed ?<seed>?");
	int val = 1;  // default seed
	if (argc == 2 && Tcl_GetInt(interp, argv[1], &val) != TCL_OK) {
		return TCL_ERROR;
	}
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
		Tcl_SetResult(interp,
			textAnimationState() ? "on" : "off",
			TCL_STATIC);
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
	if (argc > 2)
		return usage ("knownlist ?<domain>?");
	const char* dom = curDomain;
	if (argc == 2) dom = argv[1];
	KnownBlockIter nextB(dom);
	const Block* b;
	while ((b = nextB++) != 0)
		addResult(b->name());
	return TCL_OK;
}

int PTcl::topblocks (int argc,char ** argv) {
	if (argc > 2)
		return usage ("topblocks ?<block-or-classname>?");
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

// return the list of states, ports, or multiports in the given block.
// No arg: states in current galaxy.
int PTcl::listobjs (int argc,char ** argv) {
	static char use[] =
		"listobjs [states|ports|multiports] ?<block-or-classname>?";
	if (argc > 3 || argc <= 1)
		return usage (use);
	const Block* b = getBlock(argv[2]);
	if (strcmp(argv[1], "states") == 0) {
		CBlockStateIter nexts(*b);
		const State* s;
		while ((s = nexts++) != 0)
			addResult(s->name());
	}
	else if (strcmp(argv[1], "ports") == 0) {
		CBlockPortIter nexts(*b);
		const PortHole* s;
		while ((s = nexts++) != 0)
			addResult(s->name());
	}
	else if (strcmp(argv[1], "multiports") == 0) {
		CBlockMPHIter nexts(*b);
		const MultiPortHole* s;
		while ((s = nexts++) != 0)
			addResult(s->name());
	}
	else return usage(use);
	return TCL_OK;
}

int PTcl::reset(int argc,char** argv) {
	if (argc > 2)
		return usage ("reset ?<name>");
	const char* nm = "main";
	if (argc == 2) nm = hashstring(argv[1]);
	newUniv(nm, curDomain);
	return TCL_OK;
}

// Create a new, empty universe named <name> (default main) and make it
// the current  universe with domain <dom> (default current domain).  If
// there was previously a universe with this name, it is  deleted.
// Whatever universe was previously the current universe is not affected,
// unless it was named <name>. 

int PTcl::newuniverse(int argc,char** argv) {
	if (argc > 3)
		return usage("newUniverse ?<name> ?<dom>");
	if (argc == 3) curDomain = hashstring(argv[2]);
	const char* nm = "main";
	if (argc > 1) nm = hashstring(argv[1]);
	newUniv(nm, curDomain);
	return TCL_OK;
}

// deluniverse <name>
// Delete the named universe.  If argument is not given, delete the
// current universe.

int PTcl::deluniverse(int argc,char** argv) {
	if (argc > 2)
		return usage("delUniverse ?<name>");
	const char* nm = argv[1];
	if (argc == 1) nm = universe->name();
	delUniv(nm);
	return TCL_OK;
}

int PTcl::curuniverse(int argc,char** argv) {
	if (argc == 1) {
		Tcl_AppendResult(interp,universe->name(),(char*)NULL);
		return TCL_OK;
	}
	else if (argc == 2) {
		InterpUniverse* u = univs.univWithName(argv[1]);
		if (u) {
			universe = u;
			currentGalaxy = u;
			curDomain = u->domain();
			currentTarget = u->myTarget();
			return TCL_OK;
		}
		else {
			Error::error("No such universe: ", argv[1]);
			return TCL_ERROR;
		}
	}
	else return usage("curuniverse ?<name>");
}

// renameuniv <newname>: rename current univ to newname.
// renameuniv <oldname> <newname>: rename <oldname> to <newname>.

int PTcl::renameuniv(int argc,char ** argv) {
	InterpUniverse* u = universe;
	const char* newname = argv[1];
	if (argc < 2 || argc > 3)
		return usage(
		  "renameuniv <newname> or renameuniv <oldname> <newname>");
	if (argc == 3) {
		u = univs.univWithName(argv[1]);
		if (!u) {
			Error::error("No such universe: ", argv[1]);
			return TCL_ERROR;
		}
		// rename with same name has no effect.
		if (strcmp(argv[1],argv[2]) == 0) return TCL_OK;
		newname = argv[2];
	}
	univs.delUniv(newname);
	u->setNameParent(hashstring(newname),0);
	return TCL_OK;
}

int PTcl::univlist(int argc,char **) {
	if (argc > 1) return usage("univlist");
	IUListIter next(univs);
	InterpUniverse* u;
	while ((u = next++) != 0) {
		addResult(u->name());
	}
	return TCL_OK;
}

// return true if Target *t is a legal target for the domain domName.

static int legalTarget(const char* domName, Target* t) {
	const char* targetName = t->name();
	const int MAX_NAMES = 40;
	const char *names[MAX_NAMES];
	int n = KnownTarget::getList (domName, names, MAX_NAMES);
	for (int i = 0; i < n; i++)
		if (strcmp(names[i], targetName) == 0) return TRUE;
	return FALSE;
}

// display or change the domain.  If on top level, the existing target
// is not legal for the new domain, it reverts to to the default for the
// new domain.

int PTcl::domain(int argc,char ** argv) {
	if (argc > 2)
		return usage ("domain ?<newdomain>?");
	if (argc == 1) {
		Tcl_AppendResult(interp,curDomain,(char*)NULL);
		return TCL_OK;
	}

	// check to see if domain already matches.  If so do nothing.
	if (strcmp(currentGalaxy->domain(), argv[1]) == 0) return TCL_OK;

	// change the domain.
	if (! currentGalaxy->setDomain (argv[1]))
		return TCL_ERROR;
	curDomain = currentGalaxy->domain();

	// check to see if existing target is legal for the new domain,
	// if not, revert to default target.
	if (!definingGal) {
		if (!legalTarget(curDomain, universe->myTarget())) {
			universe->newTarget ();
			currentTarget = universe->myTarget();
		}
	}
	return TCL_OK;
}

// display or change the target
int PTcl::target(int argc,char ** argv) {
	if (argc == 1) {
		Target* tp = definingGal ? currentTarget : universe->myTarget();
		return staticResult(tp->name());
	}
	else if (argc > 2) {
		return usage("target ?<targetname>?");
	}
	else {
		const char* tname = hashstring(argv[1]);
		int status;
		if (!definingGal) {
			status = universe->newTarget(tname);
			currentTarget = universe->myTarget();
		} else {
		// shouldn't need this test. Compiler bug?
			if (currentTarget) { LOG_DEL; delete currentTarget; }
			currentTarget = KnownTarget::clone(tname);
			status = (currentTarget != 0);
		}
		return status ? TCL_OK : TCL_ERROR;
	}
}

int PTcl::targets(int argc,char** argv) {
	if (argc > 2) return usage("targets ?<domain>?");
	const char* domain;
	if (argc == 2) domain = argv[1];
	else domain = curDomain;
	const int MAX_NAMES = 40;
	const char *names[MAX_NAMES];
	int n = KnownTarget::getList (curDomain, names, MAX_NAMES);
	for (int i = 0; i < n; i++)
		addResult(names[i]);
	return TCL_OK;
}

// set (change) or return value of target parameter
int PTcl::targetparam(int argc,char ** argv) {
	if (argc != 2 && argc != 3)
		return usage("targetparam <name> ?<value>?");
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
	if (argc == 1) return usage("multilink <file1> ?<file2> ...?");
	return Linker::multiLink(argc,argv) ? TCL_OK : TCL_ERROR;
}

// Override tcl exit function with one that does cleanup of the universe(s).
int PTcl::exit(int argc,char ** argv) {
	int estatus = 0;
	if (argc > 2) return usage("exit ?<returnCode>?");
	if (argc == 2 && Tcl_GetInt(interp, argv[1], &estatus) != TCL_OK) {
		return TCL_ERROR;
	}
	univs.deleteAll();
	::exit (estatus);
	return TCL_ERROR;	// should not get here
}

// Request a halt of a running universe
int PTcl::halt(int argc,char ** argv) {
	SimControl::requestHalt();
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//			Register actions with SimControl
/////////////////////////////////////////////////////////////////////////////

// First, define the action function that will be called by the Ptolemy kernel.
// The string tclAction passed will be executed as a Tcl command.
static void ptkAction(Star* s, char *tclCommand)
{
    if(!tclCommand || *tclCommand == NULL) {
    	Tcl_Eval(PTcl::activeInterp,
	    "error {null pre or post action requested}",0, (char **) NULL);
	return;
    }
    Tcl_VarEval(PTcl::activeInterp,tclCommand," \"",
		(const char*)(s->fullName()), "\"",
		(char*)NULL);
}

// Destructor cancels an action and frees memory allocated for a TclAction
TclAction::~TclAction() {
    SimControl::cancel(action);
    delete [] tclCommand;
    delete [] name;
}

// constructor out-of-line to save code (esp. with cfront)
TclActionList::TclActionList() {}
// ditto for these.
TclActionListIter :: TclActionListIter(TclActionList& sl) : ListIter (sl) {}


// Return a pointer to the TclAction with the given name, if it exists.
// Otherwise, return 0.
TclAction* TclActionList::objWithName(const char* name) {
    TclAction *obj;
    ListIter next(*this);
    while ((obj = (TclAction*)next++) != 0) {
	if (strcmp(name,obj->name) == 0)
	return obj;
    }
    return 0;
}

static TclActionList tclActionList;

// Register a tcl command to be executed before or after the firing of any star.
// The function takes two arguments, the first of which must be "pre" or "post".
// This argument indicates whether the action should occur before or after
// the firing of a star.  The second argument is a string giving a tcl command.
// Before this command is invoked, however, the name of the star that triggered
// the action will be appended as an argument.  For example:
// 	registerAction pre puts
// will result in the name of a star being printed on the standard output
// before it is fired.  The value returned by registerAction is an
// "action_handle", which must be used to cancel the action using cancelAction.
//
int PTcl::registerAction(int argc,char ** argv) {
    if(argc != 3) return usage("registerAction <pre or post> <command>");
    int pre;
    if(strcmp("pre",argv[1]) == 0) pre = 1;
    else if(strcmp("post",argv[1]) == 0) pre = 0;
    else return usage("registerAction <pre or post> <command>");

    TclAction *tclAction;
    LOG_NEW; tclAction = new TclAction;
    tclAction->tclCommand = savestring(argv[2]);
    tclAction->action =
	SimControl::registerAction(ptkAction, pre, tclAction->tclCommand);

    // Create a unique name for the action
    char buf[32];
    sprintf(buf,"an%u",tclAction);
    tclAction->name = savestring(buf);

    // Register the action with the action list
    tclActionList.put(*tclAction);

    // The following guarantees to Tcl that the name will not be deleted
    // at least until the next call to Tcl_Eval.  This is safe because only
    // the cancelAction command below can delete this name.
    Tcl_SetResult(interp,tclAction->name,TCL_STATIC);
    return TCL_OK;
}

// Cancel a registered action
// The single argument is the action_handle returned by registerAction
//
int PTcl::cancelAction(int argc,char ** argv) {
    if(argc != 2) return usage("cancelAction <action_handle>");
    TclAction *tclAction;
    if(!(tclAction = tclActionList.objWithName(argv[1]))) {
	Tcl_AppendResult(interp,
	   "cancelAction: Failed to convert action handle", (char*) NULL);
	return TCL_ERROR;
    }
    tclActionList.remove(tclAction);
    LOG_DEL; delete tclAction;
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//				Function table
/////////////////////////////////////////////////////////////////////////////

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
	ENTRY(cancelAction),
	ENTRY(connect),
	ENTRY(cont),
	ENTRY(curuniverse),
	ENTRY(defgalaxy),
	ENTRY(delnode),
	ENTRY(delstar),
	ENTRY(deluniverse),
	ENTRY(descriptor),
	ENTRY(disconnect),
	ENTRY(domain),
	ENTRY(domains),
	ENTRY(exit),
	ENTRY(halt),
	ENTRY(knownlist),
	ENTRY(link),
	ENTRY(listobjs),
	ENTRY(multilink),
	ENTRY(newstate),
	ENTRY(newuniverse),
	ENTRY(node),
	ENTRY(nodeconnect),
	ENTRY(numports),
	ENTRY2(permlink,multilink),
	ENTRY(print),
	ENTRY(renameuniv),
	ENTRY(registerAction),
	ENTRY(reset),
	ENTRY(run),
	ENTRY(schedtime),
	ENTRY(schedule),
	ENTRY(seed),
	ENTRY(setstate),
	ENTRY(star),
	ENTRY(statevalue),
	ENTRY(target),
	ENTRY(targetparam),
	ENTRY(targets),
	ENTRY(topblocks),
	ENTRY(univlist),
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
		Tcl_SetResult(interp,
		       "Internal error in PTcl::dispatcher!",TCL_STATIC);
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

// IUList methods.

void IUList::put(InterpUniverse& u) {NamedObjList::put(u);}

int IUList::delUniv(const char* name) {
	InterpUniverse* z = univWithName(name);
	if (z) {
		remove(z);
		LOG_DEL; delete z;
	}
	return (z != 0);
}
