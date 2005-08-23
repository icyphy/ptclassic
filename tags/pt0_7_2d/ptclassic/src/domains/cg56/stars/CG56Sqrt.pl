defstar {
	name { Sqrt }
	domain { CG56 }
	desc { Square Root. }
	version { @(#)CG56Sqrt.pl	1.9 01 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
Output is the square root of input. 
Negative values are equated to zero.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	constructor {
		noInternalState();
	}
	codeblock(sqrtblock) {
        clr     b               #<$$40,y0
        move    $ref(input),x0
        move    y0,y1
	do      #23,$label(sqrtend)
        mpyr    -y0,y0,a
        add     x0,a
        tge     y0,b
        tfr     y1,a
        asr     a
        add     b,a             a,y1
        move    a,y0
$label(sqrtend)
        move    b,$ref(output)
  	}
 	go {
 		addCode(sqrtblock);
 	}

	exectime {
		return 165;
 	}
}
