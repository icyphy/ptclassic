defstar {
	name {Sqrt}
	domain { CGC }
	desc { This star computes the square root of its input.  }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
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
}
