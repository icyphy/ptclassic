defstar {
    name { LinToCirc }
    domain { C50 }
    desc {
Copy data from a circular buffer to a linear buffer.
If N is zero (the default), the number of samples transferred per execution
equals the number of samples read per execution by the star that reads
from the output. 
    }
    version { $Id$  }
    author { Luis Gutierrez }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 demo library }
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
    }
    output {
	name { output }
	type { =input }
	attributes { P_CIRC }
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
	attributes { A_UMEM|A_NOINIT|A_NONSETTABLE }
    }

    initCode{ addCode(init); }

    go {

	if (input.bufSize()>1) {
	    addCode(setupC(n-1));
	    addCode(moveData(n-1));
	    addCode(restore);
	} else {
	    if (input.resolvedType() == COMPLEX){
	        addCode(copyCx);
	    } else  addCode(copyReal);
	}
    }
    
    exectime {
	return 4+2*n;
    }

    codeblock(init){
	.ds	#$addr(ptr)
	.word	$addr(input)
	.text
	}

    codeblock(setupC,"int buffSize") {
	lmmr	ar0,#$addr(ptr)		; ar0 = start address
	lacc	#$addr(input),0
	samm	cbsr1			; set circ buff start addr
	add	#@buffSize,0
	samm	cber1			; set circ buff end addr
	lacl	#08
	samm	cbcr			; set ar0 = cir. buff reg.
	mar	*,ar0			; arp -> ar0
    }

    codeblock(moveData,"int iter"){
	rpt	#@iter
	bldd	#$addr(output),*+
    }

    codeblock(copyReal){
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
    }

    codeblock(copyCx){
	lmmr	ar0,#$addr(input)
	lmmr	ar1,#$addr(input,1)
	smmr	ar0,#$addr(output)
	smmr	ar1,#$addr(output,1)
    }

    codeblock(restore) {
	zap
	samm	brcr
	smmr	ar0,#$addr(ptr)
    }

}


