defstar {
	name { Quant }
	domain { CG56 }
	desc { Linear fractional with adjustable offset. }
	version { $Id$ }
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
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
 	state  {
		name { X }
		type { fix }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
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
		
                double a = pow(2,1-double(noBits));
   		X=a;
 		gencode(std);
 	}

	execTime { 
		return 6;
	}
 }
