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

Sim56Target :: Sim56Target(const char* nam, const char* desc) :
	CG56Target(nam,desc)
{
	initStates();
}

Sim56Target::Sim56Target(const Sim56Target& arg) :
	CG56Target(arg)
{
	initStates();
	copyStates(arg);
}

void Sim56Target :: initStates() {
	addState(plotFile.setState("plotFile",this,"",
				   "file to plot with xgraph after run"));
	addState(plotTitle.setState("plotTitle",this,"Simulator output",
				    "graph title (if any)"));
	addState(plotOptions.setState("plotOptions",this,"",
				      "xgraph options"));
	addState(simCode.setState("Simulate code?",this,"YES",
	                          "display code if YES."));
}

int Sim56Target :: setup (Galaxy& g) {
	if (!CG56Target::setup(g)) return FALSE;
	StringList lod = "!load ";
	lod += uname;
	lod += "\n";
	addCode(lod);
	return TRUE;
}

void Sim56Target :: headerCode () {
	CG56Target :: headerCode();
	addCode(
		"	org	p:$48\n"
		"START\n"
		"	movep	#$0000,x:m_bcr\n\n");
};

Block* Sim56Target::clone() const {
	LOG_NEW; return new Sim56Target(*this);
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
	CG56Target::wrapup();
// put the stuff into the files.
	if (!genFile(cmds, uname,".cmd")) return;
// directive to change to the working directory
	StringList cd = "cd "; cd += dirFullName; cd += ";";
// execute the assembler
	if (int(simCode)) {
		StringList assem = cd;
		assem += "asm56000 -A -B -L ";
		assem += uname;
		if (system(assem) != 0) {
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

