defstar {
	name { S56XCGCReceive }
	domain { CG56 }
	desc { CGC to S56X Receive star }
	version { $Id$ }
	author { Jose L. Pino }
	derivedFrom {S56XCGCBase}
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	ccinclude { "CGCS56XReceive.h" }
	explanation {
	}
	output {
		name {output}
		type {ANYTYPE}
	}

	codeblock(receiveOne) {
$label(wait)
	jclr	#m_hrdf,x:m_hsr,$label(wait) ;wait for host port available
	movep	x:m_hrx,$ref(output)
	}

codeblock(receiveMany) {
	move	#$addr(output),r0	;read starting location address
	do	a,$label(WHL)
$label(wait)
	jclr	#m_hrdf,x:m_hsr,$label(wait) ;wait for host port available
	movep	x:m_hrx,$mem(output):(r0)+
$label(WHL)
	nop
}
        codeblock(intTable) {
;
; HostPort Receive Interrupt Pointer
;
$label(SAVEPC)  equ     *       ;save program counter
	org	p:i_hstcm8		; Host command 8
STARTW	jsr	$starSymbol(DMAWRITE)
	org	p:$label(SAVEPC)	;restore program counter
	}

	codeblock (intSub) {
;
; HostPort Receive Interrupt Subroutine
;
$starSymbol(DMAWRITE)
	movep   x:m_hrx,$ref(wordCnt)   ; Save word count
        nop
	rti
        }
	

	setup {
	    numXfer = output.numXfer();
	    CG56S56XCGCBase::setup();
	    PortHole* input = cgcSide->portWithName("input");
	    DataType inputType = input->resolvedType();
	    if (strcmp(inputType,FLOAT) == 0) {
		    output.setPort("output",this,FIX,numXfer);
	    }
        }

        initCode {
	    CG56S56XCGCBase::initCode();
	    addCode(intSub,PROCEDURE,"DMAReceiveSubroutine");
            addCode(intTable,CODE,"DMAReceivePointer");
	}

	go {
		addCode(wordCntCB);
		if (output.numXfer()==1)
			addCode(receiveOne);
		else
			addCode(receiveMany);
		addCode(resetWordCnt);
	}

	execTime {
		if (output.numXfer()==1) return 9;
		return (9 + 2*output.numXfer());
	}
}
