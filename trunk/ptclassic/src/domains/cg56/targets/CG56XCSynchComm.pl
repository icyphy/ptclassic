defstar {
	name { XCSynchComm }
	domain { CG56 }
	desc { S56X to CGC send/receive base star }
	version { $Id$ }
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	explanation {
	}

	state {
		name {buffer}
		type {intarray}
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
		default {0}
	}

	state {
		name {bufferSemaphore}
		type {intarray}
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_SHARED}
		default {0}
	}
 
 	state {
 		name {currentBuffer}
 		type {int}
 		attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_SHARED}
 		default {0}
 	}

	header {
	  class CGCXSynchComm;
	}

 	public {
		CGCXSynchComm* cgcSide;
		int commCount;
		int numXfer;
		int pairNumber;
	}

	setup {
		buffer.resize(numXfer);
		bufferSemaphore.resize(commCount/24+1);
	}

codeblock(processPendingInterrupts,"int pairNumber") {
; Trigger Host Interrupt
; make sure no interrupts pending
$label(pending)
	move	$ref(currentBuffer),a
	tst	a
	jne	$label(pending)
	nop

; set host interrupt pending flag to current pair number
	move	#@(pairNumber+1),a0
	move	a0,$ref(currentBuffer)
}


codeblock(triggerInterrupt) {
; trigger interrupt
	move    P:$$8000,a0              ; trigger host SIGUSR1 interrupt
}

method {
    name{bufferName}
    type{"const char*"}
    access{public}
    arglist{"()"}
    code {
	return starSymbol.lookup("buffer");
    }
}

initCode {
	addCode("	bset	#m_hf3,x:m_hcr",NULL,"hostInterruptEnable");
	@	org	$ref(buffer)
	@$starSymbol(buffer)
        for(int i=0;i<numXfer;i++)
	    @	dc	0
	@	org	p:
}

}
