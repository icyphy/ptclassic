defstar {
	name {Const}
	domain {CGC}
	desc { Output a constant signal with value level (default 0.0).  }
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name {level}
		type{float}
		default {"0.0"}
		desc {The constant value.}
	}
	constructor {	
		noInternalState();
	}
	go {
		addCode("\t$ref(output) = $val(level);\n");
	}
}

