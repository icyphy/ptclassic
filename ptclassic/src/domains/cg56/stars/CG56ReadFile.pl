defstar {
	name { ReadFile }
	domain { CG56 }
	desc { Reads data from file for use with simulator.}
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="Motorola DSP56000 simulator"></a>
<a name="simulator, Motorola DSP56000"></a>
<a name="file input"></a>
Reads one data value from the <i>fileName</i> for use with the
Motorola DSP56000 simulator.
The <i>inVal</i> is used as the storage location of the read data value.
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
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
	move	$ref(inVal),a
	move	a,$ref(output)
	}

	go {
		addCode(copy);
	}

	execTime {
		return 2;
	}
}
