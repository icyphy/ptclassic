defstar {
	name { Thresh }
	domain { CGC }
	desc {
Compares input values to "threshold" (default 0.5).  Output is 0
if input is greater than or equal to threshold; otherwise, it is 1.
	}
	version {@(#)CGCThresh.pl	1.6 03/20/98}
	author { J. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name { threshold }
		type { float }
		default {0.5}
		desc { Threshold applied to input. }
	}
	constructor { noInternalState();}
	codeblock (gen) {
		$ref(output) = ($ref(input) > $val(threshold)) ? 1 : 0;
	}
	go {
		addCode(gen);
	}
}
