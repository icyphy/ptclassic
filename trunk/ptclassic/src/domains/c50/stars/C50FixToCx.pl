defstar {
    name { FixToCx }
    domain { C50 }
    desc { Convert a fixed-point input to a complex output. }
    version { $Id$ }
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
	type {COMPLEX}
    }
    constructor {
	noInternalState();
    }
    codeblock(convert) {
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,0,ar1
	sacl	*+,0
	sach	*,0
    }
    go {
	addCode(convert);
    }
    exectime { return 6; }
}
