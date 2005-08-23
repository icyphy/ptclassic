defstar {
	name { OrderTwoInt }
	domain { CG56 }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
	move	$ref(lower),y0			; y0 = lower input
	move	$ref(upper),x0	y0,a		; x0 = upper input
	cmp	x0,a	x0,b			; compare upper to lower
	tlt	x0,a			; a holds lesser value
	tlt	y0,b			; b holds greater value
	move	b,$ref(greater)
	move	a,$ref(lesser)
	}

 	go {
		addCode(order);
	}

	exectime {
		return 7;	
	}
}
