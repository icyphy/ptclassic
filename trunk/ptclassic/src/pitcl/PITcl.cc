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

Programmer:  E. A. Lee
Based on: PTcl design by Joe Buck
Date of creation: 2/97

This file implements a class that adds Ptolemy-specific Itcl
commands to an Itcl interpreter. The commands are designed to
resemble those of the Ptolemy kernel, mostly. These commands are
defined in the ::pitcl namespace. Normally, these commands should
not be used directly. Use instead the class interface defined in the
::ptolemy namespace.

**************************************************************************/
static const char file_id[] = "PITcl.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "isa.h"		// define the quote macro
#include "PITcl.h"

#include "InterpUniverse.h"
#include "Target.h"
#include "KnownTarget.h"
#include "KnownBlock.h"
#include "Domain.h"
#include <ACG.h>
#include <stream.h>
#include "Linker.h"
#include "textAnimate.h"
#include "SimControl.h"
#include "ConstIters.h"
#include "Scheduler.h"
#include "InfString.h"

#include "ptclDescription.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////                     Miscellaneous header data                       ////

/////////////////////////////////////////////////////////////////////////////
//// ptable
// We want to be able to map Tcl_interp pointers to PTcl ojects.
// This is done with a table storing all the PTcl objects.
// For now, we allow up to MAX_PTCL PTcl objects at a time.
//
const int MAX_PTCL = 10;
static PTcl* ptable[MAX_PTCL];

/////////////////////////////////////////////////////////////////////////////
//// DEFAULT_DOMAIN
// The default domain.  This must be defined somewhere else.
//
extern char DEFAULT_DOMAIN[];

/////////////////////////////////////////////////////////////////////////////
//// constructor
//
PTcl::PTcl(Tcl_Interp* i) : universe(0), currentGalaxy(0),
currentTarget(0), interp(i), stopTime(0.0), lastTime(1.0), definingGal(0)
{
    KnownBlock::setDefaultDomain(DEFAULT_DOMAIN);
    // Start up in the default domain.
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

/////////////////////////////////////////////////////////////////////////////
//// destructor
//
PTcl::~PTcl() {
    removeEntry();
    if (myInterp) {
        Tcl_DeleteInterp(interp);
        interp = 0;
    }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////                     Itcl callable public methods                    ////

///////////////////////////////////////////////////////////////////////////
//// addBlock
// Create a new instance of star, galaxy, or wormhole. The
// specified block name may be absolute or relative to the current
// galaxy, unless it is a universe, in which case it must be absolute.
// I.e., it must begin with a leading period.
// The galaxy within which the block is to be created must exist or an
// error is triggered.  An error is also triggered if the block already
// exists.
//
int PTcl::addBlock(int argc,char** argv) {
    if (argc != 3) return usage("addBlock <name> <class>");
    InterpGalaxy* saveGal = currentGalaxy;
    currentGalaxy = getParentGalaxy(argv[1]);
    if (currentGalaxy) {
        const char* root = rootName(argv[1]);
        if (currentGalaxy->blockWithName(root)) {
            InfString msg = "Block ";
            msg << root << " already exists in " << fullName(currentGalaxy);
            result(msg);
            currentGalaxy = saveGal;
            return TCL_ERROR;
        }
        if (!currentGalaxy->addStar(root, argv[2])) {
            currentGalaxy = saveGal;
            return TCL_ERROR;
        }
    } else {
        currentGalaxy = saveGal;
        return TCL_ERROR;
    }
    currentGalaxy = saveGal;
    return TCL_OK;
}

///////////////////////////////////////////////////////////////////////////
//// addUniverse
// Create a new, empty universe.  The name can begin with
// a period (as in ".foo") or can omit the period (as in "foo").  In either
// case, the full name of the resulting universe has a leading period (".foo").
// If a universe with the given name already exists, it is cleared.
// The second argument, if given, is the domain of the new universe.
// This defaults to the current domain.
//
int PTcl::addUniverse(int argc,char** argv) {
    if (argc > 3 || argc < 2) {
        return usage("newuniverse <name> ?<dom>?");
    }
    if (argc == 3) curDomain = hashstring(argv[2]);
    const char* nm = argv[1];
    if (*nm == '.') nm++;
    const char* name = hashstring(nm);
    newUniv(name, curDomain);
    return TCL_OK;
}

///////////////////////////////////////////////////////////////////////////
//// aliasDown
// Return the full name of the port that is aliased to the specified galaxy
// port.  If there is none, return an empty string.  If the -deep option
// is given, then return the bottom-level port, which is always a star port.
// Otherwise, just return the next level down.
//
int PTcl::aliasDown(int argc,char** argv) {
    int deep=0;
    char* portname=NULL;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-deep") == 0) deep = 1;
        else if (portname != NULL) return usage("aliasDown ?-deep? <portname>");
        else portname = argv[i];
    }
    if (portname == NULL) {
        return usage("aliasDown ?-deep? <portname>");
    }

    GenericPort* gp = getPort(portname);
    if (!gp) return TCL_ERROR;

    // If the port has a downward alias, and -deep was not specified,
    // then return the alias.
    if (!deep) {
        if (gp->alias()) gp = gp->alias();
        else return TCL_OK;
    } else {
        while(gp->alias()) gp = gp->alias();
    }
    result(fullName(gp));
    return TCL_OK;
}

///////////////////////////////////////////////////////////////////////////
//// aliasUp
// If the -deep option is not given, return the full name of the galaxy
// port that is aliased to the specified port at the next level up in the
// hierarchy, or an empty string if there is none. If the -deep option
// is given, then return the top-level galaxy port, which may in fact be
// the same as the specified port if that port is already at the top level.
//
// NOTE: If the specified port is a plain port within a multiport, the
// returned port may be a plain port within a multiport one level up in
// the hierarchy.  This port name can be used to disconnect, but not
// to recreate the topology.
//
int PTcl::aliasUp(int argc,char** argv) {
    int deep=0;
    char* portname=NULL;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-deep") == 0) deep = 1;
        else if (portname != NULL) return usage("aliasUp ?-deep? <portname>");
        else portname = argv[i];
    }
    if (portname == NULL) {
        return usage("aliasUp ?-deep? <portname>");
    }

    GenericPort* gp = getPort(portname);
    if (!gp) return TCL_ERROR;

    // If the port has an upward alias, and -deep was not specified,
    // then return the alias.
    if (!deep) {
        if (gp->aliasFrom()) gp = gp->aliasFrom();
        else return TCL_OK;
    } else {
        while(gp->aliasFrom()) gp = gp->aliasFrom();
    }
    return result(fullName(gp));
}

///////////////////////////////////////////////////////////////////////////
//// blocks
// Return a list of blocks contained by the specified block.
// <p>
// Usage: blocks ?-fullname? ?-deep? ?<blockname>?
// <p>
// If the -fullname option is given, the names in the returned list
// have the form _.universe.galaxy...galaxy.block_, giving the exact
// unique name of each block, with a leading period.
// If the -deep option is given, then recursively descend into
// galaxies, adding their blocks to the list, rather than listing
// the galaxy.  Thus, all blocks returned will be stars.  Note that
// probably makes no sense to use the -deep option without the
// -fullname option, since the names may not end up being unique.
//
// If no blockname is given, then the current galaxy is used.
// If the argument is the name of a star (and hence contains no
// blocks), then return an empty string.
//
int PTcl::blocks (int argc,char ** argv) {
    int deep=0;
    int fullname=0;
    char* blockname=NULL;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-deep") == 0) deep = 1;
        else if (strcmp(argv[i],"-fullname") == 0) fullname = 1;
        else if (blockname != NULL)
        return usage ("blocks ?-fullname? ?-deep? ?<blockname>?");
        else blockname = argv[i];
    }
    const Block *b;
    if (blockname == NULL) {
        b = getBlock("this");
    } else {
        b = getBlock(blockname);
    }
    if (!b) return TCL_ERROR;
    galTopBlockIter(b,deep,fullname);
    // empty list returned for atomic blocks
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// connect
// Usage:
// connect ?-bus _buswidth_? ?-note _annotation_? _srcport dstport_
//
// Form a connection between two ports.  The ports are given by dotted
// names that can be absolute (beginning with a period) or relative to the
// current galaxy.
// If a _buswidth_ option is given, then it specifies the number of parallel
// connections to make.  If the _annotation_ option is given, the annotation
// is recorded on the connection.  Such annotations are used, for example,
// in dataflow domains to specify the number and values of initial tokens that
// are put on an arc.  For such domains, the _annotation_ argument
// is a list giving the values of initial tokens that will be put on the arc.
//
int PTcl::connect(int argc,char** argv) {
    char* bus=NULL;
    char* annotation="";
    char* nmargs[2];
    int j=0;
    int err=0;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-bus") == 0) {
            bus = argv[++i];
            if (i>=argc) err=1;
        } else if (strcmp(argv[i],"-note") == 0) {
            annotation = argv[++i];
            if (i>=argc) err=1;
        } else if (j <= 1) {
            nmargs[j++] = argv[i];
        } else {
            err=1;
        }
    }
    if (err) return usage ("connect ?-bus <buswidth>? ?-note <annotation>? <srcport> <dstport>");

    const Block* src = getParentBlock(nmargs[0]);
    const Block* dst = getParentBlock(nmargs[1]);
    if (!src || !dst) return TCL_ERROR;
    Block* parent = src->parent();
    if (!parent || parent != dst->parent() || !parent->isA("InterpGalaxy")) {
        InfString msg = "Invalid connect command: ";
        msg << "source " << fullName(src) << " and destination " 
                << fullName(dst) 
                << " must be in a galaxy (and the same galaxy).";
        result(msg);
        return TCL_ERROR;
    }
    InterpGalaxy* parentg = (InterpGalaxy*)parent;
    // NOTE: The interface really should take over the functionality
    // of InterpGalaxy, in which case this will be of type Galaxy.
    if (bus==NULL) {
        if (!parentg->connect(src->name(), rootName(nmargs[0]),
                dst->name(), rootName(nmargs[1]),
                annotation)) return TCL_ERROR;
    } else {
        if (!parentg->busConnect(src->name(), rootName(nmargs[0]),
                dst->name(), rootName(nmargs[1]),
                bus, annotation)) return TCL_ERROR;
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// connected
// Return a list of ports to which the given port is connected, or an
// empty list if it is not connected. The full name of each port is
// returned. If the given port is the alias of a galaxy port and the
// -deep option is not given, then the galaxy port name is reported as
// {*aliasUp* _portname_}. If the -deep option is given, then a star port
// is always returned, never a galaxy port. That is, the star port to
// which the given port is ultimately connected to is returned. If the
// name of a multiport is given, this method returns a list of all the
// ports connected to the multiport.
//
// NOTE: If the far side port is a multiport, then the specific port
// within that multiport to which we are connected is returned.  This
// is not exactly the information that would be needed to recreate the
// topology (by issuing connect commands) since the connect command wants
// to be given the multiport name, not the specific inside port name.
// However, it is exactly the information needed to disconnect.
//
int PTcl::connected(int argc,char** argv) {
    int deep=0;
    char* portname=NULL;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-deep") == 0) deep = 1;
        else if (portname != NULL) {
            return usage ("connected ?-deep? <portname>");
        } else portname = argv[i];
    }
    if (portname == NULL) return usage ("connected ?-deep? <portname>");

    GenericPort* gp = getPort(portname);
    if (!gp) return TCL_ERROR;

    // If the port has an upward alias, and -deep was not specified,
    // then return the alias.
    if (!deep && gp->aliasFrom()) {
        InfString res = "aliasUp ";
        // FIXME: If the name has a space, this doesn't do the right thing.
        // Not a proper list element.
        res << fullName(gp->aliasFrom());
        addResult(res);
    }
    // Translate aliases downward, if any, to get the real port with
    // a connection
    while (gp->alias()) gp = gp->alias();

    // FIXME: What does this do if we are connected to a node?
    if (gp->isItMulti()) {
        // The port is a multiport.  Iterate over its constituent ports.
        MPHIter next(*((MultiPortHole*) gp));
        PortHole* ph;
        while ((ph = next++) != 0) {
            // FIXME: skip if there is an aliasFrom
            if (getFarPorts(ph,deep) != TCL_OK) return TCL_ERROR;
        }
        return TCL_OK;
    } else {
        // The port is a simple port.
        // NOTE: The following cast assumes that only PortHole and
        // MultiPortHole are derived from GenericPort.
        return getFarPorts((PortHole*)gp,deep);
    }
}

/////////////////////////////////////////////////////////////////////////////
//// curgalaxy
// Return the full name of current galaxy or universe if no argument is
// given. If a name is given and a galaxy or universe exists with that
// name, make that the current galaxy. If there is no such galaxy,
// return an error.
//
int PTcl::curgalaxy(int argc,char** argv) {
    if (argc == 1) {
        if (currentGalaxy != NULL) {
            InfString nm = fullName(currentGalaxy);
            result(nm);
        }
        return TCL_OK;
    } else if (argc == 2) {
        const Block* b = getBlock(argv[1]);
        if (!b) {
            return TCL_ERROR;
        }
        if (b->isA("InterpUniverse")) {
            InterpUniverse* u = (InterpUniverse*) b;
            universe = u;
            currentGalaxy = u;
            curDomain = u->domain();
            currentTarget = u->myTarget();
            return TCL_OK;
        }

        // Check to be sure this is a galaxy.
        if (!b->isA("InterpGalaxy")) {
            Tcl_AppendResult(interp, "No such galaxy: ",argv[1], (char*) NULL);
            return TCL_ERROR;
        }
        currentGalaxy = (InterpGalaxy*) b;
        curDomain = currentGalaxy->domain();
        if (b->isItWormhole()) {
            // A wormhole has a target too.
            currentTarget = ((Runnable*)b)->myTarget();
        }
        return TCL_OK;
    } else return usage("curgalaxy ?<name>?");
}

/////////////////////////////////////////////////////////////////////////////
//// defgalaxy
// Define a master galaxy.  The first argument is the name of the galaxy,
// i.e. the name that is registered on the knownlist for use by *addBlock*.
// The optional second argument is a set of galaxy building commands.
// If it is not given, an empty galaxy is defined.  Otherwise, the
// commands are executed in the ::pitcl namespace to construct the
// galaxy.  Note that if errors occur during the execution of these
// galaxy defining commands, then a partially constructed galaxy
// will be on the knownlist.
//
// FIXME: Change the name.
//
int PTcl::defgalaxy(int argc,char ** argv) {
    if (argc < 2 || argc > 3) {
        return usage("defgalaxy <galname> ?{<galaxy-building-commands>}?");
    }
    // Prevent recursive calls.
    if (definingGal) {
        Tcl_SetResult(interp, "already defining a galaxy!", TCL_STATIC);
        return TCL_ERROR;
    }
    const char* galname = hashstring(argv[1]);
    const char* outerDomain = curDomain;
    definingGal = TRUE;
    currentGalaxy = new InterpGalaxy(galname,curDomain);
    currentGalaxy->setBlock (galname, 0);
    currentTarget = 0;
    int status;  // return value
    currentGalaxy->addToKnownList("ptcl defgalaxy command",
            outerDomain, currentTarget);
    InfString cmd = "namespace ::pitcl {";
    cmd << argv[2] << "}";
    if ((status = Tcl_Eval(interp, (char*)cmd)) != TCL_OK) {
        Tcl_AppendResult(interp, 
                "Error in defining galaxy ", galname, (char*)NULL);
    }
    currentGalaxy = universe;
    currentTarget = universe->myTarget();
    definingGal = FALSE;
    curDomain = outerDomain;
    if (! currentTarget) {
        Tcl_AppendResult(interp, "Error in restoring outer universe target",
               (char*) NULL);
        return TCL_ERROR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////////////
//// galaxyPort
// This procedure establishes an "alias" relationship between a galaxy
// port and a port in a block within the galaxy.  The first argument is
// the name of the galaxy port.  If no port with that name exists, it is
// created.  The second argument is the name of a block port within the
// galaxy.
//
// NOTE: This procedure replaces the *alias* command in PTcl.
// Also note that this procedure could be greatly simplified by
// bypassing the InterpGalaxy::alias method.  We use that method
// only because it registers the operation on the "actionList" for
// later cloning of the galaxy.  If the cloning mechanism is simplified,
// then this implementation can probably be simplified.
//
int PTcl::galaxyPort(int argc,char** argv) {
    if (argc != 3) {
        return usage ("galaxyPort <portname> <aliasport>");
    }
    const Block* pb = getParentBlock(argv[1]);
    if (!pb || !pb->isA("InterpGalaxy")) {
        InfString msg = argv[1];
        msg << " is not a valid galaxy port name.";
        result(msg);
        return TCL_ERROR;
    }
    InterpGalaxy* gal = (InterpGalaxy*)pb;
    GenericPort* gp = getPort(argv[2]);
    if (!gp) return TCL_ERROR;
    if (!gp->parent()) {
        InfString msg = argv[2];
        msg << " has no parent block!";
        result(msg);
        return(TCL_ERROR);
    }

    // NOTE: this method will use the names to look up the pointers
    // again.  Room for efficiency improvement here.
    if (!gal->alias(rootName(argv[1]), gp->parent()->name(), gp->name())) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// getAnnotation
// Return the annotation created with the *-note* option for a given port.
// If no annotation was specified, return an empty string.
//
// NOTE: If a multiport is passed in, this method returns an empty string,
// indicating no annotation. Is this what we want?
//
int PTcl::getAnnotation(int argc,char** argv) {
    if (argc != 2) {
        return usage ("getAnnotation <portname>");
    }
    GenericPort* gp = getPort(argv[1]);
    if (!gp) return TCL_ERROR;

    // FIXME: finish this.  Have to resolve aliases.
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// getClassName
// Return the class name of the specified block, or if none is
// specified, of the current galaxy. Note that the class name of a
// galaxy is the name of the master galaxy from which it was originally
// cloned, or if the galaxy is a master (on the KnownBlocks list), then
// its own name. The class name of a Ptolemy star has the domain name
// as a prefix, as in "SDFRamp".
//
int PTcl::getClassName(int argc,char** argv) {
    if (argc > 2) return usage("getClassName ?<blockname>?");
    const Block* b;
    if (argc == 1)
        b = getBlock("this");
    else
        b = getBlock(argv[1]);     
    if (!b) return TCL_ERROR;
    return staticResult(b->className());
}

/////////////////////////////////////////////////////////////////////////////
//// getDescriptor
// Return the descriptor of the specified block, or if none is specified,
// of the current galaxy.
//
int PTcl::getDescriptor(int argc,char** argv) {
    if (argc > 2) return usage("getDescriptor ?<blockname>?");
    const Block* b;
    if (argc == 1)
        b = getBlock("this");
    else
        b = getBlock(argv[1]);     
    if (!b) return TCL_ERROR;
    return staticResult(b->descriptor());
}

/////////////////////////////////////////////////////////////////////////////
//// getDomain
// Return the domain of the specified block, or if none is
// specified, of the current galaxy.
//
int PTcl::getDomain(int argc,char** argv) {
    if (argc > 2) return usage("getDomain ?<blockname>?");
    const Block* b;
    if (argc == 1)
        b = getBlock("this");
    else
        b = getBlock(argv[1]);     
    if (!b) return TCL_ERROR;
    return result(b->domain());
}

/////////////////////////////////////////////////////////////////////////////
//// getFullName
// Return the full name of the specified block, or if none is
// specified, of the current galaxy.  The full name begins with a period,
// followed by the universe name, followed by parent galaxy names separated
// by periods, followed by the final block name.
//
int PTcl::getFullName(int argc,char** argv) {
    if (argc > 2) return usage("getFullName ?<blockname>?");
    const Block* b;
    if (argc == 1)
        b = getBlock("this");
    else
        b = getBlock(argv[1]);
    if (!b) return TCL_ERROR;
    InfString res = fullName(b);
    return result(res);
}

/////////////////////////////////////////////////////////////////////////////
//// getParent
// Return the full name of the parent of the specified block, or if
// none is specified, of the current galaxy. If the specified block is
// an instance of a star or galaxy, then the full name begins with a
// period, followed by the universe name, followed by parent galaxy
// names separated by periods, followed by the final block name. If the
// specified block is a universe, then return ".". If the specified
// block is a master star or galaxy, then return the empty string. If
// the specified block does not exist, then return an error.
//
int PTcl::getParent(int argc,char** argv) {
    if (argc > 2) return usage("getParent ?<blockname>?");
    const Block* b;
    if (argc == 1)
        b = getBlock("this");
    else
        b = getBlock(argv[1]);
    if (!b) return TCL_ERROR;

    if (b->parent() == NULL) {
        // The block is either a master or a universe.
        if (b->isA("InterpUniverse")) {
            // must be a universe.
            return(result("."));
        } else {
            // must be a master.
            return(result(""));
        }
    } else {
        InfString name = fullName(b->parent());
        return(result(name));
    }
}

/////////////////////////////////////////////////////////////////////////////
//// matlab
// FIXME: Documentation.
// we make matlabtcl a static instance of the MatlabTcl class instead of
// a data member of the PTcl class because other libraries, e.g. pigilib,
// rely on the PTcl class but do not use the "matlab" ptcl command, so
// false dependencies would otherwise be automatically created by make depend
#include "MatlabTcl.h"

static MatlabTcl matlabtcl;

int PTcl::matlab(int argc,char** argv) {
    matlabtcl.SetTclInterpreter(interp);
    return matlabtcl.matlab(argc, argv);
}

/////////////////////////////////////////////////////////////////////////////
//// mathematica
// FIXME: Documentation.
// we make mathematicatcl a static instance of the MathematicaTcl class
// instead of a data member of the PTcl class because other libraries,
// e.g. pigilib, rely on the PTcl class but do not use the "mathematica"
// ptcl command, so false dependencies would otherwise be automatically
// created by make depend
#include "MathematicaTcl.h"

static MathematicaTcl mathematicatcl;

int PTcl::mathematica(int argc,char** argv) {
    mathematicatcl.SetTclInterpreter(interp);
    return mathematicatcl.mathematica(argc, argv);
}

///////////////////////////////////////////////////////////////////////////
//// ports
// Return a list of port names for the specified block.
// <p>
// Usage: ports ?-inputs? ?-outputs? ?-multiports? <blockname>
// <p>
// If options are specified, they can be any combination of:
// <ul>
// <li> -inputs
// <li> -outputs
// <li> -multiports
// </ul>
// The first two options indicate that inputs or outputs should be
// listed. Specifying both of these options lists only ports that are
// both an input and an output, something supported in some domains.
// Specifying neither results in all ports being listed.
// The -multiports option specifies that only multiports should be
// listed, rather than the individual ports within the multiport.
// In this case, after each multiport is a list of its contained ports.
// For example, a multiport named "input" with two contained ordinary
// ports will appear in the result as "input {input#1 input#2}".
//
int PTcl::ports(int argc,char** argv) {
    int inputs=0;
    int outputs=0;
    int multiports=0;
    char* blockname=NULL;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-inputs") == 0) inputs = 1;
        else if (strcmp(argv[i],"-outputs") == 0) outputs = 1;
        else if (strcmp(argv[i],"-multiports") == 0) multiports = 1;
        else if (blockname != NULL)
            return usage("ports ?-inputs? ?-outputs? ?-multiports? <blockname>");
        else blockname = argv[i];
    }
    const Block* b = getBlock(blockname);
    if (!b) return TCL_ERROR;
    if (multiports) {
        CBlockMPHIter next(*b);
        const MultiPortHole* mp;
        while ((mp = next++) != 0) {
            if (inputs && !mp->isItInput()) continue;
            if (outputs && !mp->isItOutput()) continue;
            addResult(mp->name());
            CMPHIter nextp(*mp);
            const PortHole *ph;
            // Start a new sublist.
            Tcl_AppendResult(interp, " {", (char*)NULL);
            while ((ph = nextp++) != 0) {
                addResult(ph->name());
            }
            Tcl_AppendResult(interp, "}", (char*)NULL);
        }
    } else {
        CBlockPortIter next(*b);
        const PortHole* pp;
        while ((pp = next++) != 0) {
            if (inputs && !pp->isItInput()) continue;
            if (outputs && !pp->isItOutput()) continue;
            addResult(pp->name());
        }
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// remove
// Delete the specified block.
//
int PTcl::remove(int argc,char** argv) {
    if (argc != 2) return usage("remove <name>");
    const Block* b = getBlock(argv[1]);
    if (!b) {
        InfString msg = "No such block: ";
        msg << argv[1];
        return TCL_ERROR;
    }
    Block* parent = b->parent();
    if (parent) {
        if (!parent->isA("InterpGalaxy")) {
            InfString msg = "Unexpected error: ";
            msg << fullName(parent) << " is a parent but not an InterpGalaxy.";
            result(msg);
            return TCL_ERROR;
        }
        if (!((InterpGalaxy*)parent)->delStar(b->name())) return TCL_ERROR;
    } else {
        // Make sure we don't end up without a current universe.
        // If we are deleting the current universe, set a flag.
        int delcurrent = (b == universe);
        int delmain = (strcmp(b->name(),"main") == 0);
        if (!univs.delUniv(b->name())) {
            // If the block has no parent, but is not a universe, then
            // it is a master.
            InfString msg = "Cannot remove a master block: ";
            msg << fullName(b);
            result(msg);
            return TCL_ERROR;
        }
        if (delcurrent) {
            if (delmain) {
                // Deleted "main", remake it.
                newUniv("main", curDomain);
            } else {
                // Make "main" the current universe.
                const InterpUniverse* u = univs.univWithName("main");
                if (!u) {
                    // This shouldn't happend, but if main has been deleted,
                    // remake it.
                    newUniv("main", curDomain);
                    result("main universe somehow got deleted!  Recreating");
                    return TCL_ERROR;
                } else {
                    currentGalaxy = (InterpGalaxy*) u;
                    curDomain = currentGalaxy->domain();
                    currentTarget = u->myTarget();
                }
            }
        }
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// univlist
// Return a list of the names of the known universes.
// Each name begins with a dot, and hence is absolute.
//
int PTcl::univlist(int argc,char **) {
    if (argc > 1) return usage("univlist");
    IUListIter next(univs);
    InterpUniverse* u;
    while ((u = next++) != 0) {
        InfString nm = fullName(u);
        addResult(nm);
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////                     auxiliary public methods                       ////

/////////////////////////////////////////////////////////////////////////////
//// findPTcl
// Given a Tcl interpreter, return the associated PTcl object.
// This is a static function.
//
PTcl* PTcl::findPTcl(Tcl_Interp* arg) {
    for (int i = 0; i < MAX_PTCL; i++)
    if (ptable[i]->interp == arg) return ptable[i];
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//// fullName
// Return the full name of the specified block, port, or state. Use
// this instead of block->fullName() because it prepends a leading
// period if the object is an instance. The leading period is not
// prepended for a block that is a master, nor an object that is an
// instance within a master galaxy.
//
// WARNING: The returned result needs to be assigned to an InfString
// before it can be used.  Thus, "result(fullName(b))" will fail.
// Use "InfString res = fullName(b); result(res)".
//
InfString PTcl::fullName(const NamedObj* obj) {
    const NamedObj* top = obj;
    // Get the top-level parent.
    while (top->parent() != NULL) {
        top = top->parent();
    }

    // If the obj is within a universe, prepend a period.
    if (top->isA("InterpUniverse")) {
        InfString name = ".";
        name << obj->fullName();
        return name;
    }
    // Otherwise, do not prepend a period.
    return obj->fullName();
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////                         protected methods                          ////

/////////////////////////////////////////////////////////////////////////////
//// addResult
// Append a string to the Tcl result as a list element.
// You can pass this a StringList.
//
void PTcl::addResult(const char* value) {
    // cast-away-const needed to interface with Tcl.
    Tcl_AppendElement(interp, (char*)value);
}

///////////////////////////////////////////////////////////////////////////
//// galTopBlockIter
// Iterate over the blocks inside a galaxy, adding them
// to the Tcl result as list elements.  If the block given
// as an argument is atomic, do nothing.  If the second
// argument is non-zero, then recursively descend into
// galaxies, adding their blocks to the list, rather than
// listing the galaxy.  Thus, all blocks returned will be stars.
// If the third argument is non-zero, return the full name of
// each block.
//
void PTcl::galTopBlockIter(const Block* b, int deep, int fullname) {
    if (b->isItAtomic()) return;
    CGalTopBlockIter nextb(b->asGalaxy());
    const Block* bb;
    while ((bb = nextb++) != 0) {
        if (!deep || bb->isItAtomic()) {
            if (fullname) {
                InfString res = fullName(bb);
                addResult(res);
            } else {
                addResult(bb->name());
            }
        } else {
            galTopBlockIter(bb,deep,fullname);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//// getBlock
// Given a name, find a block with that name and return a pointer to
// it. If the name is "this", return the current galaxy. If the name is
// "target" return the current target. Otherwise, the name can be
// simple or dotted, where a dotted name has the form
// _name1.name2...nameN_ or _.name1.name2...nameN_. If the name is
// simple, first search the current galaxy, then the known list (which
// contains all the master stars and galaxies), then the universe list,
// and finally the known targets. Return a null pointer if the block is
// not found.
//
// If the name is dotted and there is no leading dot, the name might be
// relative to the current galaxy or absolute (beginning with a
// universe name). In the case of ambiguity (i.e. _name1_ is both a
// universe and a block within the current galaxy), preference is given
// to the local block. To avoid this ambiguity, provide a name with a
// leading dot.
//
// If the final portion, _nameN_, is absent (the last character is a
// dot) or "this", return the galaxy with name _name1.name2...nameN-1_;
// if it is "target", and _name1.name2...nameN-1_ is a wormhole, return
// the target of the wormhole.
//
// NOTE: This does not use InterpGalaxy::blockWithDottedName because that
// method does not support full names that begin with the universe name.
//
// NOTE: The leading dot really should be required for all absolute dotted
// names, but for backward compatibility, it is not required.  However,
// this can lead to unexpected errors.  E.g. given _name1.name2_ where
// _name1_ is a universe and local block, but _name2_ is not contained
// within the local block _name1_, this method will return a null string
// rather than searching the universe _name1_.  To avoid this problem,
// provide the leading dot for abosolute references.
//
const Block* PTcl::getBlock(const char* name) {
    // Return immediately on standard cases.
    if (!name || *name == 0 || strcmp(name,"this") == 0) return currentGalaxy;
    if (strcmp(name,"target") == 0) return currentTarget;

    // Determine whether a leading dot is provided.
    int absolute = 0;
    if (*name == '.') {
        absolute = 1;
        name++;
    }

    // Find the first block name in a dotted name, and whether the name
    // is dotted.
    int dotted = 0;
    const char* p = strchr (name, '.');
    const char* firstname;
    if (p != NULL) {
        dotted = 1;
	char buf[256];
	int n = p - name;
	strncpy (buf, name, n);
	buf[n] = 0;
        firstname = buf;
    } else {
        firstname = name;
    }
    const Block* b = NULL;
    // If not absolute, search the current galaxy, the known list,
    // and the targets, in that order.
    if (!absolute) {
        b = currentGalaxy->blockWithName(firstname);
        if (!b) b = KnownBlock::find(firstname, curDomain);
        // Used to search universes also.  Now require absolute name.
        // if (!b) b = univs.univWithName(firstname);
        if (!b) b = KnownTarget::find(firstname);
    } else {
        // Look for a universe name.
        if (!b) b = univs.univWithName(firstname);
    }
    if (b && dotted) {
        // Descend to find subblocks.
        b = getSubBlock(p+1,b);
    }
    if (!b) {
        StringList msg;
        msg << "No such block: " << name;
        result(msg);
        return NULL;
    }
    return b;
}

/////////////////////////////////////////////////////////////////////////////
//// getParentBlock
// Given a dotted name for a port or state, return a pointer to the parent
// block within which the port or state would appear. The port or state
// need not exist. If the name begins with a period, then it is assumed
// to be absolute. Otherwise, it is assumed to be relative the current
// galaxy. If there is no period in the name, then return the current
// galaxy. If the desired block does not exist, register an error message
// with Tcl and return a null pointer.
//
const Block* PTcl::getParentBlock(const char* name) {
    // Return immediately on problem cases.
    if (!name || *name == 0) {
        result("getParentBlock method must be given a port or state name");
        return NULL;
    }

    // Find the last occurrence of a period in the name.
    const char* p = strrchr (name, '.');
    if (p == NULL) {
        // No dot in the name.  Current galaxy is the context.
        return currentGalaxy;
    }
    // Copy all but the last dot and name into a buffer.
    // The input buffer is always long enough.
    char buf[strlen(name)];
    int n = p - name;
    strncpy (buf, name, n);
    buf[n] = 0;
    return (getBlock(buf));
}

/////////////////////////////////////////////////////////////////////////////
//// getParentGalaxy
// Given a dotted name for a block, return a pointer to the parent
// (galaxy or universe) within which the block would appear. The block
// need not exist. If the name begins with a period, then it is assumed
// to be absolute. Otherwise, it is assumed to be relative the current
// galaxy. If the name is a universe, return a pointer to the universe.
// If the desired galaxy does not exist, register an error message and
// return a null pointer.
//
InterpGalaxy* PTcl::getParentGalaxy(const char* name) {
    const Block* b = getParentBlock(name);
    if (!b) return NULL;
    if (!b->isA("InterpGalaxy")) {
        StringList msg;
        msg << "Not a galaxy: " << name;
        result(msg);
        return NULL;
    }
    return (InterpGalaxy*)b;
}

/////////////////////////////////////////////////////////////////////////////
//// getPort
// Given a name, find a port with that name and return a pointer to it.
// The port may be a multiport or a simple port.
// The name has the form _name1.name2...nameN_ (relative) or
// _.name1.name2...nameN_ (absolute). If the name is simple (no dots at
// all), search the ports of the current galaxy. If the name is dotted
// relative, the name is relative to the current galaxy. If the port
// is not found, register an error message with Tcl and return a null
// pointer.
//
GenericPort* PTcl::getPort(const char* portname) {
    const Block* b = getParentBlock(portname);
    if (!b) return NULL;
    // FIXME: in Ptolemy kernel: Block::portWithName is not const (why??)
    // Should probably have a const version of it!  See PortHole::portWithName.
    // Thus, we need the following cast.
    Block* bnc = (Block*)b;
    GenericPort* p = bnc->genPortWithName(rootName(portname));
    if (!p) {
        InfString msg = "No such port: ";
        msg << portname;
        result(msg);
        return NULL;
    }
    return p;
}

/////////////////////////////////////////////////////////////////////////////
//// makeEntry
// Add this object to the table of PTcl objects.
//
void PTcl::makeEntry() {
    int i = 0;
    while (ptable[i] != 0 && i < MAX_PTCL) i++;
    if (i >= MAX_PTCL) return;
    ptable[i] = this;
}

/////////////////////////////////////////////////////////////////////////////
//// newUniv
// Create a new universe with the given name and domain and make it the
// current universe.  Whatever universe was previously the current universe
// is not affected, unless it has the same name, in which case it is
// replaced.
//
void PTcl::newUniv(const char* name, const char* dom) {
    univs.delUniv(name);
    universe = new InterpUniverse(name, dom);
    currentGalaxy = universe;
    curDomain = universe->domain();
    currentTarget = universe->myTarget();
    univs.put(*universe);
}

/////////////////////////////////////////////////////////////////////////////
//// removeEntry
// Remove this object from the table of PTcl objects.
//
void PTcl::removeEntry() {
    for (int i = 0; i < MAX_PTCL; i++) {
        if (ptable[i] == this) {
            ptable[i] = 0;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//// result
// Set the Tcl result from a const char* or StringList.
// We arrange for Tcl to copy the value in case a literal string or a
// StringList is passed (because the StringList might be deleted afterwards).
// Return TCL_OK.
//
int PTcl::result(const char *str) {
    // VOLATILE will tell Tcl to copy the value, so it is safe
    // if the StringList is deleted soon.
    Tcl_SetResult(interp, (char*)str, TCL_VOLATILE);
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// rootName
// Return the name after the final period in the given name.
// If the given name has no period, return the given name.
//
const char* PTcl::rootName(const char* name) {
    // Find the last occurrence of a period in the name.
    const char* p = strrchr (name, '.');
    if (p == NULL) {
        return name;
    }
    p++;
    return p;
}

/////////////////////////////////////////////////////////////////////////////
//// staticResult
// Set the Tcl result from a static string.
// Typical usage: return staticResult("foo")
// Don't give this one a StringList!
// Return TCL_OK.
//
int PTcl::staticResult(const char* value) {
    Tcl_SetResult(interp,(char*)value,TCL_STATIC);
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// usage
// Append to the Tcl result a "usage" error with standard format and
// return TCL_ERROR.
//
int PTcl::usage(const char* msg) {
    Tcl_AppendResult(interp,
            "incorrect usage: should be \"",msg,"\"",(char*)NULL);
    return TCL_ERROR;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////                         private methods                            ////

/////////////////////////////////////////////////////////////////////////////
//// getFarPorts
// Add to the Tcl result a list of ports connected to the given port.
// This method assumes aliases have been resolved on this port, so we
// are given a star port, which may have a connection to other star ports.
// If _deep_ is non-zero, list those other ports. Otherwise, list the name
// of the top-level port that is aliased to the *far* ports.  If the given
// port is not connected to anything, add nothing to the Tcl result.
// Return TCL_OK.
//
int PTcl::getFarPorts(PortHole* ph, int deep) {
    const GenericPort* fp = ph->far();
    if (fp) {
        if (!deep) {
            while (fp->aliasFrom()) fp = fp->aliasFrom();
        }
        InfString res = fullName(fp);
        addResult(res);
    }
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
//// getSubBlock
// Recursively descend inside a galaxy to find a block with dotted name.
// Return NULL if the block does not exist within the given galaxy.
//
const Block* PTcl::getSubBlock(const char* name, const Block* gal) {
    if (gal->isItAtomic()) {
        return NULL;
    }
    // Cast is safe now
    InterpGalaxy* galaxy = (InterpGalaxy*)gal;

    // Return immediately on standard cases.
    if (!name || *name == 0 || strcmp(name,"this") == 0) return galaxy;
    if (strcmp(name,"target") == 0) {
        if (!gal->isItWormhole()) return NULL;
        // Cast is safe now.
        Runnable* worm = (Runnable*)gal;
        return worm->myTarget();
    }

    // Find the first block name in a dotted name, and whether the name
    // is dotted.
    int dotted = 0;
    const char* p = strchr (name, '.');
    const char* firstname;
    if (p != NULL) {
        dotted = 1;
	char buf[256];
	int n = p - name;
	strncpy (buf, name, n);
	buf[n] = 0;
        firstname = buf;
    } else {
        firstname = name;
    }
    const Block* b = galaxy->blockWithName(firstname);
    if (b && dotted) {
        // Descend to find subblocks.
        b = getSubBlock(p+1,b);
    }
    return b;
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
        if (strcmp(nm, s->name()) == 0) return s;
    }
    return 0;
}

// initialize: initialize a block or state (or porthole?)
int PTcl::initialize(int argc, char** argv) {
    if (argc == 3) {
        // Initialize a state.  Cast away the const.
        Block* blk = (Block*)getBlock(argv[1]);
        if (!blk) return TCL_ERROR;
        State* st = (State*)findState(blk, argv[2]);
        if (!st) {
            Tcl_AppendResult(interp,"No such state: ", argv[1], ":",
            argv[2],(char*)NULL);
            return TCL_ERROR;
        }
        st->initialize();
    } else if (argc == 2) {
        // Initialize a state.  Cast away the const.
        Block* blk = (Block*)getBlock(argv[1]);
        if (!blk) return TCL_ERROR;
        blk->initialize();
    } else {
        return usage("initialize <block> ?<state>?\n"
        "or initialize this\n"
        "or initialize target");
    }
    return TCL_OK;
}

// statevalue: return a state value
// syntax: statevalue <block> <state> ?current|initial?
// default is current value
int PTcl::statevalue(int argc,char ** argv) {
    if (argc < 3 || argc > 4 ||
    (argc == 4 && strcmp(argv[3], "current") != 0
    && strcmp(argv[3], "initial") != 0))
    return usage ("statevalue <block> <state> ?current|initial?");
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

int PTcl::computeSchedule() {
    SimControl::clearHalt();
    universe->initTarget();
    unsigned int flags = SimControl::flagValues();
    if (flags & SimControl::abort) {
        Tcl_SetResult(interp, "Aborted", TCL_STATIC);
        return FALSE;
    }
    if (flags & (SimControl::error | SimControl::halt)) {
        Tcl_SetResult(interp, "Error in setting up the schedule",
        TCL_STATIC);
        return FALSE;
    }
    return TRUE;
}

int PTcl::schedule(int argc,char **) {
    if (argc > 1)
    return usage("schedule");
    SimControl::clearHalt();
    // Here we call Runnable::initTarget rather than Universe::initTarget,
    // because Universe::initTarget invokes the begin methods of the 
    // stars.  We just want to generate the schedule.
    if (! (universe->myTarget() && universe->myTarget()->scheduler()) )
    universe->Runnable::initTarget();
    if (SimControl::flagValues() & (SimControl::error|SimControl::halt)) {
        Tcl_SetResult(interp, "Error in setting up the schedule",
        TCL_STATIC);
        return TCL_ERROR;
    } else
    return result(universe->displaySchedule());
}

// return the stop time of the current run
int PTcl::stoptime(int argc, char ** /*argv*/) {
    if (argc > 1) return usage("stoptime");
    sprintf(interp->result, "%.10g", stopTime);
    return TCL_OK;
}

// return the current time from the top-level scheduler of the current
// universe.  If the target has a state named "schedulePeriod", the
// returned time is divided by this value, unless a second argument
// with the value "actual" appears.  Return 0 if there is no scheduler.

int PTcl::schedtime(int argc,char **argv) {
    int actualFlag = (argc == 2 && strcmp(argv[1], "actual") == 0);
    if (argc > 2 || argc == 2 && !actualFlag)
    return usage("schedtime ?actual?");
    Scheduler *sch = universe->scheduler();
    if (sch) {
        double time = sch->now();
        if (!actualFlag) {
            State* s = universe->myTarget()->stateWithName
            ("schedulePeriod");
            if (s) {
                StringList value = s->currentValue();
                time /= atof(value);
            }
        }
        sprintf (interp->result, "%.10g", time);
        return TCL_OK;
    }
    else {
        Tcl_AppendResult(interp,"0",(char*)NULL);
        return TCL_OK;
    }
}

int PTcl::run(int argc,char ** argv) {
    if (argc > 2)
    return usage("run ?<stoptime>?");
    
    // We need to set stopTime first, so that the
    // "stoptime" command can work in the setup,
    // begin, and go methods of the stars.
    stopTime = 1.0;  // default value of the stop time
    lastTime = 1.0;
    if (argc == 2) {
        double d;
        if (Tcl_GetDouble(interp, argv[1], &d) != TCL_OK)
        return TCL_ERROR;
        stopTime = d;
        lastTime = d;
    }
    
    if (!computeSchedule())
    return TCL_ERROR;
    // universe->setStopTime has to be called after computeSchedule
    // because computeSchedule resets the stop time.
    universe->setStopTime(stopTime);
    universe->run();
    
    return checkErrorAbort();
}

// function to check error/abort status
// common code for run, cont, wrapup
int PTcl::checkErrorAbort() {
    // If aborted, override any existing result
    // (the Tcl result may contain some random command's result,
    // so we don't want to return it)
    unsigned int flags = SimControl::flagValues();
    if (flags & SimControl::abort) {
        Tcl_SetResult(interp, "Aborted", TCL_STATIC);
        return TCL_ERROR;
    }
    // Else check the error flag bit.
    // If it is set, we assume the result has been set to an error message.
    return (flags & SimControl::error) ? TCL_ERROR : TCL_OK;
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
    
    // If an error or abort has occurred, do not proceed.
    if (SimControl::flagValues() & (SimControl::error|SimControl::abort)) {
        Tcl_SetResult(interp, "Cannot continue after error", TCL_STATIC);
        return TCL_ERROR;
    }
    
    // Clear the halt bit, because the run may have been terminated
    // prematurely by a halt request, and we still want to continue.
    SimControl::clearHalt();
    
    stopTime += lastTime;
    universe->setStopTime(stopTime);
    universe->run();
    
    return checkErrorAbort();
}

int PTcl::wrapup(int argc,char **) {
    if (argc > 1)
    return usage("wrapup");
    
    // If an error or abort has occurred, do not proceed.
    if (SimControl::flagValues() & (SimControl::error|SimControl::abort)) {
        Tcl_SetResult(interp, "Cannot wrapup after error", TCL_STATIC);
        return TCL_ERROR;
    }
    
    // Clear the halt bit, because the run may have been terminated
    // prematurely by a halt request, and we still want to wrap up.
    SimControl::clearHalt();
    
    universe->wrapup();
    
    return checkErrorAbort();
}

// domains: list domains
int PTcl::domains(int argc,char **) {
    if (argc > 1)
    return usage("domains");
    int n = Domain::number();
    for (int i = 0; i < n; i++) {
        const char* domain = Domain::nthName(i);
        if (KnownBlock::validDomain(domain))
        addResult(domain);
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
    int c = 1;
    if (argc == 2) t = argv[1];
    if (argc > 2 ||
    (argc == 2 && (c=strcmp(t,"on"))!=0 && strcmp(t,"off")!=0))
    return usage ("animation ?on|off?");
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
    if (argc == 2) {
        dom = argv[1];
        if (!KnownBlock::validDomain(dom)) {
            Tcl_AppendResult(interp, "No such domain: ", dom,
            (char *) NULL);
            return TCL_ERROR;
        }
    }
    KnownBlockIter nextB(dom);
    const Block* b;
    while ((b = nextB++) != 0)
    addResult(b->name());
    return TCL_OK;
}

int PTcl::isgalaxy (int argc,char ** argv) {
    if (argc != 2)
    return usage ("isgalaxy <block>");
    const Block* b = getBlock(argv[1]);
    if (!b) return TCL_ERROR;
    if (b->isItAtomic()) {
        result("0");
    } else {
        result("1");
    }
    // empty value returned for atomic blocks
    return TCL_OK;
}

int PTcl::iswormhole (int argc,char ** argv) {
    if (argc != 2)
    return usage ("iswormhole <block>");
    const Block* b = getBlock(argv[1]);
    if (!b) return TCL_ERROR;
    if (b->isItWormhole()) {
        result("1");
    } else {
        result("0");
    }
    return TCL_OK;
}

// return the list of states, ports, or multiports in the given block.
// The first argument, specifying states, ports, or multiports, must
// be given.  If the second argument is not given, block is current galaxy.
// FIXME: Obsolete.  Replaced by ports, states.
int PTcl::listobjs (int argc,char ** argv) {
    static char use[] =
    "listobjs states|ports|multiports ?<block-or-classname>?";
    if (argc > 3 || argc <= 1)
    return usage (use);
    const Block* b = getBlock(argv[2]);
    if (!b) return TCL_ERROR;
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

// FIXME: This should be renamed "clear", and "reset" should
// invoke InterpGalaxy->reset(), which re-executes the action list.
int PTcl::reset(int argc,char** argv) {
    if (argc > 2)
    return usage ("reset ?<name>?");
    const char* nm = "main";
    if (argc == 2) nm = hashstring(argv[1]);
    newUniv(nm, curDomain);
    return TCL_OK;
}

// renameuniv <newname>: rename current univ to newname.
// renameuniv <oldname> <newname>: rename <oldname> to <newname>.
// Side effect: if there is an existing universe by the new name,
// it will be destroyed.

int PTcl::renameuniv(int argc,char ** argv) {
    InterpUniverse* u = universe;
    const char* newname = argv[1];
    if (argc < 2 || argc > 3)
        return usage("renameuniv <newname> or renameuniv <oldname> <newname>");
    if (argc == 3) {
        u = univs.univWithName(argv[1]);
        if (!u) {
            Tcl_AppendResult(interp, "No such universe: ", argv[1],
            (char*) NULL);
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

// Return true if the target targetName is a legal target for
// the domain domName.

static int legalTarget(const char* domName, const char* targetName) {
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
        if (!legalTarget(curDomain, universe->myTarget()->name())) {
            universe->newTarget ();
            currentTarget = universe->myTarget();
        }
    }
    return TCL_OK;
}

// display or change the target
int PTcl::target(int argc,char ** argv) {
    if (argc == 1) {
        const char *targName = "No target";
        if (currentTarget) targName = currentTarget->name();
        return staticResult(targName);
    }
    else if (argc > 2) {
        return usage("target ?<targetname>?");
    }
    else {
        const char* tname = hashstring(argv[1]);
        if (!legalTarget(curDomain, tname)) {
            Tcl_AppendResult(interp, tname,
            " is not a legal target for domain ", curDomain,
            (char*) NULL);
            return TCL_ERROR;
        }
        int status;
        if (!definingGal) {
            status = universe->newTarget(tname);
            currentTarget = universe->myTarget();
        } else {
            LOG_DEL; delete currentTarget;
            currentTarget = KnownTarget::clone(tname);
            status = (currentTarget != 0);
        }
        return status ? TCL_OK : TCL_ERROR;
    }
}

int PTcl::targets(int argc,char** argv) {
    if (argc > 2) return usage("targets ?<domain>?");
    const char* domain;
    if (argc == 2) {
        domain = argv[1];
        if (!KnownBlock::validDomain(domain)) {
            Tcl_AppendResult(interp, "No such domain: ", domain,
            (char *) NULL);
            return TCL_ERROR;
        }
    } else
    domain = curDomain;
    const int MAX_NAMES = 40;
    const char *names[MAX_NAMES];
    int n = KnownTarget::getList (domain, names, MAX_NAMES);
    for (int i = 0; i < n; i++)
    addResult(names[i]);
    return TCL_OK;
}

// set (change) or return value of target parameter
int PTcl::targetparam(int argc,char ** argv) {
    if (argc != 2 && argc != 3)
    return usage("targetparam <name> ?<value>?");
    if (!currentTarget) {
        Tcl_AppendResult(interp, "Target has not been created yet.",
        (char*) NULL);
        return TCL_ERROR;
    }
    State* s = currentTarget->stateWithName(argv[1]);
    if (!s) {
        Tcl_AppendResult(interp, "No such target parameter: ", argv[1],
        (char*) NULL);
        return TCL_ERROR;
    }
    if (argc == 2) {
        return staticResult(s->initValue());
    }
    s->setInitValue(hashstring(argv[2]));
    return TCL_OK;
}

// Set or examine pragmas registered with the current target.
// This procedure can take between 1 and 4 arguments, all optional.
// They must be given in the order:
//	parentname
//	blockname
//	pragmaname
//	pragmavalue
// If two arguments are given, assume the second is a block name,
// and the first is the name of the galaxy or universe within which the block
// sits, and return the names and values of all pragmas that have been set for
// that block for the current target.  If three arguments
// are given (blockname, parentname, pragmaname), return
// the value of the particular pragma.  If four arguments are given,
// use the fourth argument to set the value of the pragma.
//
int PTcl::pragma(int argc,char ** argv) {
    if (!currentTarget) {
        Tcl_AppendResult(interp,"No current target!",(char*) NULL);
        return TCL_ERROR;
    }
    if (argc == 3) {
        return staticResult(currentTarget->pragma(argv[1],argv[2]));
    } else if (argc == 4) {
        return staticResult(currentTarget->pragma(argv[1],argv[2],argv[3]));
    } else if (argc == 5) {
        return staticResult(currentTarget->pragma(argv[1],argv[2],argv[3],argv[4]));
    } else {
        return usage("pragma ?<parentname>? ?<blockname>? ?<pragmaname>? ?<pragmavalue>?");
    }
}

// Return the names, types, and default values of the pragmas supported
// by targets with the given name.
// If the target name is "<parent>", issue an error message.
//
int PTcl::pragmaDefaults(int argc,char ** argv) {
    if (argc < 2 || strcmp(argv[1],"<parent>") == 0) {
        Tcl_AppendResult(interp,
        "You must specify a Target to access pragmas.\n",
        "It cannot be <parent>.",
        (char*) NULL);
        return TCL_ERROR;
    }
    const Target* t = KnownTarget::find(argv[1]);
    if (!t) {
        Tcl_AppendResult(interp,"Unrecognized target: ", argv[1], (char*) NULL);
        return TCL_ERROR;
    }
    if (argc == 2) {
        addResult(t->pragma());
        return TCL_OK;
    } else {
        return usage("pragmaDefaults <targetname>");
    }
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
    if (argc == 1) {
        char msg[128];
        // This method is dispatched to handle both multilink
        // and permlink.
        sprintf(msg, "%s <file1> ?<file2> ...?", argv[0]);
        return usage(msg);
    }
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

// Request a non-error halt of a running universe
int PTcl::halt(int /*argc*/, char ** /*argv*/) {
    SimControl::requestHalt();
    return TCL_OK;
}

// Request an abortive halt of a running universe
int PTcl::abort(int /*argc*/, char ** /*argv*/) {
    SimControl::declareAbortHalt();
    return TCL_OK;
}

// Added to support tycho - eal
// Monitor commands to ptcl.  The third, monitorPtcl can be redefined.
// The default method prints the command to the standard output using cout.

// static member -- tells whether monitoring is on or off
int PTcl::monitor = 0;

int PTcl::monitorOn(int /*argc*/, char ** /*argv*/) {
    monitor = 1;
    return TCL_OK;
}

int PTcl::monitorOff(int /*argc*/, char ** /*argv*/) {
    monitor = 0;
    return TCL_OK;
}

int PTcl::monitorPtcl(int argc,char **argv) {
    for (int i = 1; i < argc; i++) {
        cout << argv[i];
        cout << " ";
    }
    cout << "\n";
    return TCL_OK;
}


/////////////////////////////////////////////////////////////////////////////
//			Register actions with SimControl
/////////////////////////////////////////////////////////////////////////////

// First, define the action function that will be called by the Ptolemy kernel.
// The string tclAction passed will be executed as a Tcl command.
void ptkAction(Star* s, const char* tclCommand) {
    if (tclCommand == NULL || *tclCommand == '\0') {
        Tcl_Eval(PTcl::activeInterp,
        "error {null pre or post action requested}");
        return;
    }
    
    if (s == 0) {
        Tcl_Eval(PTcl::activeInterp,
                "error {no star passed to pre or post action}");
        return;
    }
    
    InfString temp = PTcl::fullName(s);
    Tcl_VarEval(PTcl::activeInterp, tclCommand, " \"",
            (char*)temp, "\"", (char*)NULL);
}

// Destructor cancels an action and frees memory allocated for a TclAction
TclAction::~TclAction() {
    // remove action and delete its memory
    SimControl::cancel(action);
    
    // delete dynamic strings
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
    TclAction* obj;
    ListIter next(*this);
    while ((obj = (TclAction*)next++) != 0) {
        if (strcmp(name, obj->name) == 0) return obj;
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
    if(argc != 3) return usage("registerAction pre|post <command>");
    
    // if "pre" is TRUE, then it is a pre-action
    int pre = FALSE;
    if (strcmp("pre", argv[1]) == 0) pre = TRUE;
    else if (strcmp("post", argv[1]) == 0) pre = FALSE;
    else return usage("registerAction pre|post <command>");
    
    // Define a new Tcl action
    TclAction* tclAction = new TclAction;
    tclAction->tclCommand = savestring(argv[2]);
    tclAction->action =
    SimControl::registerAction(ptkAction, pre, tclAction->tclCommand);
    
    // Create a unique name for the action
    char buf[32];
    sprintf(buf, "an%u", (unsigned int)tclAction);
    tclAction->name = savestring(buf);
    
    // Register the action with the action list
    tclActionList.put(*tclAction);
    
    // The following guarantees to Tcl that the name will not be deleted
    // at least until the next call to Tcl_Eval.  This is safe because only
    // the cancelAction command below can delete this name.
    // I'm not sure about this. -BLE
    Tcl_SetResult(interp, tclAction->name, TCL_VOLATILE);
    return TCL_OK;
}

// Cancel a registered action
// The single argument is the action_handle returned by registerAction
//
int PTcl::cancelAction(int argc, char** argv) {
    if (argc != 2) return usage("cancelAction <action_handle>");
    TclAction* tclAction = tclActionList.objWithName(argv[1]);
    if (tclAction == 0) {
        Tcl_AppendResult(interp,
        "cancelAction: Failed to convert action handle", (char*) NULL);
        return TCL_ERROR;
    }
    tclActionList.remove(tclAction);
    delete tclAction;
    return TCL_OK;
}

// FIXME: Remove this garbage.  Kept only temporarily for the
// DomainInfo object.
int PTcl::print(int argc,char** argv) {
	if (argc > 2) return usage("print ?<block-or-classname>?");
	const Block* b = getBlock(argv[1]);
	if (!b) return TCL_ERROR;
	return result(b->print(0));
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
// All commands are registerd in the pitcl namespace.

#define ENTRY(verb) { quote(::pitcl::verb), PTcl::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), PTcl::action }

// Here is the table.  Make sure it ends with "0,0"
// CAUTION: the dispatcher treats the first three entries specially
// for monitor support.
static InterpTableEntry funcTable[] = {
    ENTRY(monitorPtcl),
    ENTRY(monitorOn),
    ENTRY(monitorOff),
    ENTRY(abort),
    ENTRY(addBlock),
    ENTRY(addUniverse),
    ENTRY(aliasDown),
    ENTRY(aliasUp),
    ENTRY(animation),
    ENTRY(blocks),
    ENTRY(cancelAction),
    ENTRY(connect),
    ENTRY(connected),
    ENTRY(cont),
    ENTRY(curgalaxy),
    ENTRY(defgalaxy),
    ENTRY(delnode),
    ENTRY(disconnect),
    ENTRY(domain),
    ENTRY(domains),
    ENTRY(exit),
    ENTRY(galaxyPort),
    ENTRY(getAnnotation),
    ENTRY(getClassName),
    ENTRY(getDescriptor),
    ENTRY(getDomain),
    ENTRY(getFullName),
    ENTRY(getParent),
    ENTRY(halt),
    ENTRY(initialize),
    ENTRY(isgalaxy),
    ENTRY(iswormhole),
    ENTRY(knownlist),
    ENTRY(link),
    ENTRY(listobjs),
    ENTRY(matlab),
    ENTRY(mathematica),
    ENTRY(multilink),
    ENTRY(newstate),
    ENTRY(node),
    ENTRY(nodeconnect),
    ENTRY2(permlink,multilink),
    ENTRY(pragma),
    ENTRY(pragmaDefaults),
    ENTRY(ports),
    ENTRY(remove),
    ENTRY(renameuniv),
    ENTRY(registerAction),
    ENTRY(reset),
    ENTRY(run),
    ENTRY(stoptime),
    ENTRY(schedtime),
    ENTRY(schedule),
    ENTRY(seed),
    ENTRY(setstate),
    ENTRY(statevalue),
    ENTRY(target),
    ENTRY(targetparam),
    ENTRY(targets),
    ENTRY(univlist),
    ENTRY(wrapup),
    { 0, 0 }
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

// This method is registered with Itcl to implement all calls to
// the PTcl methods below.  The third argument, an integer, determines
// which call is made.
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
    
    // Added to support tycho - eal
    // If the flag monitor is set, call monitorPtcl to monitor commands.
    // Avoid the call if the command is "monitorPtcl", "monitorOn",
    // or "monitorOff".
    if (monitor && i>2) {
        // Call monitorPtcl with the arguments being the
        // Ptcl command that we are executing and its arguments
        InfString cmd = "monitorPtcl ";
        for(int j = 0; j < argc; j++) {
            cmd += argv[j];
            cmd += " ";
        }
        Tcl_Eval(interp, (char*)cmd);
    }
    
    int status = (obj->*(funcTable[i].func))(argc,argv);
    activeInterp = save;
    return status;
}


// IUList methods.

void IUList::put(InterpUniverse& u) {NamedObjList::put(u);}

// Delete the specified universe if it exists and return TRUE.
// Otherwise, return FALSE.
//
int IUList::delUniv(const char* name) {
    InterpUniverse* z = univWithName(name);
    if (z) {
        remove(z);
        LOG_DEL; delete z;
    }
    return (z != 0);
}
