defstar {
	name { Trainer }
	domain { CGC }
	desc {
Passes the "train" input to the output for the first "trainLength"
samples, then passes the "decision" input to the output.  Designed
for use in decision feedback equalizers, but can be used for other
purposes.
	}
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
.Id "DFE training"
.Id "decision feedback equalizer training"
.Id "equalizer, decision feedback, training"
	}
	defstate {
		name { trainLength }
		type { int }
		default { 100 }
		desc { Number of training samples to use }
	}
	input {
		name { train }
		type { anytype }
	}
	input {
		name { decision }
		type { =train }
	}
	output {
		name { output }
		type { = train }
	}
	state {
		name { count }
		type { int }
		default { "0" }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
		desc { local variable for counting inputs }
	}
	constructor {
		noInternalState();
	}
	setup {
		count = 0;
	}
	codeblock(out) {
	if ($ref(count) < $val(trainLength)) {
		$ref(output) = $ref(train);
		$ref(count)++;
	} else {
		$ref(output) = $ref(decision);
	}
	}
	go {
		addCode(out);
	}
	exectime {
		return 1;
	}
}
