static const char file_id[] = "CGTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGTarget.h"
#include "CGStar.h"
#include "GalIter.h"
#include "Error.h"
#include "UserOutput.h"
#include "SDFScheduler.h"
#include "CGDisplay.h"
#include "miscFuncs.h"

// Return a string for indenting to the given depth
StringList CGTarget::indent(int depth) {
	StringList out;
	out = "";
	for(int i=0; i<depth; i++) {
		out += "    ";
	}
	return out;
}

int CGTarget :: decideBufSize(Galaxy& g) { return TRUE; }
int CGTarget :: codeGenInit(Galaxy& g) { return TRUE; }

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc, char sep = '_') : Target(name,starclass,desc) {
	separator = sep;
}

void CGTarget :: initialize() {
	myCode.initialize();
	Target::initialize();
}

int CGTarget::setup(Galaxy& g) {
	// reset the label counter
	numLabels = 0 ;
	CGStar* s;
	GalStarIter next(g);
	while ((s = (CGStar*)next++) != 0 ) {
		s->codeblockSymbol.setTarget(this);
		s->starSymbol.setTarget(this);
	}
	if (!Target::setup(g)) return FALSE;

	// decide the buffer size
	if(!decideBufSize(g)) return FALSE;

	return codeGenInit(g);
}

void CGTarget :: start() {
	if (!mySched() && !parent()) {
		LOG_NEW; setSched (new SDFScheduler);
	}
	headerCode();
}

int CGTarget :: run() {
	// Sorry about the following horrible cast.
	// Design of kernel Scheduler makes it very difficult to avoid
	int iters = ((SDFScheduler*)mySched())->getStopTime();
	StringList startIter = beginIteration(iters,1);
	addCode(startIter);
	mySched()->setStopTime(1);
	int i = Target::run();
	StringList endIter = endIteration(iters,1);
	addCode(endIter);
	return i;
}

Block* CGTarget :: clone() const {
	LOG_NEW; return new CGTarget(readName(),starType(),readDescriptor());
}

void CGTarget :: addCode(const char* code) {
	if(code) myCode += code;
}

void CGTarget :: headerCode () {
	StringList code = "/* generated code for target ";
	code += readFullName();
	code += " */\n";
	addCode(code);
}

void CGTarget :: wrapup() {
	Target :: wrapup();
	display(myCode);
}

void CGTarget :: writeCode(UserOutput& o) {
	o << myCode;
	o.flush();
}

ISA_FUNC(CGTarget,Target);

CGTarget :: ~CGTarget() { delSched();}

void CGTarget :: outputComment (const char* msg) {
	StringList code = "/* ";
        code += msg;
        code += " */\n";
        addCode(code);
}

Symbol::Symbol (CGTarget* target=0) {
	initialize();
	myTarget = target;
}
	
// Lookup unique symbol, if one doesn't exist, create new symbol.
// Symbols are stored in pairs: key, symbol.
StringList Symbol::lookup(const char* name) {
	StringListIter next(symbols);
	const char* p;
    	StringList s;
    	while ((p = next++) != 0) {
		s = next++;
		if(!strcmp(name,p)) return s;
   	}
   	// name is not on the list.  add it.
  	symbols += name;
  	s = name;
	s += myTarget->separator;
    	s += myTarget->numLabels++;
    	// add the new symbol as well
   	symbols += s;
    	return s;
}
	

NestedSymbol::NestedSymbol (CGTarget* target=0) {
	initialize();
	myTarget = target;
}
	
const char* NestedSymbol::push(const char* name="L") {
	StringList s;
   	s = name;
	s += myTarget->separator;
    	s += myTarget->numLabels++;
	const char* temp = s;
   	symbols.push(temp);
    	return savestring(temp);
}

const char* NestedSymbol::pop() {
	if(depth()==0) return 0;
	return symbols.pop();

}
	

