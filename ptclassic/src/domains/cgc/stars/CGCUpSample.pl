defstar {
	name { UpSample }
	domain { CGC }
	desc { 
Upsample by a factor (default 2), filling with fill (default 0.0).  The
"phase" tells where to put the sample in an output block.  The default
is to output it first (phase = 0). The maximum phase is "factor" - 1.
	}
	version { $Id$ }
	author { E. A. Lee and S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced. }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block. }
	}
	state {
		name {fill}
		type {float}
		default {0.0}
		desc { Value to fill the output block. }
	}
	state {
		name {ix}
		type {int}
		default {0}
		attributes { A_NONSETTABLE }
	}
	setup {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	constructor {
		noInternalState();
	}
	codeblock (sendOne) {
	$ref2(output,ix) = $ref(input);
	}
	codeblock (sendAll) {
	int i;
	for (int i = 0; i < $val(factor); i++) {
		$ref2(output,i) = $val(fill);
	}
	$ref2(output,ix) = $ref(input);
	}
	go {
		ix = int(factor) - int(phase) - 1;
		if (output.staticBuf() && output.linearBuf())
			addCode(sendOne);
		else
			addCode(sendAll);
	}
}
