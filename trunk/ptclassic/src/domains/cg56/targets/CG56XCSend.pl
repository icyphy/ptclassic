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
	location { CG56 Target Directory }
	explanation {
	}

	input {
		name {input}
		type {ANYTYPE}
	}

	codeblock(wordCnt) {
$label(initial_wait)
	move	y:WordCnt,a	; get word count
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
	move	a,y:WordCnt
	nop
	}

	go {
		addCode(wordCnt);
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
