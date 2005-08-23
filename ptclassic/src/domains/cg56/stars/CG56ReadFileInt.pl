defstar {
	name { ReadFileInt }
	domain { CG56 }
	desc { Reads data from file for use by simulator.}
	version { @(#)CG56ReadFileInt.pl	1.12 03/29/97 }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="Motorola DSP56000 simulator"></a>
<a name="simulator, Motorola DSP56000"></a>
<a name="file input"></a>
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
                addCode(logIn,"simulatorCmds");
	}

	// this codeblock produces code
	codeblock (copy) {
	move	$ref(inVal),a
	move	a,$ref(output)
	}
	go {
		addCode(copy);
	}
}



