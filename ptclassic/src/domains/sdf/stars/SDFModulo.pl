defstar {
	name {Modulo}
	domain {SDF}
	desc { 
The output is equal to the remainder after dividing
the input by the modulo the "modulo" parameter.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name{modulo}
		type{float}
		default{"1.0"}
		desc {The modulo parameter}
	}
	go {
	  double dummy;
	  output%0 << modf(double(input%0)/double(modulo), &dummy);
	}
}
