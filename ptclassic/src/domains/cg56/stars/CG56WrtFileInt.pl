defstar {
	name { WriteFileInt }
	domain { CG56 }
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { $Id$ }
	author { Chih-Tsung Huang and Jose Pino }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Ir "simulator, Motorola DSP56000"
.Ir "Motorola DSP56000 simulator"
.Id "file output"
Writes data to a file, for use with the Motorola DSP56000 simulator.
	}
	execTime {
		return 2;
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
		default { "0"}
	}

	// this codeblock tells the simulator to log writes to the
	// outVal state.
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
		gencode(copy);
	}
}
