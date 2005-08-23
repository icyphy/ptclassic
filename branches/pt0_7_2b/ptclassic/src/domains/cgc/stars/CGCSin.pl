defstar {
	name {Sin}
	domain { CGC }
	desc { This star computes the sine of its input, in radians.  }
	version { @(#)CGCSin.pl	1.8	01/01/96 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	constructor {
		noInternalState();
	}
	initCode {
		addInclude("<math.h>");
	}
	go {
		addCode(singen);
	}
   codeblock(singen) {
	$ref(output) = sin($ref(input));
   }
	exectime {
		return 23;	/* value taken from CG96Sin */
	}
}

		
