defstar {
	name { Abs }
	domain { C50 }
	desc { Absolute value }
	version { @(#)C50Abs.pl	1.4	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="absolute value"></a>
The input is moved into accumulator "a", where the "abs" instruction
is applied.
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
	constructor {
		noInternalState();
	}
	go {
		addCode(absblock);
	}
	execTime {
		return 5;
	}
}
