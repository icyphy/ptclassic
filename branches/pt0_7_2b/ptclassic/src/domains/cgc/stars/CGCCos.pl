defstar {
	name {Cos}
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
	initCode {
		addInclude("<math.h>");
	}
	go {
		gencode(cosgen);
	}
   codeblock(cosgen) {
	$ref(output) = cos($ref(input));
   }
}

		