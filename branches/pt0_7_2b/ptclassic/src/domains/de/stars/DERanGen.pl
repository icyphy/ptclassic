defstar {
	name { RanGen }
	domain { DE }
	desc {
This is a parameterized random number generator.  Upon receiving an input event,
it generates a random number with a uniform(u), exponential(e), or normal(n)
distribution, as determined by the "distribution" parameter.  Depending on
the distribution, parameters also specify either the mean and variance or
the lower and upper extent of the range.
	}
	version { @(#)DERanGen.pl	1.12	6/20/95}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	input {
		name { input }
		type { anytype }
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
		Random *random;
	}
	code {
		extern ACG* gen;
	}
	constructor {
		random = 0;
	}
	destructor {
		LOG_DEL; delete random;
	}
	setup {
		LOG_DEL; delete random;

		// decide which distribution.
		const char* dist = distribution;
		char  c = dist[0];
		double mOru = double(meanORupper);	// mean or upper
		double vOrl = double(varianceORlower);	// variance or lower

		switch (c) {
			case 'u' :
			case 'U' : LOG_NEW; random = new Uniform(vOrl, mOru, gen);
			   	   break;
			case 'e' :
			case 'E' : LOG_NEW; random = new NegativeExpntl(mOru, gen);
			   	   break;
			case 'n' :
			case 'N' : LOG_NEW; random = new Normal(mOru, vOrl, gen);
			   	   break;
			default :
			  Error::abortRun(*this, "unknown distribution.");
			  break;
		}			
	}
	go {
		// generate a random number
		double p = (*random)();
		// output
		completionTime = arrivalTime;
		output.put(completionTime) << p;
	}
}

