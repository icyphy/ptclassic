defstar {
	name { OrderTwoInt }
	domain { C50 }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { @(#)C50OrderTwoInt.pl	1.5	3/27/96 }
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	input {
		name {upper}
		type {int}
	}
	input {
		name {lower}
		type {int}
}
	output {
		name {greater}
		type {int}
	}
	output {
		name {lesser}
		type {int}
	}

	codeblock(order) {
	lar	ar0,#0012h		; lesser -> ar0 -> ar2
	lar	ar1,#0012h		; greater ->ar1 -> ar2
	mar	*,ar0			; arp =0
	bldd	#$addr(upper),*+	; ar2 = upper; ar0->ar2
	bldd	#$addr(lower),*		; ar3 = lower; ar0 -> ar3
	lamm	ar2			; acc = ar2 = upper
	sub	*			; acc -= ar3 = lower
	nop				; wait until conds. are stable
	xc	2,LT			; if upper > lower 
	mar	*-,ar1			; xchange ar0, ar1
	mar	*+			; arp = 1
	bldd	*,#$addr(greater),ar0
	bldd	*,#$addr(lesser)
	}

 	go {
		addCode(order);
	}

	exectime {
		return 7;	
	}
}
