ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/19/90
 Converted to use preprocessor, 10/3/90, JTB

Convert two numbers to magnitude and phase

**************************************************************************/
}
defstar {
	name { RectPolar }
	domain { SDF }
	desc {
		"Convert two numbers to magnitude and phase"
	}
	input {
		name { x }
		type { float }
	}
	input {
		name { y }
		type { float }
	}
	output {
		name { magnitude }
		type { float }
	}
	output {
		name { phase }
		type { float }
	}
	go {
		double xpart = x%0;
		double ypart = y%0;
		double m = sqrt(xpart*xpart + ypart*ypart);
		magnitude%0 << m;
		if (m == 0)
		   phase%0 << 0.0;
		else	
		   phase%0 << atan2(ypart,xpart);
	}
}
