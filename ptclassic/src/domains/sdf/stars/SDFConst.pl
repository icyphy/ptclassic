ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/19/90
 Converted to use preprocessor, 10/3/90

 This star produces a float DC output (default zero)

**************************************************************************/
}
defstar {
	name {FloatDC}
	domain {SDF}
	desc {
		"Output a constant signal with value level (default 0.0)"
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {level}
		type{float}
		default {"0.0"}
		desc {"DC value"}
	}
	go {
		output%0 << double(level);
	}
}

