defstar {
	name {Sin}
	domain { CGC }
	desc { This star computes the sine of its input, in radians.  }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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

		
