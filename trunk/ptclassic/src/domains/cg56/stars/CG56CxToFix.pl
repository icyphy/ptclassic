defstar {
    name { CxToFix }
    domain { CG56 }
    desc { Convert a complex input to a fixed-point output. }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 conversion library }
    input {
	name {input}
	type {COMPLEX}
    }
    output {
	name {output}
	type {FIX}
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
