defstar {
	name { Abs }
	domain { C50 }
	desc { Absolute value }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }
	explanation {
.Id "absolute value"
The input is moved into accumlator "a", where the "abs" instruction is applied.
	}
	execTime {
		return 5;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (absblock) {
	mar 	*,AR6
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lacc	*,15,AR7			;Accu = input
	abs					;Accu = |input|
	sach	*,1				;output = |input|
	}
	go {
		addCode(absblock);
	}
}







