defstar {
	name { Sqrt }
	domain { CG56 }
	desc { Square Root. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
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
