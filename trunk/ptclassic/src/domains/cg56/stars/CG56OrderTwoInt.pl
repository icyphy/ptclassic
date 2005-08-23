defstar {
	name { OrderTwoInt }
	domain { CG56 }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { @(#)CG56OrderTwoInt.pl	1.5	3/27/96 }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
	move	$ref(lower),a		; a = lower input
	move	$ref(upper),b		; b = upper input
	cmp	a,b	a,x0		; compare upper to lower, x0 = lower
	tlt	b,a			; if upper(b) < lower(a) swap b and a
	tlt	x0,b
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
