defstar {
	name {Exp}
	domain { CGC }
	desc { Outputs the exponential function of the input. }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC nonlinear library }
	htmldoc {
<b>Bugs:</i> Overflow is not handled well.
<a name="exponential"></a>
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
	exectime {
		return 33;  /* use the same value as log */
	}
}
