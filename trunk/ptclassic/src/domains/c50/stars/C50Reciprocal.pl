defstar {
	name { Reciprocal }
	domain { C50 }
	desc {
Output parameters are reciprocal of the inputs.
The time to execute the division algorithm will be
Nf + 2 cycles if Nf < 8 and Nf if Nf >= 8.  Also,
15 >= Nf >= 1.
	}
	version { $Id$  }
	author { Luis Gutierrez  }
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Find the reciprocal of a fraction in terms of a fraction and some left shifts.
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
		type { int }
	}
	state {
		name {Nf}
		type {int}
	        default {8}
	        desc {precision}
	}

	constructor {
		noInternalState();
	}

	codeblock (Rblock,"") {
	setc    ovm	; enable sat arithmetic
	lar     ar2,#$addr(input)
	lar     ar1,#0001h
	mar     *,ar2
	lacc    *,16,ar1
	abs             ; take absolute value
	rpt     #13
	norm    *+      ; normalize input so that it's >= 0.5
	lar     ar4,#$addr(f)
	mar     *,ar2
	bit     *,0,ar4 ; test sign of input, TC=1 if input < 0
	sach    *,0     ; store normalize denom in f
	lacc    #4000h,15 ; acc = 0.25
	.if	@(int(Nf) > 1)
	rpt     #@(int(Nf) - 1)
	.endif
	subc    *
	.if	@(int(Nf) < 8)
	sacl	*
	lacc	*,@(15 - int(Nf))
	sacl	*
	.else
	sacl	*,@(15 - int(Nf))
	.endif 
	lacc    *,16
	add     *,16
	smmr    ar1,#$addr(s) ; output shifts
	xc      1,TC
	neg		      ; negate if input was negative
	sach    *,0	      ; output result
	clrc	ovm	; disable sat arith(default)
	}

 	go {
	 	 addCode(Rblock());
	}

	exectime {
		return (21 + int(Nf) );
	}
}
