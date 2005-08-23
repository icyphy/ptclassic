static const char file_id[] = "DSKC50Target.cc";
/******************************************************************
Version identification:
@(#)DSKC50Target.cc	1.11	8/19/96

@Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: A. Baensch, Luis Gutierrez
 Date of creation: 30 June 1995

 Target for TI TMS320C5x DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DSKC50Target.h"
#include "KnownTarget.h"
#include "TITarget.h"

DSKC50Target :: DSKC50Target(const char* nam, const char* desc,
			     const char* assocDomain) :
C50Target(nam,desc,assocDomain),
TITarget(nam,desc,"C50Star", assocDomain) {
	initStates();
	addStream("shellCmds",&shellCmds);
}

DSKC50Target::DSKC50Target(const DSKC50Target& arg) :
C50Target(arg),
TITarget(arg) {
	initStates();
	copyStates(arg);
	addStream("shellCmds",&shellCmds);
}

void DSKC50Target :: initStates() {

	// ROM tables: Block B1 512 words and User Data (u) 8704 words
	// Values for the following C50/AIC registers used by the AIC
	// init routine will be stored in the following order(starting
	// at address 1271):
	// TCR, PRD, TA, RA, TA', RA', TB, RB, AIC control reg.

	bMemMap.setInitValue("768-1270");
	uMemMap.setInitValue("2432-6847");

	// the u and b memory maps should be settable since
	// u/b memory are the same as far as teh assembler is
	// concerned so if there's not enough room in one
	// the program can use the other

	bMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	uMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	compileFlag.setInitValue("YES");
	compileFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);

}

void DSKC50Target :: headerCode () {

	// initialize code streams
	shellCmds.initialize();

	myCode	<< "\t.ds	1271\n"
		<< "TCR		.word	20h\n"
		<< "PDR		.word	6h\n"
		<< "TA		.word	5\n"
		<< "RA		.word	5\n"
		<< "TB		.word	36\n"
		<< "RB		.word	36\n"
		<< "TAP		.word	10\n"
		<< "RAP		.word	10\n"
		<< "AIC_CTR	.word	17\n"
	 	<< "\t.ps	00808h\n"
	   	<< "\tB	TINT		; set timer interrupt vector\n"
  	   	<< "\tB	RINT		; set receive interrupt vector\n"
	   	<< "\tB	XINT		; set transmit interrupt vector\n";
	C50Target :: headerCode();
	interruptFlag = TRUE;
	myCode  << "\tcall	SETUPX\n"
		<< "\tcall	SETUPR\n"
		<< "\tcall	AIC_INIT\n";
}

Block* DSKC50Target::makeNew() const {
	LOG_NEW; return new DSKC50Target(*this);
}


// the following code is needed to initialize the AIC

static const char AICInitCodeString[] =
"AIC_INIT:\n"
"	 LDP     #0h\n"
"	 LAMM    IMR\n"
"	 SACB\n"
"	 SPLK    #22h,IMR\n"
"	 SPLK    #20h,TCR \n"     
"	 SPLK    #06h,PRD \n"    
"	 MAR     *,AR0\n"
"	 LACC    #0008h   \n"    
"	 SACL    SPC    \n"      
"	 LACC    #00c8h  \n"     
"	 SACL    SPC\n"
"	 LACC    #080h   \n"     
"	 SACH    DXR\n"
"	 SACL    GREG\n"
"	 LAR     AR0,#0FFFFh\n"
"	 RPT     #10000    \n"   
"	 LACC    *,0,AR0  \n"    
"	 SACH    GREG\n"
"	 ;------------------------\n"
"	 LDP     #TA          \n"
"	 SETC    SXM    \n"      
"	 LACC    TA,9   \n"      
"	 ADD     RA,2   \n"      
"	 CALL    AIC_2ND \n"     
"	 ;------------------------\n"
"	 LDP     #TB \n"
"	 LACC    TB,9    \n"      
"	 ADD     RB,2  \n"        
"	 ADD     #02h  \n"        
"	 CALL    AIC_2ND  \n"     
"	 ;------------------------ \n"
"	 LDP     #TAP \n"
"	 LACC    TAP,9 \n"
"	 ADD     RAP,2 \n"
"	 ADD     #01h \n"
"	 CALL    AIC_2ND \n"
"	 ;------------------------- \n"
"	 LDP     #AIC_CTR \n"
"	 LACC    AIC_CTR,2 \n"    
"	 ADD     #03h    \n"      
"	 CALL    AIC_2ND \n"      
"	 LACB \n"
"	 SAMM   IMR \n"
"	 NOP \n"
"	 NOP \n"
"	 RET      \n"             
" \n"
"AIC_2ND: \n"
"	 LDP    #0h \n"
"	 SACH    DXR  \n"         
"	 CLRC    INTM \n"
"	 IDLE \n"
"	 ADD     #6h,15  \n"      
"	 SACH    DXR   \n"        
"	 IDLE \n"
"	 SACL    DXR  \n" 
"	 IDLE \n"
"	 LACL    #0  \n"  
"	 SACL    DXR  \n" 
"	 IDLE \n"
"	 SETC    INTM \n"
"	 RET \n";



void DSKC50Target :: trailerCode () {
	C50Target::trailerCode();
}

int DSKC50Target :: compileCode() {

 	StringList assembleCmds;
  	assembleCmds << "asmc50 " << filePrefix << "DSK.asm";
  	if (systemCall(assembleCmds,"Errors in assembly")!=0)
  		return FALSE;
	return TRUE;
}

// generate the C50 assembler .asm file and truncate its lines to 80
// characters due to limitations in the DSK assembler
void DSKC50Target :: writeCode() {

	C50Target :: writeCode();

	// DSK assembler doesn't seem to like lines longer than 80 characters
	// so make a new file that's the original file with each line truncated
	// to 80 characters.
	StringList postProcessCmd;
	postProcessCmd.initialize();
	postProcessCmd << "cut -c 1-79 " << filePrefix<< ".asm > "
		       << filePrefix << "DSK.asm";
	systemCall(postProcessCmd,
		   "Post-processing of the C50 assembler file failed");

}

int DSKC50Target :: runCode() {
	StringList runCmd;
	runCmd << "loadc50 " << filePrefix << "DSK.dsk";
	return ( systemCall(runCmd,
			    "Problems running code onto TMS320C5x",
			    targetHost) == 0 );
}

void DSKC50Target::frameCode() {
            
        aicFlag = testFlag("aicFlag");
	xmitFlag = testFlag("xmitFlag");
	rcvFlag = testFlag("rcvFlag");
	timerFlag = testFlag("timerFlag");

	if ((aicFlag == TRUE) || (xmitFlag == TRUE) || (rcvFlag == TRUE)) {
		*defaultStream << AICInitCodeString;
	} else {
	        *defaultStream << "AIC_INIT:\n\tret\n";
	}
	if (xmitFlag == FALSE) {
		// generate dummy transmit interrupt handling procedure
		// and dummy SETUPX procedure
		*defaultStream << "XINT:\n\trete\nSETUPX:\n\tret\n";
	} 
	if (rcvFlag == FALSE) {
		// generate dummy receive interrupt handling procedure
		// and dummy SETUPR procedure
		*defaultStream << "RINT:\n\trete\nSETUPR:\n\tret\n";
	};
	if (timerFlag == FALSE) {
		// generate dummy timer interrupt handling procedure
		*defaultStream << "TINT:\n\trete\n";
	}
	
	TITarget::frameCode();	
}

ISA_FUNC(DSKC50Target, C50Target);

// Register an instance
static DSKC50Target proto("DSKC50", "run code on the DSK TMS320C5x card");

static KnownTarget entry(proto, "DSKC50");










