defstar {
	name { Test }
	domain { CGC }
	version { @(#)CGCTest.pl	1.5	01 Oct 1996 }
	author { Rolando Diesta, Brian L. Evans, and Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc {
This star compares its two inputs "upper" and "lower".
The test "condition" can be any one of {EQ NE GT GE},
or equivalently any one of {== != > >=},
whose elements represent the binary operations of equals, not equals,
greater than, and greater than or equals, respectively.

If the "crossingsOnly" parameter is TRUE, then the output is TRUE only
when the outcome of the test changes from TRUE to FALSE
or FALSE to TRUE.
In this case, the first output is always TRUE.
	}
	htmldoc {
To implement the tests "&lt;" or "&lt;=", simply reverse the inputs.
	}
	input {
		name { upper }
		type { float }
		desc { Left-hand side of the test }
	}
	input {
		name { lower }
		type { float }
		desc { Right-hand side of the test }
	}
	output {
		name { output }
		type { int }
		desc { Result of the test }
	}
	defstate {
		name { condition }
		type { string }
		default { "EQ" }
		desc { The test condition, i.e, one of EQ, NE, LT, or LE }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc {
If TRUE, outputs are TRUE only when the sense of the test changes
		}
	}
	defstate {
		name { prevResult }
		type { int }
		default { "-1" }
		desc {
Initial value indicates that prevResult has not yet gotten the result of a
test.  This ensures that the first test result will always be TRUE.
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	private {
		StringList test;
	}
	code {
#define EQID "=="
#define NEID "!="
#define GTID ">"
#define GEID ">="
	}
        constructor {
		noInternalState();
	}
	setup {
		const char* cn = condition;
		if ( strcasecmp(cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp(cn, "NE") == 0 ) test = NEID;
		else if ( strcasecmp(cn, "GT") == 0 ) test = GTID;
		else if ( strcasecmp(cn, "GE") == 0 ) test = GEID;
		else if ( strcasecmp(cn, "==") == 0 ) test = EQID;
		else if ( strcasecmp(cn, "!=") == 0 ) test = NEID;
		else if ( strcasecmp(cn, ">") == 0 ) test = GTID;
		else if ( strcasecmp(cn, ">=") == 0 ) test = GEID;
		else Error::abortRun(*this, "Unrecognized test.", cn);
	}
	codeblock(decl) {
		int result;
	}
	codeblock(crossings) {
		$ref(output) = ( $ref(prevResult) != result );
		$ref(prevResult) = result;
	}
	go {
		StringList compare = "($ref(upper) ";
		compare << test << " $ref(lower)) ? 1 : 0;\n";

		if ( int(crossingsOnly) ) {
			addCode(decl);
			StringList setResult = "result = ";
			setResult << compare;
			addCode(setResult);
			addCode(crossings);
		}
		else {
			StringList simpleCode = "$ref(output) = ";
			simpleCode << compare;
			addCode(simpleCode);
		}
	}
}
