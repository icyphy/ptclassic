#ifndef _State_h
#define _State_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "StringList.h"
#include "Error.h"
#include "type.h"
#include "NamedObj.h"
#include "ComplexSubset.h"
#include "Attribute.h"

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

 Programmer: I. Kuroda and J. T. Buck 
 Date of creation: 5/26/90
 Revisions:

 State is a data member of a Block, it is where paremeters and
 state information is stored for that Block.

**************************************************************************/

class Block;
class State;

class Tokenizer;

// token types: from state parser
const int T_EOF = 257;
const int T_ERROR = 258;
const int T_Float = 259;
const int T_Int = 260;
const int T_ID = 261;
const int T_STRING = 262;

// token, from state parser.
class ParseToken {
public:
	int tok;
	union {
		char cval;
		const char *sval;
		int intval;
		double  doubleval;
		Complex* Complexval;
		const State*  s;
	}; 
	ParseToken () { tok = 0; intval = 0;}
};

////////////////////////////////////////////
// class State
////////////////////////////////////////////

// attribute bit definitions.  The kernel reserves the next two bits
// (8, 16) for future expansion; domains may use higher-order bits
// for their own purposes (example: code generation domains do this).

const bitWord AB_CONST = 1;
const bitWord AB_SETTABLE = 2;
const bitWord AB_DYNAMIC = 4;
const bitWord AB_DEFAULT = AB_CONST | AB_SETTABLE;

// standard attributes for all domains.
extern const Attribute A_CONSTANT;	// value never changed by star exec.
extern const Attribute A_SETTABLE;	// user may set this state
extern const Attribute A_NONCONSTANT;	// value changed by star exec.
extern const Attribute A_NONSETTABLE;	// user may not set this state
extern const Attribute A_DYNAMIC;	// state may be modified externally
					// during execution.
class State : public NamedObj
{
public:

        State() : myInitValue(0), attributeBits(AB_DEFAULT) {}
	~State();

	// Method setting internal data  in the State
        State& setState(const char* stateName, 
			Block* parent ,
			const char* ivalue,
			const char* desc = NULL);

	// same, but this one also sets attributes.
        State& setState(const char* stateName, 
			Block* parent ,
			const char* ivalue,
			const char* desc,
			Attribute attr);

	// set the initial value
	void setInitValue(const char* s);

	// get the initial value
	const char* initValue () const { return myInitValue;}

        // return the parameter type (for use in GUI, interpreter)
        virtual const char* type() const = 0;

	virtual const char* className() const = 0;

        // return the parameter size (redefined for array states)
        virtual int size() const;

	// am I an array state? (default FALSE)
	virtual int isArray() const;

	// Initialize when starting or restarting a simulation
	virtual void initialize() = 0;

        // return the current value as a string.  Here we just give
        // back initValue
	virtual StringList currentValue() const = 0;

	// modify the current value, in a type-independent way
	void setCurrentValue(const char* newval);

	// force all state classes to redefine this: produce a new
	// State of identical type.
	virtual State* clone() const = 0;

	// output all info.  This is NOT redefined for each type of state
	StringList print(int verbose) const;

	// attributes
	bitWord attributes() const { return attributeBits;}

	bitWord setAttributes(const Attribute& attr) {
		return attributeBits = attr.eval(attributeBits);
	}

	bitWord clearAttributes(const Attribute& attr) {
		return attributeBits = attr.clearAttribs(attributeBits);
	}

	// class identification
	int isA(const char*) const;

protected:
	// get Token  from  string 
	ParseToken getParseToken(Tokenizer&, int = T_Float);

	// lookup state from name
	const State* lookup(const char*, Block*);

	// complain of parse error
	void parseError (const char*, const char* = "");

	// pushback token, for use in parsing
	static ParseToken pushback;

	// expression evaluation functions
	ParseToken evalIntExpression(Tokenizer& lexer);
	ParseToken evalIntTerm(Tokenizer& lexer);
	ParseToken evalIntFactor(Tokenizer& lexer);
	ParseToken evalIntAtom(Tokenizer& lexer);

	ParseToken evalFloatExpression(Tokenizer& lexer);
	ParseToken evalFloatTerm(Tokenizer& lexer);
	ParseToken evalFloatFactor(Tokenizer& lexer);
	ParseToken evalFloatAtom(Tokenizer& lexer);

private:
	// string used to set initial value by initialize()
	const char* myInitValue;

	// attributes
	bitWord attributeBits;
};

///////////////////////////////////////////
// class StateList
///////////////////////////////////////////

//  This class is used to store a list of states in a Block

class StateList : private NamedObjList
{
	friend class StateListIter;
	friend class CStateListIter;
public:
	// Add State to list
	void put(State& s) {NamedObjList::put(s);}

	// Find a state with the given name and return pointer
	State* stateWithName(const char* name) {
		return (State*)objWithName(name);
	}

	const State* stateWithName(const char* name) const {
		return (const State*)objWithName(name);
	}

	NamedObjList::size;
	NamedObjList::initElements;
	NamedObjList::deleteAll;
};

///////////////////////////////////////////
// class StateListIter
///////////////////////////////////////////

// an iterator for StateList
class StateListIter : private NamedObjListIter {
public:
	StateListIter(StateList& sl) : NamedObjListIter (sl) {}
	State* next() { return (State*)NamedObjListIter::next();}
	State* operator++() { return next();}
	NamedObjListIter::reset;
};

#endif
