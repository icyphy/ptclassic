defstar {
    name { IntToBits }
    domain { CGC }
    desc {
Read the least significant nBits bits from an integer input,
and output the bits as integers serially on the output,
most significant bit first.
    }
    version { @(#)CGCIntToBits.pl	1.8 01 Oct 1996 }
    author { Jose Luis Pino }
    copyright {
	Copyright (c) 1990-1996 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
	htmldoc {
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
	desc {Number of bits to place in one word}
    }
    constructor {
	noInternalState();
    }
    setup {
        /* Need (int) cast on sizeof to eliminate gcc warning */
	if (int(nBits) > int(sizeof(int)*8)) {
	    StringList message = "nBits needs to be less than ";
	    message << (unsigned int)(sizeof(int)*8);
	    Error::abortRun(*this, message);
	    return;
	}
	if (int(nBits) < 0) {
	    Error::abortRun(*this, "nBits must be positive");
	    return;
	}
	output.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite) {
	int word;
	int i = 0;
	word = $ref(input);
	for(; i < $val(nBits); i++) {
 	    $ref(output,i) = (word & 1);
	    word >>= 1;
	}
    }
    go {
	addCode(readNwrite);
    }
    exectime {
	return int(nBits)*2;
    }
}
