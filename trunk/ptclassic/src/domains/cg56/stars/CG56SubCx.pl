defstar {
    name { SubCx }
    domain { CG56 }
    desc { Output the "pos" input minus all "neg" inputs. }
    version { @(#)CG56SubCx.pl	1.2	01 Oct 1996 }
    author { Brian L. Evans and Jose Luis Pino }
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
    input {
	name {pos}
	type {COMPLEX}
    }
    inmulti {
	name {neg}
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
	@	move	x:$addr(pos),a
	@	move	y:$addr(pos),b

	for (int i = 1; i <= neg.numberPorts(); i++) {
	    @	move	$ref(neg#@i),x
	    @	sub	x1,a
	    @	sub	x0,b
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

    exectime { return 4 + 3*neg.numberPorts(); }
}
