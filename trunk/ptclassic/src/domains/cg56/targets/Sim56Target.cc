static const char file_id[] = "Sim56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Target for Motorola 56000 assembly code generation that runs its
 output on the simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Sim56Target.h"
#include "UserOutput.h"
#include "CG56Star.h"
#include "KnownTarget.h"
#include <ctype.h>

Sim56Target :: Sim56Target(const char* nam, const char* desc) :
	CG56Target(nam,desc), uname(0)
{
	initStates();
}

void Sim56Target :: initStates() {
	addState(dirName.setState("dirName",this,"~/DSPcode",
				  "directory for all output files"));
	addState(plotFile.setState("plotFile",this,"",
				   "file to plot with xgraph after run"));
	addState(plotTitle.setState("plotTitle",this,"Simulator output",
				    "graph title (if any)"));
	addState(plotOptions.setState("plotOptions",this,"",
				      "xgraph options"));
	addState(disCode.setState("Display code?",this,"YES",
	                          "display code if YES."));
	addState(simCode.setState("Simulate code?",this,"YES",
	                          "display code if YES."));
}

Sim56Target :: ~Sim56Target() {
	LOG_DEL; delete dirFullName; dirFullName = 0;
	LOG_DEL; delete uname;
}

Sim56Target::Sim56Target(const Sim56Target& arg) :
	CG56Target(arg)
{
	initStates();
	copyStates(arg);
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

int Sim56Target :: setup (Galaxy& g) {
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(dirName);
	cmds.initialize();
	if (!CG56Target::setup(g)) return FALSE;
	StringList lod = "!load ";
	uname = makeLower(g.readName());
	lod += uname;
	lod += "\n";
	addCode(lod);
	return TRUE;
}

void Sim56Target :: headerCode () {
	CG56Target :: headerCode();
	const char* path = expandPathName("~ptolemy/lib/cg56");
	StringList inc = "\tinclude '";
	inc += path;
	inc += "/intequlc.asm'\n\tinclude '";
	inc += path;
	inc += "/ioequlc.asm'\n";
	addCode(inc);
	addCode(
		"	org	p:$48\n"
		"START\n"
		"	movep	#$0000,x:m_bcr\n\n");
};

Block* Sim56Target::clone() const {
	LOG_NEW; return new Sim56Target(*this);
}

void Sim56Target :: addCode(const char* code) {
	if (code[0] == '!')
		cmds += (code + 1);
	else CGTarget::addCode(code);
}

void Sim56Target :: wrapup () {
	addCode (
		 "	jmp	$ff0\n"
		 "ERROR	jmp	$ff0\n"
		 "	org	p:$ff0\n"
		 "	nop\n"
		 "	stop\n");
	addCode ("!break pc>=$ff0\n"
		 "go $48\n");
	inProgSection = TRUE;
	StringList map = mem->printMemMap(";","");
	addCode (map);
	if (int(disCode))
		CGTarget::wrapup();
// put the stuff into the files.
	if (!genFile(myCode, uname, ".asm")) return;
	if (!genFile(cmds, uname,".cmd")) return;
// directive to change to the working directory
	StringList cd = "cd "; cd += dirFullName; cd += ";";
// execute the assembler
	if (int(simCode)) {
		StringList ass = cd;
		ass += "asm56000 -A -B -L ";
		ass += uname;
		if (system(ass) != 0) {
			StringList listing = (const char*)dirName;
			listing += "/";
			listing += uname;
			listing += ".list";
			Error::abortRun(
			"Errors in assembly: see assembler listing in file ",
					listing, " for errors");
			return;
		}
// execute the simulator
		StringList ex = cd;
		ex += "xterm -e sim56000 ";
		ex += uname;
		ex += ".cmd";
		system (ex);
// now plot the result if requested
		const char* file = plotFile;
		if (*file == 0) return;
		StringList plot = cd;
		plot += "awk '{print ++n, $1}' ";
		plot += file;
		plot += " | xgraph -t '";
		plot += (const char*)plotTitle;
		plot += "' ";
		plot += (const char*)plotOptions;
		plot += "&";
		system (plot);
	}
}

ISA_FUNC(Sim56Target,CG56Target);

// make an instance
static Sim56Target proto("sim-CG56","run code on the 56000 simulator");

static KnownTarget entry(proto, "sim-CG56");

