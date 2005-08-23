defstar {
	name { FixToInt }
	domain { CGC }
	derivedFrom { Fix }
	version { @(#)CGCFixToInt.pl	1.2 7/10/96 }
	author { Yu Kee Lim }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc { Convert a fixed-point input to an integer output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { int }
		desc { Output int type }
	}
        // a setup method is inherited from CGCFix
	// if you define your own, you should call CGCFix::setup()
        // an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()
	codeblock(fixtoint) {
	        $ref(output) = FIX_Fix2Int($ref(input));
	}
        go {
                addCode(fixtoint);
	}
        // a wrapup method is inherited from CGCFix
	// if you define your own, you should call CGCFix::wrapup()
}
