defstar {
	name { QntBtsInt }
	domain { CG56 }
	desc { Integer quantizer with adjustable offset. }
	version { @(#)CG56QntBtsInt.pl	1.11 06 Oct 1996 }
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
Normally, the <i>output</i> is just the two's complement number
given by the top <i>noBits</i> of the <i>input</i> (for integer output).
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
	constructor {
		noInternalState();
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
