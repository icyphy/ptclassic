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
    location { CG56 conversion library }
    input {
	name {input}
	type {FIX}
    }
    output {
	name {output}
	type {INT}
    }
    codeblock(convert) {
	move	$ref(input),x1
	move	x1,$ref(output)
    }
    go {
	addCode(convert);
    }
    exectime { return 2; }
}
