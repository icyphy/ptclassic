defstar {
	name {Exp}
	domain { CGC }
	desc { Outputs the exponential function of the input. }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC nonlinear library }
	explanation {
\fBBugs:\fR Overflow is not handled well.
.Id "exponential"
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
		addCode(exp);
	}
   codeblock(exp) {
	$ref(output) = exp($ref(input));
   }
}
