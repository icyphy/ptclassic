#ifndef _CGStar_h
#define _CGStar_h 1
/******************************************************************
Version identification:
@(#)CGStar.h	1.41	09/20/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  E. A. Lee, J. Buck, J. Pino, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFStar.h"
#include "CGPortHole.h"
#include "CGTarget.h"
#include "CodeBlock.h"
#include "SymbolList.h"
#include "IntState.h"

// Although both of these attributes are defined here, it is up to
// the derived targets to implement the various combinations.
const bitWord AB_SHARED = 0x10; // global state on the uniprocessor level
const bitWord AB_GLOBAL = 0x20; // global state over all processors

// A state is shared over all the stars either:
// Globally - all stars over all the processors
// Locally -  all stars over the current processor 
extern const Attribute A_SHARED;
				
extern const Attribute A_GLOBAL; // state is visible over all the processors

// By default, states are both private and local.  That means, all the 
// stars instances of a particular class have there own copy of each state.
extern const Attribute A_PRIVATE;
extern const Attribute A_LOCAL;

class StringList;

// the following function is provided by the SunOS and Ultrix libs.
// don't know how generally it is available.
extern "C" int strcasecmp(const char* s1, const char* s2);

class CodeStream;
class Profile;

	////////////////////////////////////
	// class CGStar
	////////////////////////////////////

// CG stars may have dynamic ports, but they must be scheduled statically.

class CGStar : public DynDFStar {
friend class CGTarget;
friend class CGCPortHole;
public:
	// Constructor
	CGStar();

	// my domain
	const char* domain() const;

	// method for generating initial code
	virtual void initCode() {}

	// Generate code.  No data is grabbed or put onto geodesics.
	int run();

	// Pointer to target
	CGTarget* cgTarget() { return (CGTarget*)target(); }

	// class identification
	int isA(const char*) const;

	// query if it is a data-parallel star?
	virtual int isParallel() const { return dataParallel; }

	// For a data parallel star, or a macro actor get the profile.
	virtual Profile* getProfile(int ix = 0); 

        // max {cost of communication with its ancestors}
        int maxComm();

	// get and set the procId
	virtual int getProcId() { return int(procId); }
	virtual void setProcId(int i) { procId.setInitValue(i); procId = i; }

	// am I a Fork star?
	virtual int isItFork() { return forkId; }

	// return type name for ddf-type stars.
	virtual const char* readTypeName();

	// set the target pointer, initialize the various target pointers
	// such as codeStreams & symbols if needed.
    	/*virtual*/ int setTarget(Target* t);

	// return error on attempt to execute dynamically
	int setDynamicExecution(int);

protected:
	SymbolList codeblockSymbol;
	SymbolList starSymbol;

	// Process the string and add it to the Target code.  If stream
	// is NULL (default) code is added to the myCode stream.
	// Name may be given for shared stream.
	// Returns TRUE if code was actually added.
	int addCode(const char*code, const char* stream=NULL, 
		     const char* name = NULL);

	// Add a procedure to the procedure stream.
	// Returns TRUE if code was actually added.
	int addProcedure(const char*code,const char* name=NULL);

	// output a comment to a target stream, if stream name is
	// null, output the comment to myCode stream.  Note: this will
	// only use the default comment separators specified by comment
	// in Target class.
	virtual void outputComment(const char* msg, const char* stream=NULL);

	// Return the special character that introduces a macro
	// in a code block.  This character is used by gencode() to
	// parse the CodeBlock.  In this base class, the character is '$'.
	virtual char substChar() const;

	// For generation of error messages
	void codeblockError(const char* p1, const char* p2 = "");
	void macroError(const char* func, const StringList& argList);

	/* Expand macros that are defined for this star.
	   The following macros are recognized:

		$val(name)		Value of a state.
		$size(name)		Size of a state or port.
		$ref(name)		Reference to a state or port.
		$ref(name,offset)	Reference with offset.
		$label(name)		Unique label for codeblock.
		$codeblockSymbol(name)	Another name for $label.
		$starSymbol(name)	Unique label for star.
		$sharedSymbol(list,name)Unique label for set list,name pair

	   The number, names, and meaning of
	   these functions can be easily redefined in derived classes.
	*/
	virtual StringList expandMacro(const char* func,
				       const StringList& argList);

	// Match macro name and argument count.
	int matchMacro(const char* func, const StringList& argList,
		       const char* name, int argc)
	{
	    return ((strcasecmp(func, name) == 0) &&
		    (argList.numPieces() == argc));
	}

	// Evaluate State used as MultiPortHole index.
	StringList expandPortName(const char*);

	// State or PortHole reference.
	virtual StringList expandRef(const char*);

	// State or PortHole reference with offset.
	virtual StringList expandRef(const char* name, const char* offset);

	// State value.
	virtual StringList expandVal(const char*);

	// Size of State or PortHole.
	StringList expandSize(const char*);

	// Lookup a CodeStream in the target.
	CodeStream* getStream(const char* name=NULL)
	{
	    return cgTarget()->getStream(name);
	}

	// create a new CodeStream with a given name.  It is legal to call this
	// function multiple times with the same name.  In this case, only the
	// first call will actually create a new CodeStream.  Error::abortRun
	// is called if this fails.
	CodeStream* newStream(const char* name);
	
	// Update all PortHoles so that the offset is incremented by the
	// number of samples consumed or produced.
	void advance();

	// processor id 
	IntState procId;

	// indicate whether this star is a (data)parallel star or not.
	int dataParallel;

	// profile (= local schedule) of the data parallel star or macro actor
	Profile* profile;

	// declare that I am a Fork star
	void isaFork() { forkId = TRUE; }

	// do initialization for fork stars.
	virtual void forkInit(CGPortHole& input,MultiCGPort& output);
	virtual void forkInit(CGPortHole& input,CGPortHole& output);

	// Lookup a shared symbol by scope name & symbol name.
	// Return NULL on error.
	const char* lookupSharedSymbol(const char* scope, const char* name);

	// modify "deferrable": always defer stars that feed into forks,
	// to keep minimum possible size for fork buffers.
	int deferrable();

private:
	// Reset local codeblock labels
	void resetCodeblockSyms(){ codeblockSymbol.initialize(); }

	// Process code, expanding macros.
	int processCode(StringList&, CodeBlock&);
	int processCode(StringList&, const char*);

 	// parse macro invocation until after the end of the argument list
 	int processMacro(StringList&, const char*& text);

	// indicate if a fork star
	int forkId;
};

class CGStarPortIter : public BlockPortIter {
public:
	CGStarPortIter(CGStar& s) : BlockPortIter(s) {}
	CGPortHole* next() { return (CGPortHole*)BlockPortIter::next();}
	CGPortHole* operator++(POSTFIX_OP) { return next();}
};

#endif
