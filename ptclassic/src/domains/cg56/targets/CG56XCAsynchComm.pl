defstar {
name { XCAsynchComm }
domain { CG56 }
desc { S56X to CGC asynchronous IPC base star }
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
    name { updateRate }
    type { int }
    default { 1 }
    desc {Update the link at most every 'updateRate' schedule iterations.}
}

state {
    name { VariableName }
    type { string }
    default { aioVariable }
    desc {Name of buffer.}
    attributes { A_NONSETTABLE} 
}

state {
    name { blockSize }
    type { int }
    default { 1 }
    desc { number of tokens to be transfered per firing. }
}

state {
    name {buffer}
    type {intarray}
    default {0}
    attributes { A_YMEM|A_NOINIT|A_NONSETTABLE|A_CONSEC} 
}

state {
    name { bufStart }
    type { int }
    default { 0 }
    desc { pointer to the buffer }
    attributes { A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
}

codeblock(initBuffer,"") {
	bclr    #m_hpl0,x:m_ipr		; set Host Port IPL 1
	bset    #m_hpl1,x:m_ipr
	org	$ref(bufStart)
	dc	$addr(buffer)
	org	$ref(buffer)
$val(VariableName)
}

setup {
    if (blockSize > 1) {
	buffer.resize(blockSize);
    }
}

initCode {
	addCode(initBuffer());
	for (int i = 0 ; i < blockSize ; i++) {
		addCode("	dc	0\n");
	}
	addCode("	org	p:\n");
}

}
