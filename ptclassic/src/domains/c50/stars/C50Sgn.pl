defstar {
	name { Sgn }
	domain { C50 }
	desc {
This star computes the signum of its input.
The output is +/- 1.  Note that 0.0 maps to 1.
	}
	version { @(#)C50Sgn.pl	1.4	01 Oct 1996 }
	acknowledge { Gabriel version by E. A. Lee }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
       	location { C50 main library }
	htmldoc {
<a name="signum"></a>
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
        sar    	AR0,*				;output = AR0
	}	
	constructor {
		noInternalState();
	}
	go {
		addCode(sgnblock);
 
	}
	execTime {
		return 12;
	}
}
