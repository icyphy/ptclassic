defstar {
    name { BitsToInt }
    domain { CGC }
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
	Copyright (c) 1990-%Q% The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC demo library }
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
	default { 4 }
	desc {number of bits read per execution}
    }
    constructor {
	noInternalState();
    }
    setup {
	if (int(nBits) > sizeof(int)*8) {
	    StringList message = "nBits needs to be less than";
	    message << (unsigned int)(sizeof(int)*8);
	    Error::abortRun(*this,message);
	    return;
	}
	if (int(nBits) < 0) {
	    Error::abortRun(*this, "nBits must be positive");
	    return;
	}
	input.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite) {
	int i;
	int word = 0;
	for(i=$val(nBits)-1;i>=0;i--) {
	    /* shift new bits into the shift register */
	    word <<= 1;
	    if ($ref(input,i)) word += 1;
	}
	$ref(output) = word;
    }
    go {
	addCode(readNwrite);
    }
    exectime {
	return int(nBits)*2;
    }
}
