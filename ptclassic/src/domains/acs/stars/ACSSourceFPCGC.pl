defcore {
    name { Source }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Source } 
    desc { Generic code generator source star. }
    version {$Id$}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	go { addCode(std); }
	codeblock (std) {
	$ref(output) = $ref(0.0);
	}
	exectime {
		return 2;
	}

}
