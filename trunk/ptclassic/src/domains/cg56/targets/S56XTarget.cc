static const char file_id[] = "S56XTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

 Target for Ariel S-56X DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "S56XTarget.h"
#include "UserOutput.h"
#include "CG56Star.h"
#include "KnownTarget.h"

S56XTarget :: S56XTarget(const char* nam, const char* desc) :
	CG56Target(nam,desc)
{
	initStates();
}

S56XTarget::S56XTarget(const S56XTarget& arg) :
	CG56Target(arg)
{
	initStates();
	copyStates(arg);
}

void S56XTarget :: initStates() {
/*	xMemMap.setValue("0-255 8192-16383");
	yMemMap.setValue("0-16383");
	xMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	yMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT); */
	xMemMap.setState("xMemMap",this,"0-255 8192-16383","X memory map");
	yMemMap.setState("yMemMap",this,"0-16383","Y memory map");
	addState(runCode.setState("Download/Run code?",this,"YES",
	                          "display code if YES."));
}

void S56XTarget :: headerCode () {
	CG56Target :: headerCode();
	addCode(
  		"; Breakpoint/trace interrupt handlers\n"
		"	org	p:i_swi\n"
		"	jsr	SWI_INTR\n"
		"	org	p:i_trace\n"
		"	jsr	SWI_INTR\n\n"
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
		"; Host port flow control interrupt handlers\n"
		"	org	p:i_hstcm+2		; Host command 1\n"
		"STARTR	bset	#0,x:m_pbddr		; Allow DSP writes to host port\n"
		"	nop\n"
		"	org	p:i_hstcm+4		; Host command 2\n"
		"STOPR	bclr	#0,x:m_pbddr		; Disallow DSP writes to host port\n"
		"	nop\n"
		"	org	p:i_hstcm+6		; Host command 3\n"
		"STARTW	bset	#1,x:m_pbddr		; Allow DSP reads from host port\n"
		"	nop\n"
		"	org	p:i_hstcm+8		; Host command 4\n"
		"STOPW	bclr	#1,x:m_pbddr		; Disallow DSP reads from host port\n"
		"	nop\n"
		"	org	p:$90\n"
		"START\n"
		"; Set device driver DMA ready flag\n"
		"	bset	#m_hf2,x:m_hcr\n"
		"	bclr	#0,x:m_pbddr		; clear read DMA flow control bit\n"
		"	bclr	#1,x:m_pbddr		; clear write DMA flow control bit\n\n");
		interruptFlag = TRUE;
};

Block* S56XTarget::clone() const {
	LOG_NEW; return new S56XTarget(*this);
}

void S56XTarget :: addCode(const char* code) {
	if (code[0] == '@')
		aio += (code + 1);
	else CG56Target::addCode(code);
}
void S56XTarget :: wrapup () {
	addCode (
		 "; Error handler for s56x architecture: ignore interrupts, loop\n"
		 "ERROR	ori	#03,mr\n"
		 "WAIT	jmp	WAIT\n"
		 "	org	p:$1ff0\n"
		 "	nop\n"
		 "	stop\n");
	inProgSection = TRUE;
	CG56Target::wrapup();
// put the stuff into the files.
	if (!genFile(aio, uname,".aio")) return;
// directive to change to the working directory
	StringList cd = "cd "; cd += dirFullName; cd += ";";
// execute the assembler
	if (int(runCode)) {
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

ISA_FUNC(S56XTarget,CG56Target);

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");

