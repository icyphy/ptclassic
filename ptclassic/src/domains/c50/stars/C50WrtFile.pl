defstar {
	name { WrtFile }
	domain { C50 } 
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 io library }
	explanation {
.Ir "simulator, TI DSK320C5x"
.Ir "TI DSK320C5x simulator"
.Id "file output"
Writes data to a file, for use with the TI DSK320C5x simulator.
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
		default { "0"}
	}
	// this codeblock produces code
	codeblock (copy) {
	mar	*,AR6
	lar	AR6,#$addr(input)
	bldd	*,#$addr(outVal)
	}
        initCode {
		StringList logOut = "output $ref(outVal) ";
		if (fileName.null()) 
			logOut << "$starSymbol(/tmp/cgwritefile) ";
		else
			logOut << "$val(fileName) ";
		logOut << (strcmp(input.resolvedType(),INT)?"-RF":"-RD");
		addCode(logOut,"aioCmds");
	}
	go {
		addCode(copy);
	}
}



