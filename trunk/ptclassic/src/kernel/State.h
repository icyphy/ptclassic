#ifndef _State_h
#define _State_h 1

#include "DataStruct.h"
#include "StringList.h"
#include "Output.h"
#include "type.h"
#include "NamedObj.h"
#include "ComplexSubset.h"

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

class ParseToken {
public:
	char* tok;
	union {
		char cval;
		const char *sval;
		int intval;
		double  doubleval;
		Complex* Complexval;
		State*  s;
	}; 
};

////////////////////////////////////////////
// class State
////////////////////////////////////////////

class State : public NamedObj
{
public:

        // Constructor
        State()  {};

        // Destructor.  Nothing to destroy here, but make it virtual
        // so subclasses can be destroyed given only a State*
        virtual ~State() {};

	// Method setting internal data  in the State
	State& setState(const char* stateName,
			Block* parent,
			const char* ivalue){
					setNameParent(stateName, parent);
					initValue = ivalue;
					return *this;
	}

        // same, but set descriptor too
        State& setState(const char* stateName, 
			Block* parent ,
			const char* ivalue,
			const char* desc) {
               		 		descriptor = desc;
               		 		return setState(stateName,parent,ivalue);
        }


	// set the initial value
	setValue(const char*  s) { initValue = s;}

        // return the parameter type (for use in GUI, interpreter)
        virtual char* type() { return "STRING";}

        // return the parameter size (redefined for array states)
        virtual int size() { return 1;}

	// Initialize when starting or restarting a simulation
	virtual void initialize(){};

	// get Token  from  string 
	ParseToken getParseToken(Tokenizer&, Block*, char*);

	// get Token  from  string 
	ParseToken getParseToken(Tokenizer&, Block*);

	// lookup state from name
	State* lookup(char*, Block*);	

        // return the current value as a string.  Here we just give
        // back initValue
	virtual StringList currentValue() { return initValue;}

	// force all state classes to redefine this
	virtual State* clone() = 0;

	// output all info.  This is NOT redefined for each type of state
	StringList printVerbose();

protected:
	// string used to set initial value by initialize()
	const char* initValue;
};

///////////////////////////////////////////
// class StateList
///////////////////////////////////////////

//  This class is used to store a list of states in a Block

class StateList : public  SequentialList
{
	friend class Block;
	friend class State;
	friend class KnownState;
	
	// Add State to list
	void put(State& s) {SequentialList::put(&s);}

	// Return size of list
	int size() const {return SequentialList::size();}
       
	// Reset the list to beginning
        void reset() {SequentialList::reset();}

	// Return next State on list
	State& operator ++ () {return *(State*)next();}

	State * next() {return (State*)SequentialList::next();}

	// initialize elements
	void initElements();

	// Find a state with the given name and return pointer
	State* stateWithName(const char* name) ;

};
#endif
