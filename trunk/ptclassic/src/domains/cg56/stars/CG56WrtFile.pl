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
	location { CG56 main library }
	htmldoc {
<a name="simulator, Motorola DSP56000"></a>
<a name="Motorola DSP56000 simulator"></a>
<a name="file output"></a>
Writes data to a file, for use with the Motorola DSP56000 simulator.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	state {
		name { fileName }
		type { STRING }
		default { "" }
		desc { 'Root' of the filename that gets the data.}
	}
	state {
		name { outVal}
		type { FIX }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
		default { "0"}
	}
	codeblock (copy) {
	move	$ref(input),a
	move	a,$ref(outVal)
	}
	ccinclude { <Uniform.h>, <ACG.h> }
	protected {
		StringList logFileNameString;
	}
	// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
        setup {
		if (fileName.null()) {
                	Uniform random(0.0, 1.0, gen);
			int uniqueId = int(32000.0 * (random)());
			logFileNameString = "$starSymbol(";
			logFileNameString << "cgwritefile)_" << uniqueId;
		}
		else {
			logFileNameString = "$val(fileName)";
		}
        }
        initCode {
		StringList logOut = "output $ref(outVal) ";
		logOut << logFileNameString << " "
		       << (strcmp(input.resolvedType(),INT) ? "-RF" : "-RD");
		addCode(logOut, "simulatorCmds");
	}
	go {
		addCode(copy);
	}
	execTime {
		return 2;
	}
}
