defstar {
	name { IIDGaussian }
	domain { SDF }
	desc {
Generates a white Gaussian pseudo-random process with mean
"mean" (default 0) and variance "variance" (default 1).
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This Star uses the GNU library <Normal.h>
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { mean }
		type { float }
		default { "0.0" }
		desc {  Mean of distribution. }
	}
	defstate {
		name { variance }
		type { float }
		default { "1.0" }
		desc { Variance of distribution. }
	}
	hinclude { <Normal.h> }
	ccinclude { <ACG.h> }
	protected {
		Normal *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		LOG_DEL; delete random;
	}
	start {
		LOG_DEL; delete random;
		LOG_NEW; random = new Normal(double(mean),double(variance),gen);
	}
        go {
		output%0 << (*random)();
	}
}

