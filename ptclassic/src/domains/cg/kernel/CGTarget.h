#ifndef _CGTarget_h
#define  _CGTarget_h 1

/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 Baseclass for all single-processor code generation targets.

*******************************************************************/

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
#include "DataFlowStar.h"
#include "ImplementationCost.h"

class CGStar;
class SDFSchedule;
class SDFScheduler;
class CGCStar;

#include "ConversionTable.h"
//class TypeConversionTable {
//public:
//    const char *src, *dst, *star;
//};

class CommPair {
public:
    CommPair(CGCStar* c, CGStar* o):cgcStar(c),cgStar(o) {};
    CommPair():cgcStar(0),cgStar(0) {};
    CGCStar* cgcStar;
    CGStar* cgStar;
};

class AsynchCommPair {
public:
    AsynchCommPair(CGStar* c, CGStar* o):peek(c),poke(o) {};
    AsynchCommPair():peek(0),poke(0) {};
    void set(CGStar* c, CGStar* o) {peek = c; poke = o; };
    CGStar *peek, *poke;
};

extern const char *CODE, *PROCEDURE;

// Defined in CGDomain.cc
extern const char CGdomainName[];

class CGTarget : public Target {
public:
    // Constructor
    CGTarget(const char* name, const char* starclass, const char* desc,
             const char* assocDomain = CGdomainName, char sep = '_');

    // Destructor
    ~CGTarget();

    // Generate a new CGTarget.
    /*virtual*/ Block* makeNew() const;

    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // If within a WormHole, transfer data to the already running code.
    // Otherwise, generate code.
    /*virtual*/ int run();

    // Do nothing if in a WormHole; else, display, compile, load, run the code
    /*virtual*/ void wrapup();

    // Generate code.
    virtual void generateCode();

    // Generate code streams.
    virtual void generateCodeStreams();

    // Save the generated code to a file.
    virtual void writeCode();

    // Compile the generated code.  Return FALSE on error.
    virtual int compileCode();

    // Load the generated code.  Return FALSE on error.
    virtual int loadCode();

    // Run the generated code.  Return FALSE on error.
    virtual int runCode();

    // Generate code for a Star firing.  By default, execute the star.
    void writeFiring(Star&,int depth);

    // Incrementally add a star.  If flag is FALSE, call the star's go method.
    virtual int incrementalAdd(CGStar* s, int flag = 1);

    // Add code from a galaxy.  The scheduler s is the galaxy's scheduler.
    virtual int insertGalaxyCode(Galaxy* g, SDFScheduler* s);

    // Support methods for loops.
    /*virtual*/ void beginIteration(int,int);
    /*virtual*/ void endIteration(int,int);

    // Generate loop initialization code for stars.
    /*virtual*/ void genLoopInit(Star& s, int reps);
    /*virtual*/ void genLoopEnd(Star& s);

    // methods for generating code for reading and writing
    // wormhole ports.  Argument is the "real port" of the interior
    // star that is attached to an event horizon.  If no argument is
    // given, generate code for all the appropriate portholes.
    virtual void wormInputCode(PortHole&);
    virtual void allWormInputCode();
    virtual void wormOutputCode(PortHole&);
    virtual void allWormOutputCode();

    // method for preparing a wormhole for code generation
    virtual void wormPrepare();

    // methods for sending and receiving data to a target when
    // run inside of a wormhole. Argument is the "real port" of the 
    // interior star that is attached to an event horizon.  If no argument 
    // is given, send & receive for all the appropriate portholes.
    virtual int allSendWormData();
    virtual int allReceiveWormData();
    virtual int sendWormData(PortHole&);
    virtual int receiveWormData(PortHole&);

    // Provided for convenience and backward compatibility.
    static int haltRequested() {return SimControl::haltRequested();}

    // Return FALSE if the Target is not inside a WormHole.
    int inWormHole();

    // Generate a comment from a specified string.
    // Multi-line comments are supported if a continuation string is specified.
    // Defaults to shell-style comments.
    virtual StringList comment(const char* cmt, const char* begin=NULL, 
            const char* end=NULL ,const char* cont=NULL);

    // Returns a comment detailing the user name, time, date, and target type.
    virtual StringList headerComment(const char* begin=NULL,
	const char* end="", const char* cont=NULL);

    // system call in destination directory.  If error is specified
    // and the system call is unsuccessful display the error message.
    // This will return the error code from system. (0 == successful)
    virtual int systemCall(const char* cmd, const char* error=NULL,
	const char* host="localhost");

    // Write a file in the 'destDirectory' on the 'targetHost'.  If the
    // directory does not exist, it is created.  The code is optionally
    // displayed, if you want to optionally let the user display the
    // code written set this argument to 'displayFlag'.  The file name
    // is set to 'filePrefix'suffix, were 'filePrefix' is a target
    // state.  If unsuccessful, the method calls Error::abortRun and 
    // returns FALSE.
    int writeFile(const char* text, const char* suffix="", int display=FALSE,
		  int mode = -1);

    // Copy an SDF schedule from the multiprocessor schedule, instead
    // of performing an SDF scheduler for a uni-processor target
    void copySchedule(SDFSchedule&);
    
    // Return a pointer to the named CodeStream.
    // If name=NULL, return a pointer to defaultStream.
    // Call Error::abortRun and return NULL on error.
    CodeStream* getStream(const char* name=NULL);

    // Add a CodeStream to the target.  This allows stars to access this
    // stream by name.  This method should be called in the the target's
    // constructor.  If a target tries to add a stream where another stream
    // with the same name already exists, Error::abortRun is called.
    void addStream(const char* name,CodeStream* slist);

    // Create a new CodeStream for the target.
    inline CodeStream* newStream(const char* name) {
	return codeStringLists.newStream(name);
    }

    // Change the default stream into which code is generated into.
    // This method first returns a reference to the current defaultStream and
    // then changes it to the one specified.  It is up to the caller
    // of this method to restore the defaultStream if appropriate.
    inline CodeStream& switchDefaultStream(CodeStream& newDefault) {
	CodeStream* oldDefault = defaultStream;
	defaultStream = &newDefault;
	return *oldDefault;
    }
	 
    // Return a pointer to the counter to make symbols unique.
    int* symbolCounter();

    // separator for symbols, <name><separator><unique number>
    char separator;

    const char* lookupSharedSymbol(const char* scope, const char* name);

    // Return the relative execution time estimate of a star
    virtual int execTime(DataFlowStar* s, CGTarget* = 0)
	{ return s->myExecTime(); }

    // If within a WormHole, generate, compile, load, and run code.
    /*virtual*/ void setup();

    // initilization routine as a child target of a multiprocessor target
    // initialize State and choose scheduler
    virtual void childInit();

    // Combine all sections of code.
    virtual void frameCode();

    // Compute buffer sizes and allocate memory.
    // Return FALSE on error.
    virtual int allocateMemory();

    // Make a unique symbol
    StringList symbol(const char *name) {
	StringList sym;
	sym << targetNestedSymbol.symbol(name);
	return sym;
    }

    // Stream to collect makefile commands
    CodeStream makefile;

    // Codestream to collect all the files to include in for a make
    // all.  Make sure you add each file name with a space character
    // prepended.  For example: makeAllList << " foo.o";
    CodeStream  makeAllList;

    // The following virtual functions do the stages of CGTarget::setup.
    // generally, derived classes should overload these functions rather
    // than overloading setup.

    // perform necessary modifications to the galaxy (e.g. splice in
    // additional stars).  This happens before initialization.
    // returns FALSE on error.  This class has a do-nothing version.
    // if defined, it is responsible for calling galaxy.initialize()
    // if needed to do the start functions.
    virtual int modifyGalaxy();

    // Return implementation cost information
    virtual const ImplementationCost* implementationCost() { return 0; }

    // Compute implementation cost information
    virtual int computeImplementationCost() { return FALSE; }

    // Displaying implementation cost information
    virtual const char* describeImplementationCost() { return "not computed"; }

    // Resetting implementation cost information
    virtual void resetImplementationCost() {}

    // Indicate whether target can compute memory cost information
    virtual int canComputeMemoryUsage() { return FALSE; }

    // Indicate whether target can compute execution time
    virtual int canComputeExecutionTime() { return FALSE; }

    /********************************************************************/
    /*  Below, are the states that may need modification if this target */
    /*  is being used as a child target.                                */
    /********************************************************************/
    
    // Directory to store code files in.
    StringState destDirectory;

    // If we set this state 0, no looping. 1, Joe's looping.
    // If set to 2, Shuvra and Ha's extensive looping.
    // If set to 3, Praveen and Shuvra's looping for buffer minimization.
    // Also, now DEF = 0, CLUST = 1, SJS = 2, ACYLOOP = 3 where "=" means
    // corresponds.
    StringState loopingLevel;


    // Routines to construct CG wormholes, using the
    // $PTOLEMY/src/domains/cgc/targets/main/CGWormTarget
    virtual CommPair fromCGC(PortHole&);
    virtual CommPair toCGC(PortHole&);
    
    // create a peek/poke communication pair.  This is described in detail in
    // J.L. Pino, T.M. Parks and E.A. Lee, "Mapping Multiple Independent
    // Synchronous Dataflow Graphs onto Heterogeneous Multiprocessors," 
    // Proc. of IEEE Asilomar Conf. on Signals, Systems, and Computers, 
    // Pacific Grove, CA, Oct. 31 - Nov. 2, 1994.
    virtual AsynchCommPair createPeekPoke(CGTarget& peekTarget,
					  CGTarget& pokeTarget);

protected:

    // choose the correct scheduler
    virtual void chooseScheduler();

    // Initialization for code generation.
    // The default version does nothing.
    virtual int codeGenInit();

    // Methods used for stages of code generation.
    virtual void headerCode();
    virtual void mainLoopCode();
    virtual void trailerCode();

    // Optionally writes the schedule to a file
    /*virtual*/ int schedulerSetup();

    // Symbols which are nested, such as for loops
    SymbolStack targetNestedSymbol;

    // Symbols which are shared Target-wide.
    ScopedSymbolList sharedSymbol;

    // The default stream.  Used by the getStream method.
    CodeStream* defaultStream;

    // myCode contains the code generated for the target
    CodeStream myCode;
    CodeStream procedures;

    // Host machine on which to compile or assemble code.
    StringState targetHost;

    // Prefix for file names.  This is automatically set in the setup
    // method to the galaxy name if = "".
    StringState filePrefix;

    // Enable or disable Target functions.
    IntState displayFlag;
    IntState compileFlag;
    IntState loadFlag;
    IntState runFlag;
    IntState writeScheduleFlag;
    IntState reportMemoryUsage;
    IntState reportExecutionTime;

    // Name of schedule file
    char* schedFileName;

    // scheduling is not needed since the schedule is 
    // copied by copySchedule method. (Multiprocessor case)
    int noSchedule;

    // The following utilities are used by many types of code generators
    // Return a list of spaces for indenting
    StringList indent(int depth);

    // The following virtual functions do the stages of CGTarget::setup.
    // generally, derived classes should overload these functions rather
    // than overloading setup.

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

    ConversionTable* typeConversionTable;
    virtual int needsTypeConversionStar(PortHole& port);
 
    // Copy of default value of the destination directory
    StringList destDirName;

    // Returns full path name of log file and create dir if it does not exist
    StringList logFilePathName(const char* dir, const char* logFileName);

private:
    // Counter used to make symbols unique.
    int counter;

    // return non-zero if this target is not a child target
    // Then, generate code in the setup stage if it is inside a wormhole.
    int alone() { return (parent() == NULL) ; }

    // list of spliced stars
    SequentialList spliceList;
};

// A utility function to set a state of a AsynchCommPair to a certain value
int setAsynchCommState(AsynchCommPair pair, const char* stateName,
		     const char* value);

// Create the name of the destination directory
StringList destDirectoryName(const char* subdir);
#endif
