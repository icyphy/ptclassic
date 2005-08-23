defstar {
    name { AddCx }
    domain { CG56 }
    desc { Add any number of inputs, producing an output. }
    version { @(#)CG56AddCx.pl	1.4 01 Oct 1996 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
	htmldoc {
The inputs are added and the result is written on the output.
    }
    inmulti {
	name {input}
	type {COMPLEX}
    }
    output {
	name {output}
	type {COMPLEX}
    }
    state {
	name { saturation }
	type { int }
	default { "YES" }
	desc { If true, use saturation arithmetic }
    }
    constructor {
	noInternalState();
    }

    go {
	@	move	x:$addr(input#1),a
	@	move	y:$addr(input#1),b

	for (int i = 2; i <= input.numberPorts(); i++) {
	    @	move	$ref(input#@i),x
	    @	add	x1,a
	    @	add	x0,b
	}

	if (int(saturation)) {
	    @	move	a,x:$addr(output)
	    @	move	b,y:$addr(output)
	}
	else {
	    @	move	a1,x:$addr(output)
	    @	move	b1,y:$addr(output)
	}
    }

    exectime { return 4 + 3*(input.numberPorts() - 1); }
}
