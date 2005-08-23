defstar {
	name {Const}
	domain {CGC}
	desc { Output a constant signal with value level (default 0.0).  }
	version { @(#)CGCConst.pl	1.6	1/1/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	exectime {
		return 0;	
	}
}

