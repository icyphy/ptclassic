defstar {
	name { IIDGaussian }
	domain {CGC}
	desc {
Generate random variables that are approximately independent identically
distributed Gaussian random variables.  The values range from "lower"
to "upper".
	}
	version { @(#)CGCIIDGaussian.pl	1.9	7/12/96 }
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
		name { var }
		type { float }
		default { 1.0 }
		desc { variance }
	}
	defstate {
		name { mean }
		type { float }
		default { 0.0 }
		desc { mean }
	}
	defstate {
		name { seed }
		type { int }
		default { 1 }
	}

	initCode {
		// Declare prototypes for srand and rand functions
		addInclude("<stdlib.h>");
		// Declare prototype for sqrt
		addInclude("<math.h>");
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
		int i;
		double sum = 0.0;
		/* Sum a large number of zero-mean random numbers that  */
		/* are uniformly distributed on the interval [-0.5,0.5] */
		/* to approximate a Gaussian distribution		*/
		for (i = 0; i < 27; i++) {
		    /* RAND_MAX is an ANSI C standard constant */
		    /* If not defined, then just use the lower 15 bits */
		    double randomValue = 0.0;
		    int randomInt = rand();
#ifdef RAND_MAX
		    randomValue = ((double) randomInt) / ((double) RAND_MAX);
#else
		    randomInt &= 0x7FFF;
		    randomValue = ((double) randomInt) / 32767.0;
#endif
		    sum += (randomValue - 0.5);
		}
		if ($val(var) != 1.0) sum *= sqrt($val(var));
		$ref(output) = (2.0/3.0) * sum + $val(mean);
	}

	exectime {
		return 300;
	}
}
