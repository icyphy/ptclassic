ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: J. T. Buck
Date of creation: 8/3/90
Converted to use preprocessor, 9/26/90

DB star: output the dB value of the input, with a minimum.

************************************************************************/
}

defstar {
	name {DB}
	domain {SDF}
	desc {
		"Converts input to dB.  Zero and negative values are\n"
		"converted to 'min' (default -100)"
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name{min}
		type{float}
		default{"-100"}
		desc{"Minimum output value"}
	}
	go {
		float f = input%0;
		if (f <= 0.0)
			output%0 << double(min);
		else {
			f = 20.0 * log10 (f);
			if (f < double(min)) f = double(min);
			output%0 << f;
		}
	}
}
