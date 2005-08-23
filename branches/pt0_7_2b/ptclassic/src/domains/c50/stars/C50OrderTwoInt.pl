defstar {
	name { OrderTwoInt }
	domain { C50 }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { @(#)C50OrderTwoInt.pl	1.6	7/22/96 }
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
	lar	ar0,#0012h		
	setc	ovm			
	mar	*,ar0			
	bldd	#$addr(upper),*+	
	bldd	#$addr(lower),*-
	lacc	*+,16		
	sub	*,16,ar1
	lar	ar1,#0012h		
	xc	2,LT
	mar	*+,ar0			
	mar	*-,ar1	
	bldd	*,#$addr(greater),ar0
	bldd	*,#$addr(lesser)
	clrc	ovm
	}

 	go {
		addCode(order);
	}

	exectime {
		return 13;	
	}
}
