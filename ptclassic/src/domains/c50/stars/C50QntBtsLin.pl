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
		type { Fix }
		desc { amount of shift.  }
		default { 0 }
	}
	
	private {
		int	off;
		
	}
	setup{
		// convert fixed point offset to an integer because the
		// assembler can only handle integer numbers in add immediate
		// instructions.
		double temp = offset.asDouble();
		if ( temp >= 0) {
			off = int(32768*temp);
		} else {
			off = int(32768*(1-temp));
		}
	}
 
	constructor {
		noInternalState();
	}
	codeblock(std,""){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,0,ar1
	and	#65535,@(16-int(noBits))
	add	#@off,0
	sacl	*,0
	}
        go { 
 		addCode(std());
 	}

	execTime { 
		return 7;
	}
 }



