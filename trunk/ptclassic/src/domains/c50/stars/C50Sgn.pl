defstar {
	name { Sgn }
	domain { C50 }
	desc { Signum. }
	version { $Id$ }
	acknowledge { Gabriel version by E. A. Lee }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
       	location { C50 nonlinear functions library }
	explanation { 
This star outputs the signum of its input.
.Id "signum"
The output is either -1.0 or 1.0. 
	}
	execTime {
		return 12;
	}
	input {
		name{input}
		type{FIX}
	}
	output {
		name{output}
		type{FIX}
	}
	codeblock (sgnblock) {
   	mar	*,AR6				;
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lacc    *,15,AR7			;Accu = input
	nop					;
	xc	2,LEQ				;if Accu <= 0 then
	 lar	AR0,#08000h			; -1.0 (in Q15) to AR0
	 nop					;
	xc	2,GT				;if Accu > 0 then
	 lar	AR0,#07fffh			; 0.9999395 (in Q15) to AR0
	 nop					;
        sar    	AR0,*				;ouput = AR0
	}	

	go {
		addCode(sgnblock);
 
	}
}
