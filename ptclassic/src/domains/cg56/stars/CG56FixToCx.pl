defstar {
    name { FixToCx }
    domain { CG56 }
    desc { Convert a fixed-point input to a complex output. }
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
	type {COMPLEX}
    }
    constructor {
	noInternalState();
    }
    codeblock(convert) {
	move	$ref(input),x1
	move	#0.0,x0
	move	x,$ref(output)
    }
    go {
	addCode(convert);
    }
    exectime { return 3; }
}
