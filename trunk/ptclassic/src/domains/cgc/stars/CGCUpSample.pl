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
	copyright { 1992 The Regents of the University of California }
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
	start {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (sendsample) {
	$ref2(output,phase) = $ref(input);
	}
	go {
		gencode(sendsample);
	}
}
