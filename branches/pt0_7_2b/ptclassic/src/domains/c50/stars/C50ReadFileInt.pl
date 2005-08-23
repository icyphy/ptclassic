defstar {
	name { ReadFileInt }
	domain { C50 }
	desc { Reads data from file for use by simulator.}
	version { @(#)C50ReadFileInt.pl	1.4	01 Oct 1996 }
	author { A.Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="TI DSK320C5x simulator"></a>
<a name="simulator, TI DSK320C5x"></a>
<a name="file input"></a>
Reads data from file for use by TI DSK320C5x simulator.
	}
	output {
		name {output}
		type {INT}
	}
	state {
		name { fileName }
		type { STRING }
		default { "infile" }
		desc {
'Root' of filename that gets the data. '.sim' is appended
		}
	}
	state {
		name { inVal}
		type { INT }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
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

	codeblock (copy) {
	mar	*,AR6
	lar	AR6,#$addr(inVal)
	sacl	*,#$addr(output)
	}

	go {
		addCode(copy);
	}

	execTime {
		return 2;
	}
}
