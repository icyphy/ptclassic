defcore {
    name { Const }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Const } 
    desc { Output the constant level as a floating-point value. }
    version { $Id$ }
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