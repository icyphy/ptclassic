static const char file_id[] = "MotorolaTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1992 The Regents of the University of California.
All Rights Reserved.

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

 Programmer: J. Buck and J. Pino

 Base target for Motorola DSP assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaTarget.h"

const Attribute ANY = {0,0};

// a MotorolaMemory represents the X and Y memories of a 56000 or 96000.  
// It is derived from DualMemory.
MotorolaMemory :: MotorolaMemory(const char* x_map, const char* y_map) :
	DualMemory("x",A_XMEM,ANY,x_map,"y",A_YMEM,ANY,y_map)
{}

MotorolaTarget :: MotorolaTarget (const char* nam, const char* desc,
	const char* stype): AsmTarget(nam,desc,stype)
{
	initStates();
}

void MotorolaTarget :: initStates() {
	inProgSection = 0;
 	mem = 0;
	addState(xMemMap.setState("xMemMap",this,"0-4095","X memory map"));
	addState(yMemMap.setState("yMemMap",this,"0-4095","Y memory map"));
}

void MotorolaTarget :: setup() {
	LOG_DEL; delete mem;
	LOG_NEW; mem = new MotorolaMemory(xMemMap,yMemMap);
	AsmTarget::setup();
}

MotorolaTarget :: ~MotorolaTarget () {
	LOG_DEL; delete mem; mem = 0;
}

// copy constructor
MotorolaTarget :: MotorolaTarget (const MotorolaTarget& src) :
AsmTarget(src.name(),src.descriptor(),src.starType())
{
	initStates();
	copyStates(src);
}

// makeNew
Block* MotorolaTarget :: makeNew () const {
	LOG_NEW; return new MotorolaTarget(*this);
}

void MotorolaTarget::beginIteration(int repetitions, int) {
    if (repetitions == -1)		// iterate infinitely
	myCode << targetNestedSymbol.push("LOOP") << "\n";
    else				// iterate finitely
	myCode << "\tdo\t#" << repetitions << "," 
	       << targetNestedSymbol.push("LOOP") << "\n";
}

void MotorolaTarget::endIteration(int repetitions, int) {
	if (repetitions == -1)		// iterate infinitely
		myCode << "\tjmp\t"<< targetNestedSymbol.pop() << "\n";
	else 				// iterate finitely
		myCode << "\tnop\n; prevent two endloops in a row\n"
		       << targetNestedSymbol.pop() << "\n";
}

void MotorolaTarget::codeSection() {
	if (!inProgSection) {
		myCode << "\torg p:\n";
		inProgSection = 1;
	}
}

void MotorolaTarget::disableInterrupts() {
	codeSection();
	myCode << "	ori	#03,mr	;disable interrupts\n";
}

void MotorolaTarget::enableInterrupts() {
	codeSection();
	myCode << "	andi	#$fc,mr	;enable interrupts\n";
}

void MotorolaTarget::saveProgramCounter() {
	codeSection();
	myCode << targetNestedSymbol.push("SAVEPC") << "\tequ	*\n";
}

void MotorolaTarget::restoreProgramCounter() {
	codeSection();
	myCode << "\torg	p:" << targetNestedSymbol.pop() << "\n";
}

void MotorolaTarget::orgDirective(const char* memName, unsigned addr) {
	myCode << "\torg\t" << memName << ":" << int(addr) << "\n";
	inProgSection = 0;
}

void MotorolaTarget::writeInt(int val) {
	myCode << "\tdc\t" << val << "\n";
}

void MotorolaTarget::writeFix(double val) {
	myCode << "\tdc\t" << limitFix(val) << "\n";
}

void MotorolaTarget::writeFloat(double val) {
	myCode << "\tdc\t" << val << "\n";
}

double MotorolaTarget::limitFix(double val) { 
	const double limit = 1.0 - 1.0/double(1<<23);
	if (val >= limit) return limit;
	else if (val <= -1.0) return -1.0;
	else return val;
}

StringList MotorolaTarget::comment(const char* msg, const char* begin,
const char* end, const char* cont) {
	if (begin==NULL) return CGTarget::comment(msg,"; ");
	else return CGTarget::comment(msg,begin,end,cont);
}
