defstar {
	name {Log}
	domain { CGC }
	desc { Outputs natural log of input. }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CGC nonlinear library }
	explanation {
Outputs natural log of input.
If the input is zero or negative, the constant -100.0 is produced.
.Id "logarithm"
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
	if($ref(input) <= 0) $ref(output) = -100.0;
	else $ref(output) = log($ref(input));
   }
}
