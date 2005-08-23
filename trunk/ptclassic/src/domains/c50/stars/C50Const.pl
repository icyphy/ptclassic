defstar {
	name { Const }
	domain { C50 }
	desc { Constant source }
	version {@(#)C50Const.pl	1.7	05/26/98}
	author { A. Baensch, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
There are no runtime instructions associated with this star.  The
output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {fix}
		attributes{P_NOINIT}
	}
	state {
		name {level}
		type {fix}
		default {"1"}
		descriptor {Constant value to be output}
		attributes {  A_UMEM | A_SETTABLE | A_CONSTANT}
	}

	constructor {
		noInternalState();
	}

	
	execTime {
		return 0;
	}
}
