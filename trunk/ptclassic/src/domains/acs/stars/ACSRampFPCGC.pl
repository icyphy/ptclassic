defcore {
    name { Ramp }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Ramp } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSRampFPCGC.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	go { addCode(std); }
	codeblock (std) {
	$ref(output) = $ref(value);
	$ref(value) += $val(step);
	}
	exectime {
		return 2;
	}

}
