static const char file_id[] = "S56XTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino
	Modified code from S56XTarget.cc
	Ported from gabriel s56x.l

 Target for Ariel S-56X DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "S56XTarget.h"
#include "UserOutput.h"
#include "CG56Star.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include <ctype.h>

S56XTarget :: S56XTarget(const char* nam, const char* desc) :
	CG56Target(nam,desc)
{
	initStates();
}

S56XTarget::S56XTarget(const S56XTarget& arg) :
	Sim56Target(arg)
{
	initStates();
	copyStates(arg);
}

void S56XTarget :: initStates() {
	xMemMap.setValue("0-255 8192-16383");
	yMemMap.setValue("0-16383");
	xMemMap.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
	yMemMap.setAttributes(A_NONCONSTANT|A_NONSETTABLE); 
	addState(dirName.setState("dirName",this,"~/DSPcode",
				  "directory for all output files"));
	addState(disCode.setState("Display code?",this,"YES",
	                          "display code if YES."));
	addState(simCode.setState("Simulate code?",this,"YES",
	                          "display code if YES."));
}

int S56XTarget :: setup (Galaxy& g) {
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(dirName);
	cmds.initialize();
	if (!CG56Target::setup(g)) return FALSE;
	uname = makeLower(g.readName());
	return TRUE;
}

void S56XTarget :: headerCode () {
	CG56Target :: headerCode();
	const char* path = expandPathName("~ptolemy/lib/cg56");
	StringList inc = "\tinclude '";
	inc += path;
	inc += "/intequlc.asm'\n\tinclude '";
	inc += path;
	inc += "/ioequlc.asm'\n";
	addCode(inc);
	addCode(
		"	org	p:$90\n"
		"START\n");
	addCode(
  		"; Breakpoint/trace interrupt handlers\n"
		"	org	p:\n"
		"l64	equ	*			; Save current P: address\n"
		"	org	p:i_swi\n"
		"	jsr	SWI_INTR\n"
		"	org	p:l64			; Restore old P: address\n\n"
		"	org	p:\n"
		"l65	equ	*			; Save current P: address\n"
		"	org	p:i_trace\n"
		"	jsr	SWI_INTR\n"
		"	org	p:l65			; Restore old P: address\n\n"
		"	org	p:\n"
		"l24	equ	*			; Save current P: address\n"
		"; Qckmon/degmon monitor code\n\n"
		"	org	p:$64\n"
		"de_pc	dc	0			; saved PC\n"
		"de_sr	dc	0			; saved SR\n"
		"de_ipr	dc	0			; saved IPR\n"
		"de_foo	dc	0			; temporary junk\n\n"
		"SWI_INTR\n"
		"	movem	ssl,p:de_sr		; save SR\n"
		"	movem	ssh,p:de_pc		; save PC (pops stack)\n"
		"	movep	x:m_ipr,p:de_ipr	; save IPR\n"
		"	andi	#$fc,mr			; allow interrupts\n"
		"	bclr	#2,x:m_pbddr		; clear allow-us-to-run flag\n"
		"	bset	#3,x:m_pbddr		; indicate processor stopped\n"
		"	bset	#m_hf3,x:m_hcr		; allow SIGUSRs to get to user\n"
		"	move	x0,p:de_foo\n"
		"	move	p:$8000,x0		; interrupt host\n"
		"	move	p:de_foo,x0\n"
		"SWI_LOOP				; wait for host to let\n"
		"	jclr	#2,x:m_pbddr,SWI_LOOP	; us continue\n"
		"	bclr	#3,x:m_pbddr		; indicate we're continuing\n"
		"	movem	p:de_pc,ssh		; restore SR, PC, IPR\n"
		"	movem	p:de_sr,ssl\n"
		"	movep	p:de_ipr,x:m_ipr\n"
		"; XXX stuff for de2 goes here\n"
		"	nop\n"
		"	rti				; continue\n\n"
		"	org	p:l24			; Restore old P: address\n"
		"; Allow host command interrupts\n"
		"	andi	#$fc,mr\n"
		"; Set device driver DMA ready flag\n"
		"	bset	#m_hf2,x:m_hcr\n"
		"	bclr	#0,x:m_pbddr		; clear read DMA flow control bit\n"
		"	bclr	#1,x:m_pbddr		; clear write DMA flow control bit\n\n"
		"; Host port flow control interrupt handlers\n"
		"	org	p:\n"
		"l25	equ	*			; Save current P: address\n"
		"	org	p:i_hstcm+2		; Host command 1\n"
		"STARTR	bset	#0,x:m_pbddr		; Allow DSP writes to host port\n"
		"	nop\n"
		"	org	p:l25			; Restore old P: address\n\n"
		"	org	p:\n"
		"l26	equ	*			; Save current P: address\n"
		"	org	p:i_hstcm+4		; Host command 2\n"
		"STOPR	bclr	#0,x:m_pbddr		; Disallow DSP writes to host port\n"
		"	nop\n"
		"	org	p:l26			; Restore old P: address\n\n"
		"	org	p:\n"
		"l27	equ	*			; Save current P: address\n"
		"	org	p:i_hstcm+6		; Host command 3\n"
		"STARTW	bset	#1,x:m_pbddr		; Allow DSP reads from host port\n"
		"	nop\n"
		"	org	p:l27			; Restore old P: address\n\n"
		"	org	p:\n"
		"l28	equ	*			; Save current P: address\n"
		"	org	p:i_hstcm+8		; Host command 4\n"
		"STOPW	bclr	#1,x:m_pbddr		; Disallow DSP reads from host port\n"
		"	nop\n"
		"	org	p:l28			; Restore old P: address\n"); 
};

Block* S56XTarget::clone() const {
	LOG_NEW; return new S56XTarget(*this);
}

void S56XTarget :: addCode(const char* code) {
	if (code[0] == '!')
		cmds += (code + 1);
	else CGTarget::addCode(code);
}
void S56XTarget :: wrapup () {
	addCode (
		 "; Error handler for s56x architecture: ignore interrupts, loop\n"
		 "ERROR	ori	#03,mr\n"
		 "l32	jmp	l32\n"
		 "	org	p:$1ff0\n"
		 "	nop\n"
		 "	stop\n");
	inProgSection = TRUE;
	StringList map = mem->printMemMap(";","");
	addCode (map);
	if (int(disCode))
		CGTarget::wrapup();
// put the stuff into the files.
	if (!genFile(myCode, uname, ".asm")) return;
//	if (!genFile(cmds, uname,".cmd")) return;
// directive to change to the working directory
	StringList cd = "cd "; cd += dirFullName; cd += ";";
// execute the assembler
	if (int(simCode)) {
		StringList assem = cd;
		assem += "asm56000 -b -l -A -oso ";
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
// load & run on 56x
		StringList ex = cd;
		ex += "load_s56x ";
		ex += uname;
		ex += ".lod";
		if(system (ex) != 0) {
			Error::abortRun("Errors in loading to s56x");
			return;
		}
		system(cmds);
	}
}

ISA_FUNC(S56XTarget,Sim56Target);

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");

