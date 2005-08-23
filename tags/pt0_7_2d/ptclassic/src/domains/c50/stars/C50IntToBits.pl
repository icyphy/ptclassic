defstar {
    name { IntToBits }
    domain { C50 }
    desc {
Read the least significant nBits bits from an integer input,
and output the bits as integers serially on the output,
most significant bit first.
    }
    version {@(#)C50IntToBits.pl	1.4	05/26/98}
    author { Luis Gutierrez, G. Arslan }
    copyright {
	Copyright (c) 1990-1998 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 demo library }
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
	default {16}
    }
    constructor {
	noInternalState();
    }
    setup {
	if (int(nBits) > 16) {
	    Error::abortRun(*this,"nBits needs to be less than 17");
	    return;
	}
	output.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite,"") {
	lar	ar0,#$addr(input)
	lar	ar1,#($addr(output)+@(nBits-1))
	lar	ar2,#0000h
	lar	ar3,#0001h
	mar	*,ar0
	lacl	#@(nBits-1)
	samm	brcr		; set up repeat block counter
	lacc	*,0,ar1
	rptb	$label(dcrmp)
	sfr
	sar	ar2,*
	xc	1,C	
	sar	ar3,*
$label(dcrmp)
	mar	*-
	}


    go {
	addCode(readNwrite());
    }
    exectime {
	return int(nBits)*5 + 9;
    }
}
