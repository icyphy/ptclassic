defstar {
    name { LinToCirc }
    domain { CG56 }
    desc {
Copy data from a linear buffer to a circular buffer.  If N is zero (default),
the number transferred per execution equals the number written per execution
by the star that feeds the input. }
    version { $Id$ }
    author { J. Buck, Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 demo library }
    explanation {
.Id "linear to circular buffer copy"
.Ir "buffer, circular"
.Ir "buffer, linear"
.Ir "linear buffer"
.Ir "circular buffer"
This star is normally inserted automatically, as needed.  Data movement
is repeated inline so may not be efficient for large N.
    }
    input {
	name { input }
	type { anytype }
    }
    output {
	name { output }
	type { =input }
	attributes { P_CIRC }
    }
    defstate {
	name { N }
	type { int }
	desc { number to transfer per execution }
	default { 0 }
    }
    defstate {
	name { ptr }
	type { int }
	desc { storage for pointer register }
	default { 0 }
	attributes { A_YMEM|A_NOINIT|A_NONSETTABLE }
    }
    protected {
	int n;
    }
    setup {
	n = int(N);
	if (n == 0)
	    n = input.far()->numXfer();
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
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
	move	#>$addr(output),x0
	move	x0,$ref(ptr)
    }

    codeblock(setupC) {
	move	#$size(output)-1,m0
	move	$ref(ptr),r0
    }

    codeblock(oneReal,"int i, char* xmemLoc") {
	move	$ref(input,@i),x0
	move	x0,x:@xmemLoc
    }

    codeblock(oneComplex,"int i, char* xmemLoc") {
	move	L:$addr(input,@i),x
	move	x,L:@xmemLoc
    }

    codeblock(restore) {
	move	r0,$ref(ptr)
	move	m7,m0
    }
}
