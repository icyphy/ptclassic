ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt
 Date of creation: 5/26/90
 Converted to use preprocessor, 10/3/90, JTB

 FloatThresh applies a threshold to its input -- output is binary {0,1}
 The threshold is a parameter, defaults to 0.5

**************************************************************************/
}
defstar {
	name { FloatThresh }
	domain { SDF }
	desc {
		"Compares input values to 'threshold' (default 0.5).\n"
		"Output is 0 if input <= threshold, else 1."
	}
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name { threshold }
		type { float }
		default {0.5}
		desc { "threshold applied to input" }
	}
	go {
		if( float(input%0) <= double(threshold) )
			output%0 << 0;
		else output%0 << 1;
	}
}
