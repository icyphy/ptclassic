defcore {
    name { Gain }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Gain } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { $Id$
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
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
