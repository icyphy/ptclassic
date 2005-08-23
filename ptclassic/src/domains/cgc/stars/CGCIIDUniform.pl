defstar {
	name { IIDUniform }
	domain {CGC}
	desc {
Generate random variables that are approximately independent identically
distributed uniform random variables.  The values range from "lower"
to "upper".
	}
	version { @(#)CGCIIDUniform.pl	1.10	7/12/96 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { lower }
		type { float }
		default { 0.0 }
		desc { lower limit of uniform random number generator }
	}
	defstate {
		name { upper }
		type { float }
		default { 1.0 }
		desc { upper limit of uniform random number generator }
	}
	defstate {
		name { seed }
		type { int }
		default { 1 }
	}

	setup {
		if ( double(lower) > double(upper) ) {
		    Error::abortRun(*this, "The upper limit must be greater ",
				    "than the lower limit");
		    return;
		}
	}

	initCode {
		// Pull in prototypes for srand and rand
		addInclude("<stdlib.h>");
		// Initialize the random number generator
		addCode(initSeed);
	}

	go {
		addCode(randomGen);
	}

	// initialize the seed of the random number generator
	codeblock(initSeed) {
/* Initialize the random number generator */
srand($val(seed));
	}

	// generate a random number
	codeblock(randomGen) {
		/* Generate a random number on the interval [0,1] and */
		/* map it into the interval [$val(lower),$val(upper)] */
		double randomValue = 0.0;
		int randomInt = rand();
		double scale = $val(upper) - ($val(lower));
		double center = ($val(upper) + ($val(lower)))/2.0;

		/* RAND_MAX is an ANSI C standard constant */
		/* If not defined, then just use the lower 15 bits */
#ifdef RAND_MAX
		randomValue = ((double) randomInt) / ((double) RAND_MAX);
#else
		randomInt &= 0x7FFF;
		randomValue = ((double) randomInt) / 32767.0;
#endif
		$ref(output) = scale * (randomValue - 0.5) + center;
	}

	exectime {
		return 10 + 5;	/* based on CG96IIDUniform */
	}
}
