defcore {
    name { Const }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Const } 
    version { $Id$ }
    desc { Output the constant level as a floating-point value. }
    version { @(#)ACSConstFPCGC.pl	1.1	05/07/98 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	go { addCode(std); }
	codeblock (std) {
	$ref(output) = $ref(level);
	}
	exectime {
		return 1;
	}
}
