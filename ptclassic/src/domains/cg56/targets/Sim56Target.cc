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
	runCode.setValue("YES");
	runCode.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

void Sim56Target::initializeCmds() {
	CG56Target::initializeCmds();
	assembleCmds += "asm56000 -A -B -L ";
	assembleCmds += uname;
	assembleCmds += "\n";
	miscCmds += "load ";
	miscCmds += uname;
	miscCmds += "\n";
	downloadCmds += "xterm -e sim56000 ";
	downloadCmds += fileName(uname,".cmd\n");
	const char* file = plotFile;
	if (*file != 0) {
		downloadCmds += "awk '{print ++n, $1}' ";
		downloadCmds += file;
		downloadCmds += " | xgraph -t '";
		downloadCmds += (const char*)plotTitle;
		downloadCmds += "' ";
		downloadCmds += (const char*)plotOptions;
		downloadCmds += "&\n";
	}
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
	miscCmds += "break pc>=$ff0\n";
	miscCmds += "go $48\n";
	inProgSection = TRUE;
	if (!genFile(miscCmds, uname,".cmd")) return;
	CG56Target::wrapup();
}

ISA_FUNC(Sim56Target,CG56Target);

// make an instance
static Sim56Target proto("sim-CG56","run code on the 56000 simulator");

static KnownTarget entry(proto, "sim-CG56");

