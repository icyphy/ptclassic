defstar {
	name { WrtFile }
	domain { CG56 }
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { $Id$ }
	author { J. Buck, Chih-Tsung Huang, Jose L. Pino }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 io library }
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
		type {ANYTYPE}
	}
	state {
		name { fileName }
		type { STRING }
		default { "" }
		desc { 'Root' of filename that gets the data.}
	}
	state {
		name { outVal}
		type { FIX }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
		default { "0"}
	}
	// this codeblock produces code
	codeblock (copy) {
	move	$ref(input),a
	move	a,$ref(outVal)
	}
        initCode {
		StringList logOut = "output $ref(outVal) ";
		if (fileName.null()) 
			logOut << "$starSymbol(/tmp/cgwritefile) ";
		else
			logOut << "$val(fileName) ";
		logOut << (strcmp(input.resolvedType(),INT)?"-RF":"-RD");
		addCode(logOut,"simulatorCmds");
	}
	go {
		addCode(copy);
	}
}
