defstar {
	name { S56XCGCReceive }
	domain { CG56 }
	desc { CGC to S56X Receive star }
	version { $Id$ }
	author { Jose L. Pino }
	ccinclude { "CGTarget.h" }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	explanation {
	}
	output {
		name {output}
		type {ANYTYPE}
	}

	state {
		name {wordCnt}
		type {int}
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_SHARED|A_YMEM}
		default {0}
	}

	codeblock(interrupt) {
$label(SAVEPC)	equ	*	;save program counter
;
; DMA Receive Request Interrupt
;
	org	p:i_hstcm7		; Host command 7
STARTR	movep	x:m_hrx,$ref(wordCnt)	; Allow DSP writes to host port
	nop
	org        p:$label(SAVEPC)	;restore program counter
	}

	codeblock(wordCntCB) {
$label(initial_wait)
	move	$ref(wordCnt),a	; get word count
	tst 	a
	jeq	$label(initial_wait)
	jclr	#m_dma,x:m_hsr,$label(initial_wait)
	}

	codeblock(receiveOne) {
$label(wait)
	jclr	#m_hrdf,x:m_hsr,$label(wait) ;wait for host port available
	movep	x:m_hrx,$ref(output)
	}

	codeblock(receiveMany) {
	move #$addr(output),r0	;read starting location address
	do	a,$label(WHL)
$label(wait)
	jclr	#m_hrdf,x:m_hsr,$label(wait) ;wait for host port available
	movep	x:m_hrx,$mem(output):(r0)+
$label(WHL)
	nop
	}

	codeblock(resetWordCnt) {
	move	#0,a
	move	a,$ref(wordCnt)
	nop
	}

	initCode {
		addCode(interrupt,CODE,"STARTR");
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
