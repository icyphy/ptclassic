defstar {
	name { ReadFile }
	domain { CG56 }
	desc { Reads data from file for use by simulator.}
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Reads data from file for use by simulator.
	}
	execTime {
		return (output.bufSize() >= 1) ? 2 : 0;
	}
	output {
		name {output}
		type {fix}
	}
	state {
		name { fileName }
		type { STRING }
		default { "infile" }
		desc { 'Root' of filename that gets the data. '.sim' is appended.}
	}
	state {
		name { inVal}
		type { fix }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
		default { "0"}
	}
	start {
		if (output.bufSize() >= 1) {
			// these attributes allocate memory
			inVal.setAttributes(A_YMEM|A_NOINIT);
		}
	}
	// this codeblock tells the simulator to log writes to the
	// outVal state, which works when the buffersize is >= 1.

	codeblock (logIn) {
input $ref(inVal) $val(fileName).sim -RF
}
	initCode {
                genMiscCmd(logIn);
	}

	// this codeblock produces code
	codeblock (copy) {
	move	$ref(inVal),a
	move	a,$ref(output)
	}
	go {
		if (output.bufSize() >= 1) gencode(copy);
	}
}



