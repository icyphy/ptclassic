static const char file_id[] = "CG56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Target.h"
#include "CG56Star.h"

const Attribute ANY(0,0);

// a CG56Memory represents the X and Y memories of a 56000.  It is
// derived from DualMemory.
CG56Memory :: CG56Memory(const char* x_map, const char* y_map) :
	DualMemory("x",A_XMEM,ANY,x_map,"y",A_YMEM,ANY,y_map)
{}

CG56Target :: CG56Target(const char* nam, const char* desc) :
	inProgSection(0),
	AsmTarget(nam,desc,"CG56Star")
{
	initStates();
	mem = 0;
}

void CG56Target :: initStates() {
	addState(xMemMap.setState("xMemMap",this,"0-4095","X memory map"));
	addState(yMemMap.setState("yMemMap",this,"0-4095","Y memory map"));
}

int CG56Target :: setup(Galaxy& g) {
	LOG_DEL; delete mem;
	LOG_NEW; mem = new CG56Memory(xMemMap,yMemMap);
	return AsmTarget::setup(g);
}

void CG56Target :: headerCode () {
        StringList code = "generated code for target ";
        code += readFullName();
	outputComment (code);
}

void CG56Target :: wrapup () {
	StringList map = mem->printMemMap(";","");
	addCode (map);
	CGTarget::wrapup();
}

CG56Target :: ~CG56Target () {
	LOG_DEL; delete mem;
}

// copy constructor
CG56Target :: CG56Target (const CG56Target& src) :
AsmTarget(src.readName(),src.readDescriptor(),"CG56Star"), inProgSection(0)
{
	initStates();
	copyStates(src);
}

// clone
Block* CG56Target :: clone () const {
	LOG_NEW; return new CG56Target(*this);
}

StringList CG56Target::beginIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)		// iterate infinitely
		out = "label\n";
	else {				// iterate finitely
		out = "\tdo\t#";
		out += repetitions;
		out += ",label\n";
	}
	return out;
}

StringList CG56Target::endIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)		// iterate infinitely
		out = "\tjmp\tlabel\n";
	else				// iterate finitely
		out = "label\n";
	return out;
}

void CG56Target::codeSection() {
	if (!inProgSection) {
		addCode("\torg p:\n");
		inProgSection = 1;
	}
}

void CG56Target::orgDirective(const char* memName, unsigned addr) {
	StringList out = "\torg\t";
	out += memName;
	out += ":";
	out += int(addr);
	out += "\n";
	addCode(out);
	inProgSection = 0;
}

void CG56Target::writeInt(int val) {
	StringList out = "\tdc\t";
	out += val;
	out += "\n";
	addCode(out);
}

void CG56Target::writeFix(double val) {
	StringList out = "\tdc\t";
	out += CG56Star::printFixedPointValue(val);
	out += "\n";
	addCode(out);
}

void CG56Target::writeFloat(double val) {
	StringList out = "; WARNING: the M56000 does not support floating point!\n";
	out += "; perhaps this state was meant to be type FIX?\n";

	out += "\tdc\t";
	out += val;
	out += "\n";
	addCode(out);
}


