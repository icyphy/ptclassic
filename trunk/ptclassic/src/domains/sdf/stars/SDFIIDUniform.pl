defstar {
	name { IIDUniform }
	domain { SDF }
	desc {
Generates an i.i.d. uniformly distributed pseudo-random process.
Output is uniformly distributed between "lower" (default 0)
and "upper" (default 1).
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This Star uses the GNU library <Uniform.h>.
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { lower }
		type { float }
		default { "0.0" }
		desc { Lower limit. }
	}
	defstate {
		name { upper }
		type { float }
		default { "1.0" }
		desc { Upper limit. }
	}
	hinclude { <Uniform.h> }
	ccinclude { <ACG.h> }
	protected {
		Uniform *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		if(random) delete random;
	}
	start {
		if(random) delete random;
		random = new Uniform(double(lower),double(upper),gen);
	}
        go {
		output%0 << (float)(*random)();
	}
}

