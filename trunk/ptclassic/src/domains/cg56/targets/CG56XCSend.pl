defstar {
	name { S56XCGCSend }
	domain { CG56 }
	desc { S56X to CGC send star }
	version { $Id$ }
	author { Jose Pino }
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

	state {
		name {wordCnt}
		type {int}
                attributes {A_NONCONSTANT|A_NONSETTABLE|A_SHARED|A_YMEM}
                default {0}
	}

        codeblock(interrupt) {
$label(SAVEPC)  equ     *       ;save program counter
;
; DMA Send Request Interrupt
;
	org	p:i_hstcm8		; Host command 8
STARTW	movep	x:m_htx,$ref(wordCnt)	; Allow DSP writes to host port
	nop
	org	p:$label(SAVEPC)	;restore program counter
        }
	
	codeblock(wordCntCB) {
$label(initial_wait)
	move	$ref(wordCnt),a	; get word count
	tst 	a
	jeq	$label(initial_wait)
	jclr	#m_dma,x:m_hsr,$label(initial_wait)
	}

	codeblock(sendOne) {
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait) ;wait for host port available
	movep	$ref(input),x:m_htx
	}

	codeblock(sendMany) {
	move #$addr(input),r0	;read starting location address
	do	a,$label(WHL)
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait) ;wait for host port available
	movep	$mem(input):(r0)+,x:m_htx
$label(WHL)
	nop
	}

	codeblock(resetWordCnt) {
	move	#0,a
	move	a,$ref(wordCnt)
	nop
	}

	initCode {
		addCode(interrupt,CODE,"STARTW");
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
