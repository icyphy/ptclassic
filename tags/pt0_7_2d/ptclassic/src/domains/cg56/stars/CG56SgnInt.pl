defstar {
	name {SgnInt}
	domain {CG56}
	desc {
Output +1 if the input is positive or zero, and 0 if the input is negative.
This definition of signum is not compatible with the Sgn stars.
	}
	version { @(#)CG56SgnInt.pl	1.5	01 Oct 1996 }
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
This star outputs the signum of its input.
<a name="signum"></a>
The output is either 0 or 1
	}
	execTime {
		return 5;
	}
	input {
		name{input}
		type{FIX}
	}
	output {
		name{output}
		type{INT}
	}
	constructor {
		noInternalState();
	}
	codeblock (sgnblock) {
; Register usage:  a = input, b1 = sign of input (1 or 0), x0 = 0
	move    $ref(input),a
	move    #1,b1
	tst     a	#0,x0
	tmi     x0,b
	move    b1,$ref(output)
	}	
	go {
		addCode(sgnblock);
	}
}
