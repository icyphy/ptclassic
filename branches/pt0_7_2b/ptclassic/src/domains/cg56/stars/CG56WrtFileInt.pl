defstar {
	name { IntWriteFile }
	domain { CG56 }
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { $Id$ }
	author { Chih-Tsung Huang and Jose Pino }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
	}
	execTime {
		return (input.bufSize() >= 1) ? 2 : 0;
	}
	input {
		name {input}
		type {int}
	}
	state {
		name { fileName }
		type { STRING }
		default { "outfile" }
		desc { 'Root' of filename that gets the data. '.sim' is appended.}
	}
	state {
		name { outVal}
		type { int }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
		default { "0"}
	}
	start {
		if (input.bufSize() >= 1) {
			// these attributes allocate memory
			outVal.setAttributes(A_YMEM|A_NOINIT);
		}
	}

	// this codeblock tells the simulator to log writes to the
	// outVal state, which works when the buffersize is >= 1.
	codeblock (logOut) {
output $ref(outVal) $val(fileName).sim -RD
}
	// this codeblock produces code
	codeblock (copy) {
	move	$ref(input),a
	move	a,$ref(outVal)
	}
        initCode {
		genMiscCmd(logOut);
	}
	go {
		if (input.bufSize() >= 1) gencode(copy);
	}
}