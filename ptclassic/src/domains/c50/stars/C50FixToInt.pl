defstar {
    name { FixToInt }
    domain { C50 }
    desc { Convert a fixed-point input to an integer output. }
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
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
    }
    go {
	addCode(convert);
    }
    exectime { return 2; }
}
