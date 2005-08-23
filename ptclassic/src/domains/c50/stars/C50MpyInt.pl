defstar {
	name { MpyInt }
	domain { C50 }
	desc {
Two input integer multiplier.  The output wraps around on overflow.
	}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 arithmetic library }
	explanation {
.Id "multiplication, integer"
.Id "integer multiplication"
Input and output are assumed to be integers (right-justified in word).
}

	input {
		name {firstInput}
		type {int}
	}
	input {
		name {secondInput}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	codeblock (multblock) {
	mar	*,AR1
	lar	AR1,#$addr(firstInput)		;Address firstInput	=> AR1
	lar	AR2,#$addr(secondInput)		;Address secondInput	=> AR2
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lt	*,AR2			    	;input#1 =>  TREG0
	mpy 	*,AR7				;P-Reg. = input#1*input#2
	spl 	*				;output = P-Reg.
	}
 	go {
		addCode(multblock);
  	}
	exectime {
	 	return 7; 
	}
}
