defstar {
    name { LinToCirc }
    domain { C50 }
    desc {
Copy data from a linear buffer to a circular buffer.
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
.Id "linear to circular buffer copy"
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

    defstate {
	name { ptr }
	type { int }
	desc { storage for pointer register (internal) }
	default { 0 }
	attributes { A_UMEM|A_NOINIT|A_NONSETTABLE }
    }

    defstate {
	name { N }
	type { int }
	desc { number to transfer per execution }
	default { 0 }
    }

    protected {
	int n,xferType,time;
	int inputBuffSize,outputBuffSize;
    }


    method {
	name { transferType }
	type { "void" }
	arglist { "()" }
	access { protected }
	code {
		outputBuffSize = output.bufSize();
		inputBuffSize = input.bufSize();
		xferType = 3;
		time = 11 + inputBuffSize;
		if (input.resolvedType() == COMPLEX ){
			if ((inputBuffSize == 2) &&
			    (outputBuffSize == 2)){
				xferType = 2;
				time = 4;
				return;
			}
		} else {
			if ((inputBuffSize == 1) &&
			    (outputBuffSize == 1)){
				xferType = 1;
				time = 2;
				return;
			}
		};
	}
    }

    setup {
	n = int(N);
	if (n == 0) {
	    n = input.far()->numXfer();
	}
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
    }

    initCode{ addCode(init); }

    go {
	
	transferType();
	switch (xferType){
		case 1:
			addCode(copyReal);
			break;
		case 2:
			addCode(copyCx);
			break;
		case 3:
			addCode(setupCircBuffer());
			addCode(moveData());
			addCode(restore);
			break;
	}
     }	

    
    exectime {
	transferType();
	return time;
    }

    codeblock(init){
	.ds	$addr(ptr)
	.word	$addr(output)
	.text
	}

    codeblock(setupCircBuffer,"") {
	lmmr	ar0,#$addr(ptr)		; ar0 = start address
	lacc	#$addr(output),0
	samm	cbsr1			; set circ buff start addr
	add	#@(outputBuffSize-1),0
	samm	cber1			; set circ buff end addr
	lacl	#8
	samm	cbcr			; set ar0 = cir. buff reg.
	mar	*,ar0			; arp -> ar0
    }

    codeblock(moveData,""){
	rpt	#@(inputBuffSize - 1)
	bldd	#$addr(input),*+
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
	samm	cbcr
	smmr	ar0,#$addr(ptr)
    }

}


