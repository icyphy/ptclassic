static const char file_id[] = "CG56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and J. Pino

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Target.h"
#include "CG56Star.h"
#include <ctype.h>

const Attribute ANY(0,0);

// a CG56Memory represents the X and Y memories of a 56000.  It is
// derived from DualMemory.
CG56Memory :: CG56Memory(const char* x_map, const char* y_map) :
	DualMemory("x",A_XMEM,ANY,x_map,"y",A_YMEM,ANY,y_map)
{}

CG56Target :: CG56Target(const char* nam, const char* desc) :
	inProgSection(0), uname(0),
	AsmTarget(nam,desc,"CG56Star")
{
	initStates();
	mem = 0;
}

void CG56Target :: initStates() {
	addState(xMemMap.setState("xMemMap",this,"0-4095","X memory map",
		A_NONSETTABLE|A_NONCONSTANT));
	addState(yMemMap.setState("yMemMap",this,"0-4095","Y memory map",
		A_NONSETTABLE|A_NONCONSTANT));
	addState(disCode.setState("Display code?",this,"YES",
	                          "display code if YES."));
	addState(dirName.setState("dirName",this,"~/DSPcode",
				  "directory for all output files"));
}

void CG56Target :: addCode(const char* code) {
	if (code[0] == '!')
		cmds += (code + 1);
	else CGTarget::addCode(code);
}

char* makeLower(const char* name) {
	LOG_NEW; char* newp = new char[strlen(name)+1];
	char *o = newp;
	while (*name) {
		char c = *name++;
		if (isupper(c)) *o++ = tolower(c);
		else *o++ = c;
	}
	*o = 0;
	return newp;
}

int CG56Target :: setup(Galaxy& g) {
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(dirName);
	cmds.initialize();
	LOG_DEL; delete mem;
	LOG_NEW; mem = new CG56Memory(xMemMap,yMemMap);
	if (!AsmTarget::setup(g)) return FALSE;
	uname = makeLower(g.readName());
	targetNestedSymbol.initialize();
	targetNestedSymbol.setTarget(this);
	return TRUE;
}

void CG56Target :: headerCode () {
        StringList code = "generated code for target ";
        code += readFullName();
	outputComment (code);
	AsmTarget::headerCode();
	const char* path = expandPathName("~ptolemy/lib/cg56");
	StringList inc = "\tinclude '";
	inc += path;
	inc += "/intequlc.asm'\n\tinclude '";
	inc += path;
	inc += "/ioequlc.asm'\n";
	addCode(inc);
}

void CG56Target :: wrapup () {
	StringList map = mem->printMemMap(";","");
	addCode (map);
// put the stuff into the files.
	if (!genFile(myCode, uname, ".asm")) return;
	if (int(disCode)) CGTarget::wrapup();
}

CG56Target :: ~CG56Target () {
	LOG_DEL; delete mem;
//	LOG_DEL; delete dirFullName; dirFullName = 0;
//	LOG_DEL; delete uname;
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
	return out;
}

StringList CG56Target::endIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)	{	// iterate infinitely
		out = "\tjmp\t";
		out += targetNestedSymbol.pop();
		out += "\n";
	}
	else {				// iterate finitely
		out = targetNestedSymbol.pop();
		out += "\n";
	}
	return out;
}

void CG56Target::codeSection() {
	if (!inProgSection) {
		addCode("\torg p:\n");
		inProgSection = 1;
	}
}

void CG56Target::disableInterrupts() {
	codeSection();
	addCode("	ori	#03,mr	;disable interrupts\n");
}

void CG56Target::enableInterrupts() {
	codeSection();
	addCode("	andi	#00,mr	;enable interrupts\n");
}

void CG56Target::saveProgramCounter() {
	codeSection();
	StringList spc;
	spc = targetNestedSymbol.push("SAVEPC");
	spc += "\tequ	*";
	addCode(spc);
}

void CG56Target::restoreProgramCounter() {
	codeSection();
	StringList spc;
	spc = targetNestedSymbol.pop();
	spc += "\tequ	*";
	addCode(spc);
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


