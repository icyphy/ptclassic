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
#include <sys/file.h>

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
	addState(plotFile.setState("Plot file name",this,"outfile.sim",
		"file to plot with xgraph after run"));
	addState(plotTitle.setState("Plot title",this,"Simulator output",
		"graph title (if any)"));
	addState(plotOptions.setState("Plot options",this,"",
		"xgraph options"));
	addState(interactiveFlag.setState(
		"Interactive Simulation",this,"YES",""));
	runFlag.setValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

int Sim56Target::compileTarget() {
	StringList assembleCmds = "asm56000 -A -b -l ";
	assembleCmds += uname;
	return systemCall(assembleCmds,"Errors in assembly");
}

int Sim56Target::runTarget() {
	const char* file = plotFile;
	if (*file != 0) unlink(fullFileName((char *)file));
	StringList cmdFile = "load ";
	cmdFile += fileName(uname,".lod\n");
	cmdFile += miscCmds;
	cmdFile += "break pc>=$ff0\n";
	cmdFile += "go $48\n";
	if (!interactiveFlag) cmdFile += "quit\n";
	if (!genFile(cmdFile, uname,".cmd")) return FALSE;
	StringList downloadCmds;
	if (interactiveFlag) 
		downloadCmds = "(xterm -e sim56000 ";
	else 
		downloadCmds = "(sim56000 ";
	downloadCmds += fileName(uname,".cmd");
	downloadCmds += " > /dev/null)";
	if (systemCall(downloadCmds,"Errors in starting up the simulator")) 
		return FALSE;
	if (*file != 0 && access(fullFileName((char*)file),F_OK)==0) {
		StringList plotCmds;
 		plotCmds = "awk '{print ++n, $1}' ";
		plotCmds += file;
		plotCmds += " | xgraph -t '";
		plotCmds += (const char*)plotTitle;
		plotCmds += "' ";
		plotCmds += (const char*)plotOptions;
		plotCmds += "&\n";
		return systemCall(plotCmds,"Plot unsuccessful");
	}
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
	inProgSection = TRUE;
	CG56Target::wrapup();
}

ISA_FUNC(Sim56Target,CG56Target);

// make an instance
static Sim56Target proto("sim-CG56","run code on the 56000 simulator");

static KnownTarget entry(proto, "sim-CG56");
