defstar {
	name {Not}
	domain {BDF}
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc { Output the logical inverse of the Boolean input }
	version { $Id$ }
	location { BDF main library }
	explanation {
This star is a logical inverter; it converts TRUE (non-zero) values on
input to FALSE (zero) values on output and vice versa.  Furthermore,
the scheduler can use this information to deduce properties of boolean
signals.
	}
	input {
		name{input}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	constructor {
		output.setBDFParams (1, input, BDF_COMPLEMENT);
	}
	go {
		output%0 << !int(input%0);
	}
}

