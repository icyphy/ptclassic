#ifndef _CGStar_h
#define _CGStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, J. Buck, J. Pino, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "CGConnect.h"
#include "Code.h"
#include "SymbolList.h"
#include "IntState.h"

class StringList;

// the following function is provided by the SunOS and Ultrix libs.
// don't know how generally it is available.
extern "C" int strcasecmp(const char* s1, const char* s2);

	////////////////////////////////////
	// class CGCodeBlock
	////////////////////////////////////

// In this generic architecture, a code block is identical to the base class
#define CGCodeBlock CodeBlock

class CGTarget;
class CGWormhole;

	////////////////////////////////////
	// class CGStar
	////////////////////////////////////

class CGStar : public SDFStar  {

public:
	// Constructor
	CGStar();

	// my domain
	const char* domain() const;

	// method for generating initial code
	virtual void initCode() {}

	// Generate code.  No data is grabbed or put onto geodesics.
	int fire();

	 // Pointer to target
	 CGTarget* myTarget() { return (CGTarget*)targetPtr; }

	// class identification
	int isA(const char*) const;

        // virtual method to return this pointer if it is a wormhole.
        // Return NULL if not.
        virtual CGWormhole* myWormhole();

        // max {cost of communication with its ancestors}
        int maxComm();

	// get and set the procId
	int getProcId() { return int(procId); }
	void setProcId(int i) { procId = i; }

	// am I a Fork star?
	int isItFork() { return forkId; }

	// set the target pointer, initialize the various target pointers
	// such as codeStreams & symbols if needed.
	void setTarget(Target* t);
	
protected:
	// Process code, expanding macros.
	StringList processCode(CGCodeBlock&);
	StringList processCode(const char*);

	SymbolList codeblockSymbol;
	SymbolList starSymbol;

	// Process the string and add it to the Target code.
	void addCode(const char*);

	// For temporary backward compatibility.
	void gencode(const char* code) { addCode(code); };

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
	virtual StringList expandMacro(const char* func, const StringList& argList);

	// Match macro name and argument count.
	int matchMacro(const char* func, const StringList& argList, const char* name, int argc)
	{
	    return (strcasecmp(func, name) == 0) && (argList.size() == argc);
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

	// lookup a code StringList that is supported by the target.  If
	// the stream does not exist, Error::abortRun is called.  The
	// target should set up pointers that are set to the StringLists they
	// need rather that allowing functions to call this routine directly.
	// Initialize the pointers by overloading the star public member
	// function void setTarget(Target* ).
	StringList* getStream(const char* name);
	
	// Update all PortHoles so that the offset is incremented by the
	// number of samples consumed or produced.
	void advance();

	// processor id 
	IntState procId;

	// declare that I am a Fork star
	void isaFork() { forkId = TRUE; }

	// do initialization for fork stars.
	virtual void forkInit(CGPortHole& input,MultiCGPort& output);
	virtual void forkInit(CGPortHole& input,CGPortHole& output);

	// lookup a shared symbol by list name & symbol name, if it is 
	// not found Error::abortRun is called.
	const char* lookupSharedSymbol(const char* list,const char* name);	

	// add a SymbolList to the list of symbol lists.  For a SymbolList
	// to be shared it should be declared static.  If a SymbolList
	// is found with the same name but different pointer Error::abortRun
	// is called.  If a SymbolList is found with the same name and
	// same pointer, it is ignored.  This command should be called in 
	// the defining star's constructor.
	void addSharedSymbolList(SymbolList* list, const char* name);

	// modify "deferrable": always defer stars that feed into forks,
	// to keep minimum possible size for fork buffers.
	int deferrable();

private:

	// List of all shared symbol lists.  To add a shared symbol list
	// use the addSharedSymbolList(name) method.  To lookup a shared
	// symbol use the lookupSharedSymbol(list_name,symbol_name) method.
	SymbolListList sharedSymbolLists;

	// Reset local codeblock labels
	void resetCodeblockSyms(){ codeblockSymbol.initialize(); }

	// indicate if a fork star
	int forkId;
	
	// Main code stream, the reference is set in the setTarget method.
	StringList* code;
	
};

class CGStarPortIter : public BlockPortIter {
public:
	CGStarPortIter(CGStar& s) : BlockPortIter(s) {}
	CGPortHole* next() { return (CGPortHole*)BlockPortIter::next();}
	CGPortHole* operator++() { return next();}
};

#endif
