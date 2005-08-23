defstar {
	name { QntBtsLin }
	domain { C50 }
	desc { Linear quantizer with adjustable offset. }
	version { $Id$ }
	acknowledge { Gabriel version by E. A. Lee }
	author { Luis Gutierrez, based on the CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
        explanation {
.Id "quantizer"
Normally, the output is just the two's complement number
given by the top noBits of the input, but
an optional offset can be added to shift the output levels up or down.
        }
	input	{
		name { input }
		type { fix }
		}
        output {
		name { output }
		type { fix }
	}
	state {
		name { noBits }
		type { int }
		desc { Number of Bits. }
		default { 4 }
	}
	state {
		name { offset }
		type { fix }
		desc { amount of shift.  }
		default { 0 }
	}
 
	constructor {
		noInternalState();
	}
	codeblock(std,""){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,@noBits,ar1
	and	#ffffh,16
	add	#@offset,@noBits
	sach	*,@(16-noBits)
	}
        go { 
 		addCode(std);
 	}

	execTime { 
		return 7;
	}
 }
