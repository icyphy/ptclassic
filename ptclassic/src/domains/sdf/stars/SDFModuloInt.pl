defstar {
	name {ModuloInt}
	domain {SDF}
	desc { 
The output is equal to the remainder after dividing the
integer input by the integer "modulo" parameter.
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
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	defstate {
		name{modulo}
		type{int}
		default{"10"}
		desc {The modulo parameter}
	}
	go {
	   output%0 << int(input%0) % int(modulo);
	}
}
