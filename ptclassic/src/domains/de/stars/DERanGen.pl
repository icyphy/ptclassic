defstar {
	name { RanGen }
	domain { DE }
	desc {
This is a parameterized random number generator.
We can select the distribution function by setting the state and parameters.
Distributions currently supported : uniform(u), exponential(e), normal(n).
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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
		random = NULL;
	}
	destructor {
		if(random) { LOG_DEL; delete random;}
	}
	start {
		if(random) { LOG_DEL; delete random;}

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

