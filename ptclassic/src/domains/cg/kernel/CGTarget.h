/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifndef _CGTarget_h
#define  _CGTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "IntState.h"
#include "StringState.h"
#include "CGSymbol.h"
#include "SimControl.h"
#include "CodeStreamList.h"

class CGStar;
class SDFSchedule;

class CGTarget : public Target {
public:
    CGTarget(const char* name, const char* starclass, const char* desc,
         char sep = '_');

    // separator for symbols, <name><separator><unique number> == unique symbol
    char separator;

    ~CGTarget();

    static int haltRequested() {return SimControl::haltRequested();}

    // function that initializes the Target itself, before a Galaxy
    // is attached to it.  This is called first (before setup).
    void start();

    // function that computes the schedule, allocates memory (if needed),
    // and prepares for code generation (generating header and initial
    // code)
    int setup(Galaxy&);

    // function that executes the schedule to generate the main code.
    int run();

    // finalization.  In derived classes, wrapup might download and
    // execute the generated code; the baseclass simply displays the
    // code.
    void wrapup();

    // generate a new, identical CGTarget.
    Block* clone() const;

    // fn for adding code to the target
    void addCode(const char*);

    // methods to compile and run the target.
    // check access privilege later.
    virtual int compileCode();
    virtual int loadCode();
    virtual int runCode();

    // do we need this?
    virtual void writeCode(ostream&);

    // generate code for a processor in a multiprocessor system
    virtual StringList generateCode(Galaxy&);

    // type identification
    int isA(const char*) const;

    // output a comment.  Default form uses C-style comments.
    virtual void outputComment (const char*);

    // generate code for a firing.  The definition here simply
    // fires the star
    void writeFiring(Star&,int depth);

    // dummy beginIteration and endIteration
    void beginIteration(int,int);
    void endIteration(int,int);

    // system call in destination directory.  If error is specified
    // & the system call is unsuccessful display the error message.
    virtual int systemCall(const char*,const char* error=NULL);

    // Copy an SDF schedule from the multiprocessor schedule, instead
    // of performing an SDF scheduler for a uni-processor target
    void copySchedule(SDFSchedule&);
    
	// return the pointer of a code StringList given its name.  If it is 
	// not found, This method allows stars to access a code StringList by 
	// name.  If stream is not found, return NULL.
	StringList* getStream(const char* name);
	
protected:
    // Add a code StringList to the target.  This allows stars to access this
    // stream by name.  This method should be called in the the target's
    // constructor.  If a target tries to add a stream where another stream
    // with the same name already exists, Error::abortRun is called.
    void addStream(const char* name,StringList* slist);
    
    NestedSymbolList targetNestedSymbol;

    // myCode contains the code generated for the target
    StringList myCode;

    StringState destDirectory;
    IntState loopScheduler;

    char *schedFileName;

    // scheduling is not needed since the schedule is 
    // copied by copySchedule method. (Multiprocessor case)
    int noSchedule;

    // The following utilities are used by many types of code generators
    // Return a list of spaces for indenting
    StringList indent(int depth);

    // The following virtual functions do the stages of CGTarget::setup.
    // generally, derived classes should overload these functions rather
    // than overloading setup.

    // perform necessary modifications to the galaxy (e.g. splice in
    // additional stars).  This happens before initialization.
    // returns FALSE on error.  This class has a do-nothing version.
    // if defined, it is responsible for calling galaxy.initialize()
    // if needed to do the start functions.
    virtual int modifyGalaxy(Galaxy&);

    // compute buffer sizes, allocate memory, etc.  return TRUE for
    // success, false for failure.  Called after schedule generation
    // and modifyGalaxy.
    virtual int allocateMemory(Galaxy&);

    // do initialization for code generation: for example, compute
    // offsets of portholes and generate initCode. But in this base
    // class, do nothing
    virtual int codeGenInit(Galaxy&);

    // writes initial code
    virtual void headerCode();

    // method for supervising all code generation for the case of
    // a wormhole.  The default implementation generates an infinite
    // loop that reads input wormholes, runs the schedule, and writes
    // output wormholes, forever.
    virtual int wormCodeGenerate(Galaxy&);

    // methods for generating code for reading and writing
    // wormhole ports.  Argument is the "real port" of the interior
    // star that is attached to an event horizon.
    virtual void wormInputCode(PortHole&);
    virtual void wormOutputCode(PortHole&);

    // The following method downloads code for the inside of a wormhole
    // and starts it executing.
    virtual int wormLoadCode();

    // methods for sending and receiving data to a target when
    // run inside of a wormhole.  Arguments are same as above.
    virtual int sendWormData(PortHole&);
    virtual int receiveWormData(PortHole&);

    // Besides the code the scheduler generated, we usually need to
    // add more codes (such as global structure definitions). Then,
    // this virtual method provides the hook for that.
    // In this base class, do nothing.
    virtual void frameCode();
    
private:
    // codeStringLists is a list of all the code streams that a star 
    // has access to.  These StringLists should be added from the 
    // derived target's constructor using the protected method add.
    // The StringLists should be accessed from the star using the 
    // public member getStream
    StringListList codeStringLists;
    

};

#endif
