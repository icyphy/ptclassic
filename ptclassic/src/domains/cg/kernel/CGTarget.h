/******************************************************************
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

 Programmer: J. Buck, J. Pino, T. M. Parks

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
#include "SymbolList.h"
#include "SimControl.h"
#include "CodeStreamList.h"
#include "StringList.h"
#include "CGStar.h"

class SDFSchedule;
class SDFScheduler;

extern const char *CODE, *PROCEDURE;

class CGTarget : public Target {
public:
    CGTarget(const char* name, const char* starclass, const char* desc,
         char sep = '_');

    ~CGTarget();

    // Generate a new CGTarget.
    /*virtual*/ Block* makeNew() const;

    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Perform initialization such as setting up the schedule.
    // If within a WormHole, generate, compile, load, and run code.
    /*virtual*/ void setup();

    // If within a WormHole, transfer data to the already running code.
    // Otherwise, generate code.
    /*virtual*/ int run();

    // If within a WormHole, do nothing.
    // Otherwise, conditionally display, compile, load, and run the code.
    /*virtual*/ void wrapup();

    // Generate code.
    virtual void generateCode();

    // Generate code for a Star firing.
    // The default version simply fires the star.
    void writeFiring(Star&,int depth);

    // incrementally add a star
    // If flag is FALSE, just call go() method of the star.
    virtual int incrementalAdd(CGStar* s, int flag = 1);

    // Add code from a galaxy.
    // The schedule of the galaxy should be provided.
    virtual int insertGalaxyCode(Galaxy* g, SDFScheduler*);

    // Support methods for loops.
    /* virtual */ void beginIteration(int,int);
    /* virtual */ void endIteration(int,int);

    // Generate loop initialization code for stars.
    /* virtual */ void genLoopInit(Star& s, int reps);
    /* virtual */ void genLoopEnd(Star& s);

    // methods for generating code for reading and writing
    // wormhole ports.  Argument is the "real port" of the interior
    // star that is attached to an event horizon.  If no argument is
    // given, generate code for all the appropriate portholes.
    virtual void wormInputCode(PortHole&);
    virtual void allWormInputCode();
    virtual void wormOutputCode(PortHole&);
    virtual void allWormOutputCode();

    // methods for sending and receiving data to a target when
    // run inside of a wormhole. Argument is the "real port" of the 
    // interior star that is attached to an event horizon.  If no argument 
    // is given, send & receive for all the appropriate portholes.
    virtual int allSendWormData();
    virtual int allReceiveWormData();
    virtual int sendWormData(PortHole&);
    virtual int receiveWormData(PortHole&);

    // Save the generated code to a file.
    virtual void writeCode(const char* fileName = NULL);

    // Provided for convenience and backward compatibility.
    static int haltRequested() {return SimControl::haltRequested();}

    // Return FALSE if the Target is not inside a WormHole.
    int inWormHole();

    // Generate a comment from a specified string.
    // Multi-line comments are supported if a continuation string is specified.
    // Defaults to shell-style comments.
    virtual StringList comment(const char* cmt, const char* begin=NULL, 
            const char* end="",const char* cont=NULL);

    // Return a StringList detailing the user name, time, date, and
    // target type.
    // Uses the comment() method.
    virtual StringList headerComment(const char* begin=NULL,
	const char* end="", const char* cont=NULL);

    // system call in destination directory.  If error is specified
    // & the system call is unsuccessful display the error message.
    virtual int systemCall(const char*cmd,const char* error=NULL,
	const char* host="localhost");

    // Copy an SDF schedule from the multiprocessor schedule, instead
    // of performing an SDF scheduler for a uni-processor target
    void copySchedule(SDFSchedule&);
    
    // return the pointer of a code StringList given its name.  If it is 
    // not found, This method allows stars to access a code StringList by 
    // name.  If stream is not found, return NULL.
    CodeStream* getStream(const char* name);

    // separator for symbols, <name><separator><unique number>
    char separator;

    // Counter used to make symbols unique.
    int symbolCounter;

    const char* lookupSharedSymbol(const char* scope, const char* name);

    // set inheritFlag;
    void amInherited() { inheritFlag = TRUE; }

    // virtual method: do I support the given star type?
    virtual int support(Star* s); 

    // virtual method to return the relative execution of a star
    // by default, it just returns the myExecTime() of the star.
    virtual int execTime(DataFlowStar* s, CGTarget* t = 0)
	{ return s->myExecTime(); }

protected:

    // Methods used in setup(), run(), and wrapup().
    // The default versions do nothing.
    // Return FALSE on error.
    virtual int compileCode();
    virtual int loadCode();
    virtual int runCode();

    // Initialization for code generation.
    // The default version does nothing.
    virtual int codeGenInit();

    // Methods used for stages of code generation.
    virtual void headerCode();
    virtual void mainLoopCode();
    virtual void trailerCode();

    // Combine all sections of code.
    virtual void frameCode();

    // Compute buffer sizes and allocate memory.
    // Return FALSE on error.
    virtual int allocateMemory();

    // Add a CodeStream to the target.  This allows stars to access this
    // stream by name.  This method should be called in the the target's
    // constructor.  If a target tries to add a stream where another stream
    // with the same name already exists, Error::abortRun is called.
    void addStream(const char* name,CodeStream* slist);
 
    // method for supervising all code generation for the case of
    // a wormhole.  The default implementation generates an infinite
    // loop that reads input wormholes, runs the schedule, and writes
    // output wormholes, forever.
 /*   virtual int wormCodeGenerate() {generateCode(); return haltRequested; }
   */
    SymbolStack targetNestedSymbol;

    // Symbols which are shared Target-wide.
    ScopedSymbolList sharedSymbol;

    // myCode contains the code generated for the target
    CodeStream myCode;
    CodeStream procedures;

    // change the myCode pointer of the CGStars in the argument galaxy
    // to the appropriate code stream.
    void switchCodeStream(Block* b, CodeStream* s);

    StringState destDirectory;
    // If we set this state 0, no looping. 1, Joe's looping.
    // If set to 2, Shuvra and Ha's extensive looping.
    IntState loopingLevel;

    // Enable or disable Target functions.
    IntState displayFlag;
    IntState compileFlag;
    IntState loadFlag;
    IntState runFlag;

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
    virtual int modifyGalaxy();

    // call the scheduler compileRun(). Make this method virtual to
    // allow derived target to manage the target code streams.
    virtual void compileRun(SDFScheduler*);


     // If a CG domain is inside a wormhole, we may need to change
     // the sample rate of event horizons after scheduling is performed.
     void adjustSampleRates();
    
    // codeStringLists is a list of all the code streams that a star 
    // has access to.  These StringLists should be added from the 
    // derived target's constructor using the protected method add.
    // The StringLists should be accessed from the star using the 
    // public member getStream
    CodeStreamList codeStringLists;
    
    // splice in a new star, returning a pointer to the new star.
    Block* spliceStar(PortHole*, const char* name, int delayBefore,
                             const char* domainName);

private:
    // return non-zero if this target is not a child target, or not
    // inherited from another target. Then, generate code in the setup
    // stage if it is inside a wormhole.
    int alone() { return (parent() == NULL) && (inheritFlag == FALSE); }

    int inheritFlag;

    // list of spliced stars
    SequentialList spliceList;
};

#endif
