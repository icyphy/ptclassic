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
