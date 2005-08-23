defstar {
	name { OrderTwoInt }
	domain { CGC }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { @(#)CGCOrderTwoInt.pl	1.1 5/1/96 }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	go {
	        addCode(ordertwoint);
	}
 	codeblock(ordertwoint) {
		int greaterValue = (int)$ref(upper);
		int lesserValue = (int) $ref(lower);

		if ( greaterValue < lesserValue ) {
		    int swap = greaterValue;
		    greaterValue = lesserValue;
		    lesserValue = swap;
		}

		$ref(greater) = greaterValue;
		$ref(lesser) = lesserValue;
	}
}
