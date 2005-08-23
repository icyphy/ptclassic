defstar {
	name { Sqr }
	domain { C50 }
	desc { Square function.}
	version { @(#)C50Sqr.pl	1.4	01 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {The output is the square of the input.}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
 	codeblock (std) {
	mar	*,AR6				;
	lar 	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	zap					;clear P-Reg and Accu
	sqra	*,AR7				;P-Reg = input*input
	pac					;Accu = input*input
   	sach	*,1				;output = input*input
	}
	constructor {
		noInternalState();
	}
 	go {
	 	 addCode(std);
	}
	execTime {
		return 6;
	}
}
