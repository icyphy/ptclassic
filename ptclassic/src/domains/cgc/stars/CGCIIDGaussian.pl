defstar {
	name { IIDGaussian }
	domain {CGC}
	desc {
Generate pseudo-IID-Gaussian random variables.  
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1992 The Regents of the University of California }
	location { CGC main library }
	explanation {
Use two library functions drand48() and srand48(long) in Sun machines.
For other machines, we may want to change this random number generation 
part. The code should be modified for portability.
	}
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
		addGlobal("double drand48();\n");
		addCode(initSeed);
		addInclude("<math.h>");
	}
	go {
		addCode(random);
	}
	// "code" to initialize the seed
	codeblock(initSeed) {
    srand48($val(seed));
	}
	// "common" part of random number generation
	codeblock(random) {
		int i;
		double sum = 0;
		for (i = 0; i < 27; i++) {
			sum += (drand48() - 0.5);
		}
		if ($val(var) != 1.0) sum *= sqrt($val(var));
		$ref(output) = sum * 2/3 + $val(mean);
	}
	exectime {
		return 300;
	}
}
