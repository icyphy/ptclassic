defstar {
    name { XCSynchComm }
    domain { CG56 }
    desc { S56X to CGC send/receive base star }
    version { @(#)CG56XCSynchComm.pl	1.7 01 Oct 1996 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1993-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation { }

    state {
	name {buffer}
	type {intarray}
	attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
	default {0}
    }

    state {
	name {bufferSemaphore}
	type {intarray}
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_SHARED|A_NOINIT }
	default {0}
    }
    
    state {
	name {currentBuffer}
	type {int}
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_SHARED|A_NOINIT }
	default {0}
    }

    header {
	class CGCXSynchComm;
    }

    public {
	CGCXSynchComm* cgcSide;
	int *commCount;
	int numXfer;
	int pairNumber;
	DataType resolvedType;
    }

    setup {
	buffer.resize(numXfer);
	if (resolvedType == COMPLEX) {
	    buffer.setAttributes(A_NONCONSTANT|A_NONSETTABLE|A_SYMMETRIC
				 |A_NOINIT);
	}
	bufferSemaphore.resize(*commCount/24+1);
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

codeblock(buffSemInit) {
        org	$ref(currentBuffer)
PTOLEMY_S56X_BUF
        dc	0
        org	$ref(bufferSemaphore)
PTOLEMY_S56X_SEM
        dc	0
        org	p:
}

initCode {
    addCode(buffSemInit,NULL,"PTOLEMY_S56X_SEM");
    addCode("	bset	#m_hf3,x:m_hcr",NULL,"hostInterruptEnable");
    if (resolvedType == COMPLEX) {
	@	org	x:$addr(buffer)
	@$starSymbol(buffer)_REAL
	@	org	y:$addr(buffer)
	@$starSymbol(buffer)_IMAG
    }
	
    @	org	$ref(buffer)
    @$starSymbol(buffer)
    for(int i=0;i<numXfer;i++)
        @	dc	0
    @	org	p:
}

}
