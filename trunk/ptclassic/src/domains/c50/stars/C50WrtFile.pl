defstar {
	name { WrtFile }
	domain { C50 } 
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { @(#)C50WrtFile.pl	1.5	02/03/99 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="simulator, TI DSK320C5x"></a>
<a name="TI DSK320C5x simulator"></a>
<a name="file output"></a>
Writes data to a file, for use with the TI DSK320C5x simulator.
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
		name { outVal }
		type { FIX }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
		default { "0" }
	}
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
	execTime {
		return 2;
	}
}
