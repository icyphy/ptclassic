defstar {
    name { BitsToInt }
    domain { CG56 }
    desc {
The integer input sequence is interpreted as a bit stream in which any nonzero
value is interpreted as to mean a 'one' bit.
This star consumes "nBits" successive bits from the input,
packs them into an integer, and outputs the resulting integer.
The first received bit becomes the most significant bit of the output.
If "nBits" is smaller than the wordsize minus one, then the
output integer will always be non-negative.
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
	default {4}
    }
    constructor {
	noInternalState();
    }
    setup {
	if (int(nBits) > 23) {
	    Error::abortRun(*this,"nBits needs to be less than 23");
	    return;
	}
	input.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite) {
	move    #>$addr(input),r0
	clr     a
        do      #$val(nBits),$label(compress)
        lsl     a
	jclr	#0,x:(r0)+,$label(dontSet)
	bset    #0,a1
$label(dontSet)
	nop
$label(compress)
	move    a1,$ref(output)
    }
    go {
	addCode(readNwrite);
    }
    exectime {
	return int(nBits)*3 + 4;
    }
}

