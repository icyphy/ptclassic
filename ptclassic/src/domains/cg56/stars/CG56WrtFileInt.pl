defstar {
	name { WrtFileInt }
	domain { CG56 }
	desc {
When run on the simulator, arranges for its input to be logged to a file.
	}
	version { @(#)CG56WrtFileInt.pl	1.13 03/29/97 }
	author { Chih-Tsung Huang and Jose Pino }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="simulator, Motorola DSP56000"></a>
<a name="Motorola DSP56000 simulator"></a>
<a name="file output"></a>
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
		addCode(logOut,"simulatorCmds");
	}
	go {
		addCode(copy);
	}
}
