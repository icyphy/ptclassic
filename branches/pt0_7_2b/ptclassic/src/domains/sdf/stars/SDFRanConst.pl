defstar {
	name { RanConst }
	domain { SDF }
	desc {
Generate an random number with a uniform(u), exponential(e), or normal(n)
distribution, as determined by the "distribution" parameter.
Output is this random number constantly.
	}
	version { @(#)SDFRanConst.pl	1.3 10/01/96 }
	author { Bilung Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This Star uses the GNU library &lt;Uniform.h&gt;.
<a name="uniform noise"></a>
<a name="noise, uniform"></a>
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
		Random *randomGen;
		double random;
	}
	code {
		extern ACG* gen;
	}
	constructor {
		randomGen = NULL;
	}
	destructor {
		delete randomGen;
	}

	setup {
		// decide which distribution.
		const char* dist = distribution;
		char  c = dist[0];
		double mOru = double(meanORupper);	// mean or upper
		double vOrl = double(varianceORlower);	// variance or lower

		delete randomGen;
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
	}

	begin {
	  // generate a random number
	  random = (*randomGen)();
	}

        go {
		output%0 << random;
	}
}

