defstar {
    name { CxToInt }
    domain { CG56 }
    desc { Convert a complex input to a fixed-point output. }
    version { @(#)CG56CxToInt.pl	1.2 2/7/96 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
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
	move	$ref(input),x
	mpy	x0,x0,a
	macr	x1,x1,b
	move	a,$ref(output)
    }
    go {
	addCode(magnitude);
    }
    exectime { return 4; }
}
