defstar {
	name { Thresh }
	domain { CGC }
	desc {
Compares input values to "threshold" (default 0.5).
Output is 0 if input <= threshold, otherwise it is 1.
	}
	version {$Id$}
	author { J. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		$ref(output) = ($ref(input) > $val(threshold));
	}
	go {
		addCode(gen);
	}
}
