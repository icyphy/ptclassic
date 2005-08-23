defstar {
    name { BitsToInt }
    domain { C50 }
    desc { Packs several input bits into an integer. }
    version { @(#)C50BitsToInt.pl	1.4	06 Oct 1996 }
    author { Luis Gutierrez }
    copyright {
	Copyright (c) 1990-1996 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 demo library }
	htmldoc {
The integer input sequence is interpreted as a bit stream in which any nonzero
value is interpreted as to mean a "one" bit.
This star consumes <i>nBits</i> successive bits from the input,
packs them into an integer, and outputs the resulting integer.
The first received bit becomes the most significant bit of the output.
If <i>nBits</i> is smaller than the word size minus one, then the
output integer will always be non-negative.
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
	if (int(nBits) > 15) {
	    Error::abortRun(*this,"nBits needs to be less than 15");
	    return;
	}
	input.setSDFParams(int(nBits),int(nBits)-1);
    }
    codeblock(readNwrite,"") {
	lar	ar3,#$addr(input)
	lacl	#@(nBits-1)
	samm	brcr		; seup block counter reg.
	zap
	samm	arcr		; setup compare reg
	lar	ar1,#$addr(output)
	mar	*,ar3
	rptb	$label(compress)
	lar	ar2,*+,ar2
	cmpr	3		; check that input != 0
	sfl			; shift acc left 1 bit
	xc	1,TC		
	add	#01h		; add 1 to acc if input!=0
$label(compress)
	mar	*,ar3
	mar	*,ar1
	sacl	*
	lacc	*,@(16-nBits)
	sacl	*,0
	}

    go {
	addCode(readNwrite());
    }
    exectime {
	return int(nBits)*6 + 12;
    }
}


