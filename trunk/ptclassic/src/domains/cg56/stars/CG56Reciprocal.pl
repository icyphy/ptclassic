defstar {
	name { Reciprocal }
	domain { CG56 }
	desc {
Output parameters are reciprocal of the inputs.
}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation {
Find the reciprocal of a fraction in terms of a fraction and some left shifts.
	}
	execTime {
		return 3;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name { f }
		type { fix }
	}
	output {
		name { s }
		type { fix }
	}
	state {
		name {Nf}
		type {int}
	        default {8}
	        desc {precision}
	}
        state {
                name {X}
	        type {int}
	        default {15}
	        attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
	codeblock (Rblock) {
        move    $ref(input),a
        move    #0,r7
	tst	a			; must setup flags before norm
$label(normalize)
        norm    r7,a
        jnn     $label(normalize)                   ;normalize data
        move    r7,y0
        move    #$$010001,b
        sub     y0,b    a,x0            ;number of left shifts in LSBs of b
        move    #0.5,a                  ;s = left shifts + 1 'cause div 0.5
        and     #$$fe,ccr
        rep     #$val(Nf)
        div     x0,a                    ;quotient (f) in Nf LSBs of a
        asl     a	b,$ref(s)
        rep     #$val(X)
        asl     a
        move    a0,$ref(f)
 	}
 	go {
                 X=23-int(Nf);
	 	 addCode(Rblock);
	}
}
