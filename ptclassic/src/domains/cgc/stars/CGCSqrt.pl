defstar {
	name {Sqrt}
	domain { CGC }
	desc { This star computes the square root of its input.  }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC nonlinear library }
	explanation {
.Id "square root"
	}
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
		addCode(ln);
	}
   codeblock(ln) {
	$ref(output) = sqrt($ref(input));
   }
	exectime {
		return 165;	/* value taken from CG96Sqrt */
	}
}
