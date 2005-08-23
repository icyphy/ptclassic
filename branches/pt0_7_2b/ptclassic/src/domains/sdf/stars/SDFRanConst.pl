defstar {
	name { RanConst }
	domain { SDF }
	desc {
Generate an random number with a uniform(u), exponential(e), or normal(n)
distribution, as determined by the "distribution" parameter.
Output is this random number constantly.
	}
	version { $Id$ }
	author { Bilung Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This Star uses the GNU library <Uniform.h>.
.Id "uniform noise"
.Id "noise, uniform"
	}

	output {
		name { output }
		type { float }
	}
	defstate {
		name { distribution }
		type { string }
		default { "uniform" }
		desc {  Uniform(u;U), exponential(e;E), normal(n;N). }
	}
	defstate {
		name { meanORupper }
		type { float }
		default { "0.0" }
		desc { Mean value or upper value. }
	} 
	defstate {
		name { varianceORlower }
		type { float }
		default { "0.0" }
		desc { Variance or lower value. }
	}
	hinclude { <Random.h> }
	ccinclude { <ACG.h>, <Uniform.h>, <Normal.h>, <NegExp.h> }

	protected {
		double random;
	}
	code {
		extern ACG* gen;
	}

	setup {
		// decide which distribution.
		const char* dist = distribution;
		char  c = dist[0];
		double mOru = double(meanORupper);	// mean or upper
		double vOrl = double(varianceORlower);	// variance or lower

		Random *randomGen;
		switch (c) {
			case 'u' :
			case 'U' : randomGen = new Uniform(vOrl, mOru, gen);
			   	   break;
			case 'e' :
			case 'E' : randomGen = new NegativeExpntl(mOru, gen);
			   	   break;
			case 'n' :
			case 'N' : randomGen = new Normal(mOru, vOrl, gen);
			   	   break;
			default :
			  Error::abortRun(*this, "unknown distribution.");
			  break;
		}			

		// generate a random number
		random = (*randomGen)();
		
		delete randomGen;
	}

        go {
		output%0 << random;
	}
}

