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

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda and J. T. Buck 
 Date of creation: 5/26/90
 Revisions:

 State is a data member of a Block, it is where data is stored in
 a Block.

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

// attribute bit definitions.  The kernel reserves the next three bits
// (4, 8, 16) for future expansion; domains may use higher-order bits
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

        // Constructor
        State() : initValue(0), attributeBits(AB_DEFAULT) {}

	// Method setting internal data  in the State
        State& setState(const char* stateName, 
			Block* parent ,
			const char* ivalue,
			const char* desc = NULL) {
               		 	descriptor = desc;
				setNameParent(stateName, parent);
				initValue = ivalue;
				return *this;
        }

	// same, but this one also sets attributes.
        State& setState(const char* stateName, 
			Block* parent ,
			const char* ivalue,
			const char* desc,
			const Attribute& attr) {
				attributeBits = attr.eval(AB_DEFAULT);
				return setState(stateName,parent,ivalue,desc);
        }

	// set the initial value
	void setValue(const char*  s) { initValue = s;}

	// get the initial value
	const char* getInitValue () const { return initValue;}

        // return the parameter type (for use in GUI, interpreter)
        virtual const char* type() const;

        // return the parameter size (redefined for array states)
        virtual int size() const;

	// Initialize when starting or restarting a simulation
	// (inherits from NamedObj)
	// virtual void initialize(){};

        // return the current value as a string.  Here we just give
        // back initValue
	virtual StringList currentValue() const;

	// modify the current value, in a type-independent way
	void setCurrentValue(const char* newval);

	// force all state classes to redefine this: produce a new
	// State of identical type.
	virtual State* clone() const = 0;

	// output all info.  This is NOT redefined for each type of state
	StringList printVerbose() const;

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
	const char* readClassName() const {return "State";}

protected:
	// string used to set initial value by initialize()
	const char* initValue;

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
	bitWord attributeBits;
};

///////////////////////////////////////////
// class StateList
///////////////////////////////////////////

//  This class is used to store a list of states in a Block

class StateList : public  SequentialList
{
public:
	// Add State to list
	void put(State& s) {SequentialList::put(&s);}

	// Return size of list
	int size() const {return SequentialList::size();}
       
	// initialize elements
	void initElements();

	// Find a state with the given name and return pointer
	State* stateWithName(const char* name) ;

};

///////////////////////////////////////////
// class StateList
///////////////////////////////////////////

// an iterator for StateList
class StateListIter : private ListIter {
public:
	StateListIter(StateList& sl) : ListIter (sl) {}
	State* next() { return (State*)ListIter::next();}
	State* operator++() { return next();}
	ListIter::reset;
};

#endif
