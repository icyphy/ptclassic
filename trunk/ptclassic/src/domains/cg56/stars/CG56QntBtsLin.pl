defstar {
	name { QntBtsLin }
	domain { CG56 }
	desc { Linear quantizer with adjustable offset. }
	version { @(#)CG56QntBtsLin.pl	1.10 01 Oct 1996 }
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="quantizer"></a>
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
 	state  {
		name { X }
		type { fix }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	constructor {
		noInternalState();
	}
        codeblock(std) {
	move	$ref(input),x0
	move	#-$val(X),a
 	move	#$val(offset),x1
	and	x0,a
	add	x1,a
	move	a1,$ref(output)
	}
  	ccinclude {<math.h>}

        go { 
		// There is no IntState to double cast, so cast to int first.
                double a = pow(2,1-double(int(noBits)));
   		X=a;
 		addCode(std);
 	}

	execTime { 
		return 6;
	}
 }
