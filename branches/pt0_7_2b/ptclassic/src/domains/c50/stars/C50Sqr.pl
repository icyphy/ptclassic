defstar {
	name { Sqr }
	domain { C50 }
	desc { Square function.}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }
	explanation {The output is the square of the input.}
	execTime {
		return 6;
	}
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
   	sach	*,1				;ouput = input*input
	}
 	go {
	 	 addCode(std);
	}
}



