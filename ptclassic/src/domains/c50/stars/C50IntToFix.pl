defstar {
    name { IntToFix }
    domain { C50 }
    desc { Convert an integer input to a fixed-point output. }
    version { $Id$  }
    author { Luis Gutierrez }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }
    input {
	name {input}
	type {INT}
    }
    output {
	name {output}
	type {FIX}
    }
    constructor {
	noInternalState();
    }
    codeblock(convert) {
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
    }
    go {
	addCode(convert);
    }
    exectime { return 2; }
}
