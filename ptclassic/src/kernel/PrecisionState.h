#ifndef _PrecisionState_h
#define _PrecisionState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"
#include "Fix.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  J.Weiss
 Revisions:

 State for precision specification for fixed-point classes.
 See Fix.h for format definitions of precision values.

**************************************************************************/

///////////////////////////////////////////
// class  Precision
///////////////////////////////////////////

// Class to describe the fixed-point precision of some state, port or var-
// iable by the tuple (total length,length of the integer part).
// In order to facilitate the handling of precisions in the CGC domain, the
// tuple may consist of either integers or symbolic expressions (for example
// variable names) or both.

// A couple of arithmetic operators are defined that return the precision of
// the result of the operaton applied to two fix variables with respective
// precisions.  For symbolic expressions the resulting precision is generated
// by literal text concatenation to create C expressions using the +/- operators
// and a function or macro call named "MAX(x,y)" that should return the maximum
// of x and y.

class Precision {

public:
	// con/destructors
	Precision();
	Precision(int len, int intb,
	    const char* symbolic_length=NULL, const char* symbolic_intBits=NULL);
	Precision(const Fix&,
	    const char* symbolic_length=NULL, const char* symbolic_intBits=NULL);
	Precision(const Precision&);

	~Precision();

	// return length in bits
	int len() const   { return isValid() ? length  : 0; }
	// return bits to right of binary point
	int intb() const  { return isValid() ? intBits : 0; }
	// return bits to left of binary point
	int fracb() const { return isValid() ? length - intBits : 0; }

	// obsolete
	int precision() const { return fracb(); }

	// return symbolic expression for length
	const char* symbolic_len() const  { return symbolic_length; }
	// return symbolic expression for bits to right of binary point
	const char* symbolic_intb() const { return symbolic_intBits; }

	// return FALSE for invalid or uninitialized data
	int isValid() const {
		return !(length < intBits || length == 0) ||
		        (symbolic_len() && symbolic_intb());
	}

	// return TRUE if the symbolic fields are defined
	int isSymbolic() const {
		return symbolic_len()|| symbolic_intb();
	}

	// convert precision into string of form intb.fracb
	operator StringList () const;

	// print on ostream with format "intb.fracb" (including quotes)
	// if they are defined, symbolic expressions rather than integer
	// expressions are used
	friend ostream& operator << (ostream&, const Precision&);
	// dito, for StringLists with format len,intb
	friend StringList& operator << (StringList&, const Precision&);

	// operators returning the resulting precision for the
	// specified arithmetic operation with two FIX operands
	
	Precision  operator +  (const Precision&) const;
	Precision  operator -  (const Precision&) const;
	Precision  operator *  (const Precision&) const;
	Precision  operator /  (const Precision&) const;

	Precision& operator += (const Precision&);
	Precision& operator -= (const Precision&);
	Precision& operator *= (const Precision&);
	Precision& operator /= (const Precision&);

	// comparison operators
	int operator == (const Precision&) const;
	int operator != (const Precision& p) const { return !(*this == p); }

	// assignment
	Precision& operator = (const Precision&);

protected:
	uchar length;        // # of significant bits 
	uchar intBits;       // # of bits to the left of the binary point 

	// symbolic expressions for the precision fields
	char* symbolic_length, *symbolic_intBits;
};


///////////////////////////////////////////
// class  PrecisionState
///////////////////////////////////////////

// new parameter value for PrecisionState::getParseToken
const int T_Precision = 500;

class PrecisionState :  public Precision, public State
{

public:
        // con/destructors
        PrecisionState() { val = NULL; }
	~PrecisionState();

	// default copy constructor
	PrecisionState(const &);

        // parses initValue to set value
        void initialize();

        // the type
	const char* type() const; // { return "PRECISION"; }

        // the value as a string
	//StringList currentValue() const {return (const char*)*this;}
	StringList currentValue() const;

        // assignment from a string
        PrecisionState& operator = (const char*);
	// assignment from another PrecisionState
	PrecisionState& operator = (const PrecisionState&);
	// assignment from a Precision
	//PrecisionState& operator = (const Precision&);

        // casting to a string
        operator const char* () const { return val ? val:""; }

	// return a Fix with value 0.0 and the precision of the state
	operator Fix () const { return Fix(len(),intb()); }
	
	// class identification
	int isA(const char*) const;
	const char* className() const {return "PrecisionState";}

	State* clone () const;//  {return new PrecisionState;}

	// this redeclaration is required by the "hiding rule".  Yuk!
	void setInitValue(const char* s) { State::setInitValue(s);}

	// parse precision string of the form "intBits.fracBits" or "fracBits/length" 
	// where each expression may contain operators and references to other states.
	static Precision parsePrecisionString(
		Tokenizer&, const char* name, Block* parent);

protected:
	// redefine token parser from class State
	virtual ParseToken getParseToken(Tokenizer&, int = T_Float);

	// non-static parser
	Precision parsePrecisionString(Tokenizer&);
private:
	// the precision as a string
	char* val;
};

#endif
