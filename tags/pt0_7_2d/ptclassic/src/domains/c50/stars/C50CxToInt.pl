defstar {
    name { CxToInt }
    domain { C50 }
    desc { 
Outputs the magnitude of the complex number. 
The output will wrap around in case of overflow
	}
    version { @(#)C50CxToInt.pl	1.3   7/22/96  }
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
	type {COMPLEX}
    }
    output {
	name {output}
	type {INT}
    }
    constructor {
	noInternalState();
    }
    codeblock(magnitude) {
	lar	ar0,#$addr(input,0)
	lar	ar1,#$addr(output)
	mar	*,ar0
	zap	
	sqra	*+
	sqra	*,ar1
	apac
	sacl	*
    }
    go {
	addCode(magnitude);
    }
    exectime { return 7; }
}
