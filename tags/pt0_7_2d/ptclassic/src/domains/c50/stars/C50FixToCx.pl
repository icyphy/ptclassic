defstar {
    name { FixToCx }
    domain { C50 }
    desc { Convert a fixed-point input to a complex output. }
    version { @(#)C50FixToCx.pl	1.4  8/16/96 }
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
	lacc	*,16,ar1
	sach	*+,0
	sacl	*,0
    }
    go {
	addCode(convert);
    }
    exectime { return 6; }
}
