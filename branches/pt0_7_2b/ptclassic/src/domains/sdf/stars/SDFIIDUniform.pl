ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt
 Date of creation: 1/1/90
 Converted to preprocessor, and common AGC object used, by JTB 10/3/90

 Uniform generates a sequence of IID pseudo-independent Uniform
 variables represented as type FLOAT

 This Star uses the GNU library <Uniform.h>

**************************************************************************/
}
defstar {
	name { IIDUniform }
	domain { SDF }
	desc {
	    "Generates an i.i.d. uniformly distributed pseudo-random process.\n"
	    "Output is uniformly distributed between 'lower' (default 0)\n"
	    "and 'upper' (default 1)."
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { lower }
		type { float }
		default { "0.0" }
		desc { "lower limit" }
	}
	defstate {
		name { upper }
		type { float }
		default { "1.0" }
		desc { "upper limit" }
	}
	hinclude { <ACG.h>, <Uniform.h> }
	protected {
		static ACG gen;
		Uniform *random;
	}
// declare the static random-number generator in the .cc file
	code {
		ACG SDFIIDUniform::gen(10,20);
	}
	constructor {
		random = new Uniform(lower,upper,&gen);
	}
	destructor {
		delete random;
	}
	start {
		random->low(double(lower));
		random->high(double(upper));
	}
        go {
		output%0 << (float)(*random)();
	}
}

