defstar {
	name { WasteCycles }
	domain { C50 }
	desc {
A star to stall the flow of data for parameter cycles to waste cycles.
	}
	version { @(#)C50WasteCycles.pl	1.4	01 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
A star to stall the flow of data for parameter cycles to waste cycles.
	}
	input {
		name { input }
		type { fix }
	}	    
	output {
		name { output }
		type { fix }
	}
	state {
		name { cyclesToWaste }
		type { int }
		desc { numbers of cycles to waste }
		default { 10 }
	}
	state {
		name { X }
		type { int }
		desc { internal }
		default { 0 }
		attributes {A_NONSETTABLE|A_NONCONSTANT}
	}
	codeblock(std) {
	mar 	*,AR6				;
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar 	AR7,#$addr(output)		;Address output		=> AR7
	lacc	*,15,AR7			;Accu = input
	rpt     #$val(X)			;number of cycles to waste
	 nop					;waste cycles
	sach	*,1				;output = input
	} 
	constructor {
		noInternalState();
	}
	go {
		X = int(cyclesToWaste) - 5;
		addCode(std);
	}
	execTime { 
		return int(cyclesToWaste);
	}
}


