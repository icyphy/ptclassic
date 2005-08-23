defstar {
	name { Neg  }
	domain { C50 }
	desc { Negates input }
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
.Id "negation"
The input is moved into accumlator "a", where the "neg" instruction is applied.
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
	codeblock (negblock) {
	mar	*,AR6
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lacc	*,15,AR7			;Accu = input
	neg					;Accu = -input
	sach	*,1				;output = -input
	}
	go {
		addCode(negblock);
	}
}



