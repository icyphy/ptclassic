defstar {
	name { QuantBitsInt }
	domain { CG56 }
	desc { Integer quantizer with adjustable offset. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
        explanation {
.Id "quantizer"
Normally, the output is just the two's complement number
given by the top noBits of the input (for integer output)
        }
	input	{
		name { input }
		type { fix }
		}
        output {
		name { output }
		type { int }
	}
	state {
		name { noBits }
		type { int }
		desc {   }
		default { 4 }
	}
	state  {
		name { Y }
		type { int }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        codeblock(std) {
	move	$ref(input),x0
	} 
	codeblock(cont3) {
	move	#$val(Y),a1
	move	a1,y0
	}
	codeblock(cont4) {
	move	#$val(Y),y0
	}
	codeblock(cont5) {
	mpy	x0,y0,a
	move	a1,$ref(output)
	}
	codeblock(cont6) {
	move	x0,$ref(output)
	}
	ccinclude {<math.h>}
        go { 
                Y = int(pow(2,noBits-1));
 		

		addCode(std);
	        if (noBits !=24) {
	                    if (noBits<9) 
				 addCode(cont3);
			    else
			         addCode(cont4);
			    addCode(cont5);
		}
		else
			addCode(cont6);
   	}
	

	execTime { 
		return 5;
	}
}
