defcore {
	name { NL }
	domain { ACS }
	coreCategory { FPCGC }
	corona { NL }
	desc {  }
	version { @(#)ACSNLFPCGC.pl	1.2 09/08/99 }
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
	codeblock(main) {
		if ( $ref(input) < $val(threshold1) ) 
			$ref(output) = 0.0;
		else
			$ref(output) = $val(gain) * $ref(input);
		if ( $ref(output) > double(threshold2) )
			$ref(output) = double(threshold2);
	}
	go {
		addCode(main);
	}
}
