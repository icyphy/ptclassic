defstar {
	name { ReadFile }
	domain { C50 }
	desc { Reads data from file for use with simulator. }
	version { @(#)C50ReadFile.pl	1.6	02/03/99 }
	author { A. Baensch }
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
Reads one value data from the <i>fileName</i> for use with
Texas Instruments DSK320C5x simulator.
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
		name { inVal }
		type { fix }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
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
