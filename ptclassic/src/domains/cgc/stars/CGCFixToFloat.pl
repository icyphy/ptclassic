defstar {
	name { FixToFloat }
	domain { CGC }
	version { $Id$ }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC conversion palette }
	desc { Convert a fixed-point input to a floating-point output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { float }
		desc { Output float type }
	}
	go {
@	$ref(output) = FIX_Fix2Double($ref(input));
	}
}
