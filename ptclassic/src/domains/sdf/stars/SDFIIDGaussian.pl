ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt
 Date of creation: 1/1/90
 Converted to preprocessor, and common AGC object used, by JTB 10/3/90

 Gaussian generates a sequence of IID pseudo-independent Gaussian
 variables represented as type FLOAT

 This Star uses the GNU library <Normal.h>

**************************************************************************/
}
defstar {
	name { IIDGaussian }
	domain { SDF }
	desc {
		"Generates a white Gaussian pseudo-random process with mean\n"
		"'mean' (default 0) and variance 'variance' (default 1)."
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { mean }
		type { float }
		default { "0.0" }
		desc { "mean of distribution" }
	}
	defstate {
		name { variance }
		type { float }
		default { "1.0" }
		desc { "variance of distribution" }
	}
	hinclude { <ACG.h>, <Normal.h> }
	protected {
		static ACG gen;
		Normal *random;
	}
// declare the static random-number generator in the .cc file
	code {
		ACG SDFIIDGaussian::gen(10,20);
	}
	constructor {
		random = new Normal(mean,variance,&gen);
	}
	destructor {
		delete random;
	}
	start {
		random->mean(double(mean));
		random->variance(double(variance));
	}
        go {
		output%0 << (float)(*random)();
	}
}

