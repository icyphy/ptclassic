ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/8/90

 It is a parameterised random number generator.
 By setting a state, we can select the distribution function among
 (uniform, exponential, normal).

**************************************************************************/
}
defstar {
	name { RanGen }
	domain { DE }
	desc {	"It is a parameterized random number generator.\n"
		"We can select the distribution function by setting\n"
		"the state and parameters.\n"
		"distribution : uniform(u), exponential(e), normal(n).\n"
	}
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
		desc { "uniform(u;U), exponential(e;E), normal(n;N)" }
	}
	defstate {
		name { meanORupper }
		type { float }
		default { "0.0" }
		desc { "mean value or upper value" }
	} 
	defstate {
		name { varianceORlower }
		type { float }
		default { "0.0" }
		desc { "variance or lower value" }
	}
	hinclude { <Random.h> }
	ccinclude { <ACG.h>, <Uniform.h>, <Normal.h>, <NegativeExpntl.h> }
	protected {
		Random *random;
	}
	code {
		extern ACG gen;
	}
	destructor {
		delete random;
	}
	start {
		// decide which distribution.
		const char* dist = distribution.getInitValue();
		char  c = dist[0];
		float mOru = double(meanORupper);	// mean or upper
		float vOrl = double(varianceORlower);	// variance or lower

		switch (c) {
			case 'u' :
			case 'U' : random = new Uniform(vOrl, mOru, &gen);
			   	   break;
			case 'e' :
			case 'E' : random = new NegativeExpntl(mOru, &gen);
			   	   break;
			case 'n' :
			case 'N' : random = new Normal(mOru, vOrl, &gen);
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
		output.put(completionTime) << float(p);
	}
}

