defcore {
	name { Abs }
	domain { ACS }
	coreCategory { FPCGC }
	corona { Abs }
	desc { takes the absolute value of the input. }
	version { $Id$ }
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
		addCode("$ref(output) = ($ref(input) < 0.0) ? - $ref(input) : $ref(input);");
	}
}
