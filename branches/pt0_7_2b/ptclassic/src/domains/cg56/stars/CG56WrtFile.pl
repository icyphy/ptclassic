defstar {
	name { WriteFile }
	domain { CG56 }
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star relies on a feature of the Sim56Target, which captures code lines
beginning with "!" and uses them as commands for the simulator.
	}
	execTime {
		return (input.bufSize() > 1) ? 2 : 0;
	}
	input {
		name {input}
		type {FIX}
	}
	state {
		name { fileName }
		type { STRING }
		default { "outfile" }
		desc { 'Root' of filename that gets the data. '.sim' is appended.}
	}
	state {
		name { outVal}
		type { FIX }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
		default { "0"}
	}
	start {
		if (input.bufSize() > 1) {
			// these attributes allocate memory
			outVal.setAttributes(A_YMEM|A_NOINIT);
		}
	}
	// this codeblock tells the simulator to log writes to the
	// input, which works when the buffersize is 1.
	codeblock (logIn) {
!output $ref(input) $val(fileName).sim -RF
}
	// this codeblock tells the simulator to log writes to the
	// outVal state, which works when the buffersize is > 1.
	codeblock (logOut) {
!output $ref(outVal) $val(fileName).sim -RF
}
	initCode {
		if (input.bufSize() == 1) gencode(logIn);
		else gencode(logOut);
	}
	// this codeblock produces code
	codeblock (copy) {
	move	$ref(input),a
	move	a,$ref(outVal)
	}
	go {
		if (input.bufSize() > 1) gencode(copy);
	}
}