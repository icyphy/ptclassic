defstar {
	name {Sin}
	domain { CGC }
	desc { This star computes the sine of its input, in radians.  }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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
}

		
