defstar {
	name {ModuloInt}
	domain {SDF}
	desc { 
The output is equal to the integer remainder after dividing the
integer input by the integer "modulo" parameter.
	}
	version {@(#)SDFModuloInt.pl	1.4   4/27/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
	setup {
                if ( int(modulo) == 0 ) {
		    Error::abortRun(*this,
				    "The modulo parameter cannot be zero");
		    return;
		}
	}
	go {
	   output%0 << int(input%0) % int(modulo);
	}
}
