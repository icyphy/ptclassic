defstar {
	name { ReadFile }
	domain { C50 }
	desc { Reads data from file for use with simulator. }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Ir "TI DSK320C5x simulator"
.Ir "simulator, TI DSK320C5x"
.Id "file input"
Reads one value data from the \fIfileName\fR for use with
Texas Instruments DSK320C5x simulator.
The \fIinVal\fR is used as the storage location of the read data value.
	}
	output {
		name {output}
		type {fix}
	}
	state {
		name { fileName }
		type { STRING }
		default { "infile" }
		desc {
'Root' of filename that gets the data. '.sim' is appended.
		}
	}
	state {
		name { inVal}
		type { fix }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
		default { "0"}
	}

	// this codeblock causes the simulator to read from a file into
	// a memory location each time it is referenced.
	codeblock (logIn) {
input $ref(inVal) $val(fileName).sim -RF
	}

	initCode {
		addCode(logIn, "simulatorCmds");
	}

	codeblock (copy) {
	mar	*,AR6
	lar	AR6,#$addr(inVal)
	bldd	*,#$addr(output)
	}

	go {
		addCode(copy);
	}

	execTime {
		return 2;
	}
}
