defstar {
    name { IntToBits }
    domain { CG56 }
    desc {
Read the least significant nBits bits from an integer input,
and output the bits as integers serially on the output,
most significant bit first.
    }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
	Copyright (c) 1994 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 demo library }
    explanation {
    }
    input {
	name {input}
	type {int}
    }
    output {
	name {output}
	type {int}
    }
    state {
	name {nBits}
	type {int}
	desc {Number of bits to place in one word}
	default {23}
    }
    constructor {
	noInternalState();
    }
    setup {
	if (int(nBits) > 23) {
	    Error::abortRun(*this,"nBits needs to be less than 24");
	    return;
	}
	output.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite) {
	move    #$addr(output)+$val(nBits)-1,r0
	move    $ref(input),a1
        do      #$val(nBits),$label(decompress)
        ror     a
	jcc     $label(clear)
        bset    #0,x:(r0)-
        jmp     $label(continue)      
$label(clear)
	bclr    #0,x:(r0)-
$label(continue)
	nop
$label(decompress)
    }
    go {
	addCode(readNwrite);
    }
    exectime {
	return int(nBits)*3;
    }
}
