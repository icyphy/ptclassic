defstar {
    name { FixToInt }
    domain { CG56 }
    desc { Convert a fixed-point input to a integer output. }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
    input {
	name {input}
	type {FIX}
    }
    output {
	name {output}
	type {INT}
    }
    constructor {
	noInternalState();
    }
    codeblock(convert) {
	move	$ref(input),x1
	move	x1,$ref(output)
    }
    // FIXME: On the 56000, there is no difference between an integer
    // and a fixed-point number: both are stored in the same word length.
    // The interpretation of the bits is dependent upon the instruction.
    // This star should require NO code, and instead behave as a fork.
    // However, making it behave as a fork in Ptolemy 0.7 and earlier
    // cause AsmGeodesic::internalBufSize to flag an error about the
    // maximum buffer size being too large.
    go {
	addCode(convert);
    }
    exectime { return 2; }
}
