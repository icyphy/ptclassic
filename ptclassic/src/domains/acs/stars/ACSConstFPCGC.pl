defcore {
    name { Const }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Const } 
    desc { Output the constant level as a floating-point value. }
    version { @(#)ACSConstFPCGC.pl	1.6 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
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
