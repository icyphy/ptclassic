defcore {
    name { IntGain }
    domain { ACS }
    coreCategory { FPCGC }
    corona { IntGain } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { $Id$}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	constructor {
		noInternalState();
	}
	go {
		addCode("$ref(output) = $val(gain) * $ref(input);\n");
	}
	exectime {
		return 1;
	}
}
