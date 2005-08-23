defstar {
	name { WasteCycles }
	domain { CG56 }
	desc {
A star to stall the flow of data for parameter cycles to waste cycles.
	}
	version { @(#)CG56WasteCycles.pl	1.13 03/29/97 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
	move	$ref(input),x1
	rep     #$val(X)
	nop
	move    x1,$ref(output)
	} 
	constructor {
		noInternalState();
	}
	go {
		X = int(cyclesToWaste) - 4;
		addCode(std);
	}
	execTime { 
		return int(cyclesToWaste);
	}
}
