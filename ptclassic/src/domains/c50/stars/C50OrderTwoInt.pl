defstar {
	name { OrderTwoInt }
	domain { C50 }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { $Id$ }
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
* there's a bug in the dsk debugger because the following inst.
* produces the wrong result.
*	xc	2,LT			
* to work around it, replace inst. with two consecutive
* xc 1,LT instructions
	xc	1,LT
	mar	*+,ar0			
	xc	1,LT
	mar	*-,ar1	
	bldd	*,#$addr(greater),ar0
	bldd	*,#$addr(lesser)
	}

 	go {
		addCode(order);
	}

	exectime {
		return 12;	
	}
}
