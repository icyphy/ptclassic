static const char file_id[] = "MotorolaTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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

int MotorolaTarget :: setup(Galaxy& g) {
	LOG_DEL; delete mem; mem = 0;
	LOG_NEW; mem = new MotorolaMemory(xMemMap,yMemMap);
	return AsmTarget::setup(g);
}

void MotorolaTarget :: wrapup () {
	StringList map = mem->printMemMap(";","");
	addCode (map);
	AsmTarget::wrapup();
}

MotorolaTarget :: ~MotorolaTarget () {
	LOG_DEL; delete mem; mem = 0;
}

// copy constructor
MotorolaTarget :: MotorolaTarget (const MotorolaTarget& src) :
AsmTarget(src.readName(),src.readDescriptor(),src.starType())
{
	initStates();
	copyStates(src);
}

// clone
Block* MotorolaTarget :: clone () const {
	LOG_NEW; return new MotorolaTarget(*this);
}

void MotorolaTarget::beginIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1) {	// iterate infinitely
		out = targetNestedSymbol.push("LOOP");
		out += "\n";
	}
	else {				// iterate finitely
		out = "\tdo\t#";
		out += repetitions;
		out += ",";
		out += targetNestedSymbol.push("LOOP");
		out += "\n";
	}
	addCode(out);
}

void MotorolaTarget::endIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)	{	// iterate infinitely
		out = "\tjmp\t";
		out += targetNestedSymbol.pop();
		out += "\n";
	}
	else {				// iterate finitely
		out = targetNestedSymbol.pop();
		out += "\n\tnop\t\t; prevent two endloops in a row\n";
	}
	addCode(out);
}

void MotorolaTarget::codeSection() {
	if (!inProgSection) {
		addCode("\torg p:\n");
		inProgSection = 1;
	}
}

void MotorolaTarget::disableInterrupts() {
	codeSection();
	addCode("	ori	#03,mr	;disable interrupts\n");
}

void MotorolaTarget::enableInterrupts() {
	codeSection();
	addCode("	andi	#$fc,mr	;enable interrupts\n");
}

void MotorolaTarget::saveProgramCounter() {
	codeSection();
	StringList spc;
	spc = targetNestedSymbol.push("SAVEPC");
	spc += "\tequ	*\n";
	addCode(spc);
}

void MotorolaTarget::restoreProgramCounter() {
	codeSection();
	StringList spc;
	spc = "\torg	p:";
	spc += targetNestedSymbol.pop();
	spc += "\n";
	addCode(spc);
}

void MotorolaTarget::orgDirective(const char* memName, unsigned addr) {
	StringList out = "\torg\t";
	out += memName;
	out += ":";
	out += int(addr);
	out += "\n";
	addCode(out);
	inProgSection = 0;
}

void MotorolaTarget::writeInt(int val) {
	StringList out = "\tdc\t";
	out += val;
	out += "\n";
	addCode(out);
}

void MotorolaTarget::writeFix(double val) {
	StringList out = "\tdc\t";
	out += limitFix(val);
	out += "\n";
	addCode(out);
}

void MotorolaTarget::writeFloat(double val) {
	StringList out = "\tdc\t";
	out += val;
	out += "\n";
	addCode(out);
}


