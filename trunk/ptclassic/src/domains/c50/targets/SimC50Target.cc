static const char file_id[] = "SimC50Target.cc";
/******************************************************************
Version identification:
@(#)SimC50Target.cc	1.4	05/28/98

@Copyright (c) 1998 The Regents of the University of California.
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

 Programmer: Guner Arslan 
 Date of creation: 2 May 1998

 Target for TI TMS320C5x Simulator.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include "SimC50Target.h"
#include "KnownTarget.h"
#include "TITarget.h"
#include "CGUtilities.h"

SimC50Target :: SimC50Target(const char* nam, const char* desc,
			     const char* assocDomain) :
C50Target(nam,desc,assocDomain), 
TITarget(nam,desc,"C50Star", assocDomain) {
	initStates();
	addStream("shellCmds",&shellCmds);
}

SimC50Target::SimC50Target(const SimC50Target& arg) :
C50Target(arg),
TITarget(arg) {
	initStates();
	copyStates(arg);
	addStream("shellCmds",&shellCmds);
}

void SimC50Target :: initStates() {

        //uMem is on-chip Program Memory RAM
        //bMem is on-chip Data Memory RAM B1 

	bMemMap.setInitValue("768-1270");
	uMemMap.setInitValue("2048-11263");
	bMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	uMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	compileFlag.setInitValue("YES");
	compileFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	displayFlag.setInitValue("YES");
	displayFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);

	// Make reportExecutionTime visible
	reportExecutionTime.setAttributes(A_SETTABLE);


	// Make reportMemoryUsage visible
	reportMemoryUsage.setAttributes(A_SETTABLE);

}

void SimC50Target :: headerCode () {

	// initialize code streams
	shellCmds.initialize();
	TITarget :: headerCode ();

	myCode	<< "\n\t.mmregs \t\t; set memory mapped registers\n"
		<< "\t.sect \"vectors\"\t\t; interrupt vector address\n\n"
	       	<< "RESET:\tb \tINIT \t\t; loaded in PM address 0h\n"
		<< "INT0:\tb \tISR0 \t\t; INT0 interrupt vector\n"
                << "INT1:\tb \tISR1 \t\t; INT1 interrupt vector\n"
		<< "INT2:\tb \tISR2 \t\t; INT2 interrupt vector\n"
                << "INT3:\tb \tISR3 \t\t; INT3 interrupt vector\n"
		<< "TINT:\tb \tTIME \t\t; timer interrupt vector\n"
		<< "RINT:\tb \tRCV \t\t; serial port receive interrupt\n"
		<< "XMT:\tb \tXINT \t\t; serial port transmit interrupt\n"
		<< "TRNT:\tb \tTRX \t\t; TDM port receive interrupt\n"
		<< "TXNT:\tb \tTXMT \t\t; TDM port transmit interrupt\n"
		<< "INT4:\tb \tISR4 \t\t; INT4 interrupt vector\n\n"
		<< "\t.space 14*16 \t\t; reserved space\n\n"
		<< "TRAP:\tb \tTRP \t\t; sofware trap instruction\n"
		<< "NMI:\tb \tNMISR \t\t; non-maskable interrupt\n\n"
		<< "\t.text\n\n"
		<< "INIT:\tldp \t#0 \t\t; initialize data pointer\n"
		<< "\topl \t#20h, PMST\t; configure onchip SARAM into DM\n"
		<< "\tclrc \tOVM\t\t; disable overflow saturation mode\n"
		<< "\tlacl\t#0\t\t; load accumulator with 0\n"
		<< "\tsamm\tCWSR\t\t; set wait state control register\n"
		<< "\tsamm\tPDWSR\t\t; to 0 waits for both PM and DM\n";

	interruptFlag = TRUE;

	myCode << "\tcall SETUP \t\t; call setup for additional initializations\n"
	       << "\tcall SETUPR \t\t; call setupx for additional RX inits\n";
}

Block* SimC50Target::makeNew() const {
	LOG_NEW; return new SimC50Target(*this);
}

void SimC50Target :: trailerCode () {
	C50Target::trailerCode();
}

int SimC50Target :: compileCode() {

 	StringList linkFile, assembleCmds, linkCmds ;

  	linkFile     << "MEMORY\n{\n"
		     << "PAGE 0: Int_Vec:	origin = 0x0000, length = 0x0040\n"
		     << "Prog_ROM:	origin = 0x0040, length = 0x07C0\n"
		     << "Prog_RAMB0:	origin = 0xFE00, length = 0x0200\n"
		     << "\nPAGE 1: Regs: origin = 0x0000, length = 0x0060\n"
		     << "Int_DARAMB2:	origin = 0x0060, length = 0x0020\n"
		     << "Int_DARAMB1:	origin = 0x0300, length = 0x0500\n"
		     << "Int_SARAM9K:	origin = 0x0800, length = 0x2400\n}\n"
		     << "SECTIONS\n{\n"
		     << "vectors:	> Int_Vec	PAGE = 0\n"
		     << ".text:		> Prog_ROM	PAGE = 0\n"
		     << ".data:		> Prog_ROM	PAGE = 0\n"
		     << ".sect:		> Int_SARAM9K	PAGE = 1\n}\n"
		     << "-o " << filePrefix << ".out\n"
		     << "-m " << filePrefix << ".map\n"
		     << "-q "
		     <<  filePrefix << ".obj\n";

	writeFile(linkFile, ".cmd");

	assembleCmds << "dspa -v50 -s " <<  filePrefix;

	if (systemCall(assembleCmds,"Assembler error")!=0)
  		return FALSE;

	linkCmds << "dsplnk " << filePrefix << ".cmd";

	if (systemCall(linkCmds,"Linker error")!=0)
	        return FALSE;

	return TRUE;
}

void SimC50Target :: writeCode() {
  C50Target::writeCode();
}

#define MAXLINE 64

int SimC50Target :: runCode() {
    int cycles = 0;
    int deleteFlag = FALSE;

    int progMem = 28;  // int. vectors are alwas there and take 28 words
    int dataMem = 0;
    char buffer[MAXLINE];

    StringList runCmd, simFile, cycleReport, tokenPtr;
 
    StringList fileNameTime, fileNameMem;
    fileNameTime << filePrefix << ".out";
    fileNameMem << filePrefix << ".map";

    StringList  fullFileNameTime = findLocalFileName(targetHost, destDirectory,
				 fileNameTime, deleteFlag);
    StringList  fullFileNameMem = findLocalFileName(targetHost, destDirectory,
				 fileNameMem, deleteFlag);

    simFile << "ma 0x0000, 0, 0x0800, RAM\n"
	    << "ma 0x0800, 0, 0x2400, RAM\n"
	    << "ma 0x2c00, 0, 0x5400, RAM\n"
	    << "ma 0x0004, 1, 0x001f, RAM\n"
	    << "ma 0x0024, 1, 0x0003, RAM\n"
	    << "ma 0x0028, 1, 0x0003, RAM\n"
	    << "ma 0x0030, 1, 0x0006, RAM\n"
	    << "ma 0x0050, 1, 0x0010, RAM\n"
	    << "ma 0x0060, 1, 0x0020, RAM\n"
	    << "ma 0x0300, 1, 0x0200, RAM\n"
	    << "ma 0x0800, 1, 0x2400, RAM\n"
	    << "ma 0x8000, 1, 0x8000, RAM\n"
	    << "\nload " << filePrefix << "\n"
	    << "ba ENDE\n"
	    << "ba RESET\n"
	    << "run\n"
	    << "runb\n"
	    << "dlog " << filePrefix << ".out\n"
	    << "? clk\n"
	    << "dlog close\n"
	    << "quit\n";

    
    writeFile(simFile, ".sim");

    runCmd << "sim5x -t " << filePrefix << ".sim";

    systemCall(runCmd, "Problems running code onto TMS320C5x Simulator", targetHost);

    if (int(reportExecutionTime)){

      FILE* fp = fopen( fullFileNameTime ,"r");

      if (fp){
	while (! feof(fp) && fgets(buffer, MAXLINE, fp) ) {
	  if (buffer[0] == ';') {
	    tokenPtr = strtok(buffer, ";");
	    cycles = atoi(tokenPtr);
	  }
	  else{
	    tokenPtr = strtok(buffer, " ");
	    cycles = atoi(tokenPtr);
	  }
	}
	
	fclose(fp);

	cycleReport << "TMS320C50 implementation costs:\nExecution time: " 
                            << cycles << " cycles \n";
      }	
      else {
	Error::message(*this, "Cannot read log file");
      }
    }

    if (int(reportMemoryUsage)){
      FILE* fp = fopen(fullFileNameMem, "r");
      if (fp){ 
	while (! feof(fp) && fgets(buffer, MAXLINE, fp) ) {
	  buffer[MAXLINE - 1] = 0;		   
	  if (buffer[0] == '.') {
	    tokenPtr = strtok(buffer, " ");
	    for (int ii=1;ii<=3;ii++)  // 4th number is memory length
	      tokenPtr = strtok(NULL, " ");	 
	    if ( strncmp(buffer, ".text", 5) == 0 ) 
	      progMem = progMem + atoi(tokenPtr);
	    else if ( strncmp(buffer, ".data", 5) == 0 ) 
	      progMem = progMem + atoi(tokenPtr);
	    else if ( strncmp(buffer, ".sect", 4) == 0 ) 
	      dataMem = dataMem + atoi(tokenPtr);
	    else if ( strncmp(buffer, ".bss", 4) == 0 ) 
	      dataMem = dataMem + atoi(tokenPtr);
	    else{
 
	      Error::message(*this, "A memory allocation directive other than"
                      ".text .data .sect and .bss is not allowed");
	      return(FALSE); 
	    }
	  }
	  else if(buffer[0] == 'a'){
	    tokenPtr = strtok(buffer, " ");
	    tokenPtr = strtok(NULL, " ");	
	    if ( strncmp(tokenPtr, "0", 1) == 0 ){
	      for (int ii=1;ii<=2;ii++)  // 4th number is memory length
		tokenPtr = strtok(NULL, " ");	 
	      dataMem = dataMem + atoi(tokenPtr);
	    }
	  }
	}
	fclose(fp);
	cycleReport << "Program Memory usage:  " << progMem << " words \n";
	cycleReport << "Data Memory usage:  " << dataMem << " words \n";
      }
      else {
	Error::message(*this, "Cannot read map file");
      }	
      
      if( int(reportMemoryUsage) | int(reportExecutionTime) )
	Error::message(*this, cycleReport);
    }
    return(TRUE);
}
	
//Override TITargets

// We want labels instead of absolute addresses. This is done by adding addr in 
// front of every address !
  
void SimC50Target::orgDirective(const char* memName, unsigned addr) {
  if (strcmp(memName,"u")==0){
        *defaultStream << "addr" << int(addr) << ": \t.data \n";
	inProgSection = 0;
  }
  if (strcmp(memName,"b")==0){
    *defaultStream << "addr" << int(addr) << ":\t.sect \"addr" << int(addr) << "\"\n";
	inProgSection = 0;
  }
}

void SimC50Target::writeFloat(double val) {
	*defaultStream << "\t.word\t" << int(limitFix(val)*(pow(2,15)-1)) << "\n";

}

void SimC50Target::writeFix(double val) {
	*defaultStream << "\t.word\t" << int(limitFix(val)*(pow(2,15)-1)) << "\n";
}


//***************

void SimC50Target::frameCode() {
	xmitFlag = testFlag("xmitFlag");
	rcvFlag = testFlag("rcvFlag");
	timerFlag = testFlag("timerFlag");
// The following flags are not used yet by any star
        int0Flag = testFlag("int0Flag");
        int1Flag = testFlag("int1Flag");
        int2Flag = testFlag("int2Flag");
        int3Flag = testFlag("int3Flag");
        int4Flag = testFlag("int4Flag");
        TDMRFlag = testFlag("TDMRFlag");
        TDMTFlag = testFlag("TDMTFlag");
        TRAPFlag = testFlag("TRAPFlag");
        NMIFlag  = testFlag("NMIFlag");


	if (xmitFlag == FALSE) {
		// generate dummy interrupt handling procedure
		*defaultStream << "XINT:\n\trete\nSETUPX:\n\tret\n";
	} 
	if (rcvFlag == FALSE) {
		// generate dummy receive interrupt handling procedure
		// and dummy SETUPR procedure
		*defaultStream << "RCV:\n\trete\nSETUPR:\n\tret\n";
	}
	if (timerFlag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "TIME:\n\trete\n";
	}
	
        if (int0Flag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "ISR0:\n\trete\n";
	}

        if (int1Flag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "ISR1:\n\trete\n";
	}

        if (int2Flag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "ISR2:\n\trete\n";
	}

        if (int3Flag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "ISR3:\n\trete\n";
	}

        if (int4Flag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "ISR4:\n\trete\n";
	}

        if (TDMRFlag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "TRX:\n\trete\n";
	}

        if (TDMTFlag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "TXMT:\n\trete\n";
	}

        if (TRAPFlag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "TRP:\n\trete\n";
	}

        if (NMIFlag == FALSE) {
		// generate dummy  interrupt handling procedure
		*defaultStream << "NMISR:\n\trete\n";
	}

	*defaultStream << "SETUP:\n\tret\n";

	TITarget::frameCode();	
}

ISA_FUNC(SimC50Target, C50Target);

// Register an instance
static SimC50Target proto("SimC50", "run code on the Sim TMS320C5x card");

static KnownTarget entry(proto, "SimC50");





