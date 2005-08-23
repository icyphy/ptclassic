defstar {
	name { OrderTwoInt }
	domain { SDF }
	desc {
Takes two inputs and outputs the greater and lesser of the two integers.
	}
	version { @(#)SDFOrderTwoInt.pl	1.1	2/13/96 }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		int greaterValue = int(upper%0);
		int lesserValue = int(lower%0);

		if ( greaterValue < lesserValue ) {
		    int swap = greaterValue;
		    greaterValue = lesserValue;
		    lesserValue = swap;
		}

		greater%0 << greaterValue;
		lesser%0 << lesserValue;
	}
}
