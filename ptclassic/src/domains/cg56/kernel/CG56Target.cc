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
#include "CGUtilities.h"
#include "miscFuncs.h"

const Attribute ANY = {0,0};

// a CG56Memory represents the X and Y memories of a 56000.  It is
// derived from DualMemory.
CG56Memory :: CG56Memory(const char* x_map, const char* y_map) :
	DualMemory("x",A_XMEM,ANY,x_map,"y",A_YMEM,ANY,y_map)
{}

CG56Target :: CG56Target(const char* nam, const char* desc) :
	AsmTarget(nam,desc,"CG56Star")
{
	initStates();
}

void CG56Target :: initStates() {
	inProgSection = 0;
	uname = 0;
	mem = 0;
	StringList hostPrompt = readClassName();
	StringList hostDescription = "Host on which ";
	hostPrompt += " host?";
	hostDescription += readClassName();
	hostDescription += " hardware is installed";
	StringList runPrompt = "Execute on ";
	StringList runDescription = "Download and run on ";
	runPrompt += readClassName();
	runPrompt += "?";
	runDescription += readClassName();
	addState(xMemMap.setState("xMemMap",this,"0-4095","X memory map"));
	addState(yMemMap.setState("yMemMap",this,"0-4095","Y memory map"));
	addState(disCode.setState("Display code?",this,"YES",
	                          "display code if YES."));
	addState(runCode.setState(savestring(runPrompt),this,"NO",
		savestring(runDescription), A_NONSETTABLE|A_NONCONSTANT));
	addState(targetHost.setState(savestring(hostPrompt),this, 
		"localhost", savestring(hostDescription), 
		A_NONSETTABLE|A_NONCONSTANT));
	// change default value of destDirectory
	destDirectory.setValue("~/DSPcode");
}

void CG56Target :: addCode(const char* code) {
	if (code[0] == downloadCmdFlag())
		downloadCmds += (code + 1);
	else if (code[0] == miscCmdFlag())
		miscCmds += (code + 1);
	else CGTarget::addCode(code);
}

int CG56Target :: setup(Galaxy& g) {
	assembleCmds.initialize();
	downloadCmds.initialize();
	miscCmds.initialize();
	LOG_DEL; delete mem; mem = 0;
	LOG_DEL; delete uname; uname = 0;
	LOG_NEW; mem = new CG56Memory(xMemMap,yMemMap);
	uname = makeLower(g.readName());
	targetNestedSymbol.initialize();
	targetNestedSymbol.setTarget(this);
	initializeCmds();
	return AsmTarget::setup(g);
}

void CG56Target :: initializeCmds() {
	assembleCmds = downloadCmds = "#/bin/sh\n";
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
	Target::wrapup();
	if (int(disCode))
		if (!genDisFile(myCode,uname,asmSuffix())) return;
	else
		if (!genFile(myCode,uname,asmSuffix())) return;
	if (int(runCode))
		if (assembleCode()) downloadCode();
}

int CG56Target :: assembleCode() {
	if (!genFile(assembleCmds,uname,".mk","u+x")) return FALSE;
	if (rshSystem("localhost",fileName(uname,".mk"),dirFullName) != 0 ) {
		StringList asmError ="Errors in assembly of ";
		asmError += uname; 
		asmError += " universe";
		Error::abortRun(asmError);
		return FALSE;
	}
	return TRUE;
}

int CG56Target :: downloadCode() {
	if (!genFile(downloadCmds,uname,NULL,"u+x")) return FALSE;
	if (rshSystem(targetHost,uname,dirFullName) != 0 ) {
		StringList dError = "Errors in loading ";
		dError += uname;
		dError += " universe on ";
		dError += readClassName();
		Error::abortRun (dError);
		return FALSE;
	}
	return TRUE;
}

CG56Target :: ~CG56Target () {
	LOG_DEL; delete mem; mem = 0;
	LOG_DEL; delete uname; uname = 0;
}

// copy constructor
CG56Target :: CG56Target (const CG56Target& src) :
AsmTarget(src.readName(),src.readDescriptor(),"CG56Star")
{
	initStates();
	copyStates(src);
}

// clone
Block* CG56Target :: clone () const {
	LOG_NEW; return new CG56Target(*this);
}

void CG56Target::beginIteration(int repetitions, int) {
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

void CG56Target::endIteration(int repetitions, int) {
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
	addCode(out);
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
	addCode("	andi	#$fc,mr	;enable interrupts\n");
}

void CG56Target::saveProgramCounter() {
	codeSection();
	StringList spc;
	spc = targetNestedSymbol.push("SAVEPC");
	spc += "\tequ	*\n";
	addCode(spc);
}

void CG56Target::restoreProgramCounter() {
	codeSection();
	StringList spc;
	spc = "\torg	p:";
	spc += targetNestedSymbol.pop();
	spc += "\n";
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


