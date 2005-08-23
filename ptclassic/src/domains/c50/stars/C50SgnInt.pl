defstar {
	name {SgnInt}
	domain {C50}
	desc {
Output +1 if the input is positive or zero, and 0 if the input is negative.
This definition of signum is not compatible with the Sgn stars.
	}
	version { @(#)C50SgnInt.pl	1.7	01 Oct 1996}
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}	
	location { C50 main library }
	htmldoc {
This star outputs the signum of its input.
<a name="signum"></a>
The output is either 0 or 1
	}
	execTime {
		return 7;
	}
	input {
		name{input}
		type{INT}
	}
	output {
		name{output}
		type{INT}
	}
	constructor {
		noInternalState();
	}
	codeblock (sgnblock) {
	lar	ar0,#$addr(input)
	mar	*,ar0
	lacc	*,16,ar1
	lar	ar1,#0000h
	xc	1,GEQ
	mar	*+
	smmr	ar1,#$addr(output)
	}

	go {
		addCode(sgnblock);
	}
}
