static const char file_id[] = "TITarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: A. Baensch and Luis Gutierrez 
 Date of creation: 6 May 1995

 Base target for TI  DSP assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "miscFuncs.h"
#include "Galaxy.h"
#include "TITarget.h"
#include "TIAttributes.h"

#define C50_MAX_LABEL_LENGTH 5
#define C50_BITS_IN_WORD 16

const Attribute ANY = {0,0};

// TIMemory represents the B1 and Users Data (UD) memories of a 320C5x.
// The B0 data memory is mapped into program memory.   
// It is derived from DualMemory.
TIMemory :: TIMemory(const char* b_map, const char* u_map) :
	DualMemory("l", "b", A_BMEM, ANY, b_map, "u", A_UMEM, ANY, u_map)
{
}

TITarget :: TITarget (const char* nam, const char* desc, const char* stype):
	AsmTarget(nam,desc,stype)
{
	initStates();
}

void TITarget :: initStates() {
	inProgSection = 0;
 	mem = 0;
	addState(bMemMap.setState("bMemMap",this,"768-1279","B memory map"));
	addState(uMemMap.setState("uMemMap",this,"2432-6848","UD memory map"));
	addState(subFire.setState("subroutines?",this,"-1",
	    "Write star firings as subroutine calls."));
	destDirectory.setInitValue("$HOME/PTOLEMY_SYSTEMS/C50");
	maxFixedPointValue = 1.0 - 1.0 / double(1 << (C50_BITS_IN_WORD - 1));
	minFixedPointValue = -1.0;
}

// complex numbers will be allocated in 2 consecutive words of memory.
void TITarget :: setup() {
	LOG_DEL; delete mem;
	LOG_NEW; mem = new TIMemory(bMemMap,uMemMap);

	AsmTarget::setup();

	GalStarIter nextStar(*galaxy());
	AsmStar* s;
	int portSize;
	while ((s = (AsmStar*)nextStar++) != 0){
	    BlockPortIter next(*s);
	    AsmPortHole * p;
	    while((p = (AsmPortHole*) next++) != 0) {
		// allocate complex numbers in 2 consecutive words of memory
		if (p->resolvedType() == COMPLEX ){
		    portSize = p->numXfer();
		    portSize = 2*portSize;
		    p->setSDFParams(portSize,portSize-1);
		}
	    }
	}
}

TITarget :: ~TITarget () {
	LOG_DEL; delete mem; mem = 0;
}

// copy constructor
TITarget :: TITarget (const TITarget& src) :
AsmTarget(src.name(),src.descriptor(),src.starType())
{
	initStates();
	copyStates(src);
}

// makeNew
Block* TITarget :: makeNew () const {
	LOG_NEW; return new TITarget(*this);
}

// either iterate for a finite number of repetitions of the schedule
// or iterate for an infinite amount.  If we iterate for a finite
// number, the stack (8 levels deep) which is used to hold the loop
// count for nested loops will overflow if there are more than 8
// levels of nested loops.  Also using the stack within a codeblock
// will most likely break the program.
void TITarget::beginIteration(int repetitions, int) {
	if (repetitions == -1) {		// iterate infinitely
	    *defaultStream << targetNestedSymbol.push("LOOP") << "\n";
	}
	else {				// iterate finitely
	    *defaultStream << "\tlacc\t#" << repetitions << "\n"
			   << targetNestedSymbol.push("LOOP") << "\tpush\n";
	}
}

void TITarget::endIteration(int repetitions, int) {
	if (repetitions == -1) {		// iterate infinitely
	    *defaultStream << "\tb\t"<< targetNestedSymbol.pop() << "\n";
	}
	else {					// iterate finitely
	    *defaultStream << "\tnop\n* prevent two endloops in a row\n"
			   << "\tpop\n\tsub\t#1\n"
			   << "\tbcnd\t" << targetNestedSymbol.pop() << ",GT\n";
	}
}

void TITarget::codeSection() {
	if (!inProgSection) {
		*defaultStream << "\t.text \n";
		inProgSection = 1;
	}
}
 
void TITarget::disableInterrupts() {
	codeSection();
	*defaultStream << "\tsetc	INTM		;disable interrupts\n";
}

void TITarget::headerCode() {
	CGTarget::headerCode();
}

void TITarget::enableInterrupts() {
	codeSection();
	*defaultStream  << "\tclrc	INTM		;enable interrupts\n";
}

// FIXME: memName is unused
void TITarget::orgDirective(const char* /*memName*/, unsigned addr) {
	*defaultStream << "\t.ds  \t" << int(addr) << "\n";
	inProgSection = 0;
}

void TITarget::writeInt(int val) {
	*defaultStream << "\t.int\t" << val << "\n";
}

void TITarget::writeFix(double val) {
	*defaultStream << "\t.q15\t" << limitFix(val) << "\n";
}

void TITarget::writeFloat(double val) {
	*defaultStream << "\t.float\t" << val << "\n";
}

void TITarget::trailerCode() {
	trailer << "\tb\tENDE		;jump to end of program\n";
}

// Put a STOP instruction at the end of program memory.
void TITarget::frameCode() {
    AsmTarget::frameCode();
    *defaultStream << "\t.ps 	02b00h\n"
	   << "AIC_2ND sach    DXR\n"
	   << "\tclrc    INTM\n"
	   << "\tidle\n"
	   << "\tadd     #6h,15\n"
	   << "\tsach    DXR\n"
	   << "\tidle\n"
	   << "\tsacl    DXR\n"
	   << "\tidle\n"
	   << "\tlacl    #0\n"
	   << "\tsacl    DXR\n"
	   << "\tidle\n"
	   << "\tsetc    INTM\n"
	   << "\tret\n"
	   << "TINT	RETE\n"
	   << "RINT	RETE\n"
	   << "XINT	RETE\n"
	   << "ENDE\n\t.end\n";	
}

// Ensure that the value val is in the interval [-1,1)
double TITarget::limitFix(double val) { 
	if (val >= maxFixedPointValue) return maxFixedPointValue;
	else if (val <= minFixedPointValue) return minFixedPointValue;
	else return val;
}

StringList TITarget::comment(const char* msg, const char* begin,
			     const char* end, const char* cont) {
	return (begin) ? CGTarget::comment(msg, begin, end, cont) :
			 CGTarget::comment(msg, "; ");
}

// Determine whether or not the star firing can be implemented with
// static code which could be enclosed in a loop or subroutine.
static int staticCode(CGStar& star) {
    BlockPortIter nextPort(star);
    CGPortHole* port;

    // Test the parameters of all ports.
    while ( (port = (CGPortHole*)nextPort++) != 0) {
	// If the buffer size is not the same as the number of
	// particles transferred in one firing, then each firing must
	// read from a different location.
	if (port->numXfer() != port->bufSize()) {
	    if ((port->attributes() & PB_CIRC) == 0) return FALSE;
	}
    }
    return TRUE;
}

void TITarget::writeFiring(Star& s, int level) {
    CGStar& star = (CGStar&)s;
    int threshold = int(subFire);

    if (threshold >= 0 && star.reps() > threshold && staticCode(star)) {
	if (star.index() < 0) setStarIndices(*galaxy());

	// Get the first five characters of name of the star without
	// the domain name of C50; e.g., C50WasteCycles would become Waste
	StringList label;
	const char* classname = star.className();
	int domainNameLength = strlen(star.domain());
	char* starname = savestring(&classname[domainNameLength]);
	starname[C50_MAX_LABEL_LENGTH] = 0;
	label << starname << separator << star.index();

	// Generate procedure definition.
	if (procedures.put("",label)) {
	    procedures << label << '\n';
	    CodeStream* previous = defaultStream;
	    defaultStream = &procedures;
	    AsmTarget::writeFiring(star, level);
	    defaultStream = previous;
	    procedures << "\tret\n";
	}

	// Invoke procedure.
	mainLoop << "\tcall\t " << label << "\n";

	delete [] starname;
    }
    else {
	AsmTarget::writeFiring(s,level);
    }
}
