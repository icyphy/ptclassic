defstar {
    name { CircToLin }
    domain { CG56 }
    desc {
Copy data from a circular buffer to a linear buffer.
If N is zero (the default), the number of samples transferred per execution
equals the number of samples read per execution by the star that reads
from the output. 
    }
    version { $Id$ }
    author { J. Buck & Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 demo library }
    explanation {
.Id "circular to linear buffer copy"
.Ir "buffer, circular"
.Ir "buffer, linear"
.Ir "linear buffer"
.Ir "circular buffer"
This star is normally inserted automatically, as needed.
Data movement is repeated inline so may not be efficient for large N.
    }
    input {
        name { input }
        type { anytype }
        attributes { P_CIRC}
    }
    output {
	name { output }
	type { =input }
    }
    protected {
	int n;
    }
    defstate {
	name { N }
	type { int }
	desc { number to transfer per execution }
	default { 0 }
    }
    setup {
	n = int(N);
	if (n == 0) {
	    n = output.far()->numXfer();
	}
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
    }
    defstate {
	name { ptr }
	type { int }
	desc { storage for pointer register (internal) }
	default { 0 }
	attributes { A_YMEM|A_NOINIT|A_NONSETTABLE }
    }
    initCode { addCode(init);}
    go {
	if (n>1) addCode(setupC);
	for (int j = 0; j < n; j++) {
	    if (input.resolvedType() == COMPLEX)
		addCode(oneComplex(j,n==1?"$addr(output)":"(r0)+"));
	    else
		addCode(oneReal(j,n==1?"$addr(output)":"(r0)+"));
	}
	if (n>1) addCode(restore);
    }
    
    exectime {
	return 4+2*n;
    }

    codeblock(init) {
	move	#>$addr(input),x0
	move	x0,$ref(ptr)
    }

    codeblock(setupC) {
	move	$ref(ptr),r0
	move	#$size(input)-1,m0
	nop			; can't read from circbuf right away
    }

    codeblock(oneReal,"int i, char* xmemLoc") {
	move	x:@xmemLoc,x0
	move	x0,$ref(output,@i)
    }
	
    codeblock(oneComplex,"int i, char* xmemLoc") {
	move	L:@xmemLoc,x
	move	x,L:$addr(output,@i)
    }

    codeblock(restore) {
	move	r0,$ref(ptr)
	move	m7,m0
    }
}
