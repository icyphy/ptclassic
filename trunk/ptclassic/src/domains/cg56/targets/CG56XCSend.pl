defstar {
	name { S56XCGCSend }
	domain { CG56 }
	desc { S56X to CGC send star }
	version { $Id$ }
	author { Jose Pino }
	derivedFrom { S56XCGCBase }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	ccinclude { "CGTarget.h" }
	location { CG56 Target Directory }
	explanation {
	}

	input {
		name {input}
		type {ANYTYPE}
	}

	codeblock(sendOne) {
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait) ;wait for host port available
	movep	$ref(input),x:m_htx
	}

	codeblock(sendMany) {
	move	#$addr(input),r0	;read starting location address
	do	a,$label(WHL)
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait) ;wait for host port available
	movep	$mem(input):(r0)+,x:m_htx
$label(WHL)
	nop
	}	
codeblock(intTable) {
;
; HostPort Send Interrupt Point
;
$label(SAVEPC)  equ     *       ;save program counter
        org     p:i_hstcm7              ; Host command 7
STARTR  jsr	$starSymbol(DMAREAD) 
	org	p:$label(SAVEPC)	;restore program counter
}

codeblock (intSub) {
;
; HostPort Send Interrupt Subroutine
;
$starSymbol(DMAREAD)
	movep   x:m_hrx,$ref(wordCnt)   ; Save word count
        nop
	rti
}

	setup {
		numXfer = input.numXfer();
		CG56S56XCGCBase::setup();
        }

        initCode {
		CG56S56XCGCBase::initCode();
		addCode(intSub,PROCEDURE,"DMASendSubroutine");
		addCode(intTable,CODE,"DMASendPointer");
	}

	go {
		addCode(wordCntCB);
		if (input.numXfer()==1)
			addCode(sendOne);
		else
			addCode(sendMany);
		addCode(resetWordCnt);
	}

	execTime {
		if (input.numXfer()==1) return 9;
		return (9 + 2*input.numXfer());
	}
}
