defstar {
	name { ReadFileInt }
	domain { CG56 }
	desc { Reads data from file for use by simulator.}
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Ir "Motorola DSP56000 simulator"
.Ir "simulator, Motorola DSP56000"
.Id "file input"
Reads data from file for use by Motorola DSP56000 simulator.
	}
	execTime {
		return 2;
	}
	output {
		name {output}
		type {INT}
	}
	state {
		name { fileName }
		type { STRING }
		default { "infile" }
		desc { 'Root' of filename that gets the data. '.sim' is appended.}
	}
	state {
		name { inVal}
		type { INT }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
		default { "0"}
	}

	// this codeblock causes the simulator to read from a file into
	// a memory location each time it is referenced.

	codeblock (logIn) {
input $ref(inVal) $val(fileName).sim -RD
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
		gencode(copy);
	}
}



