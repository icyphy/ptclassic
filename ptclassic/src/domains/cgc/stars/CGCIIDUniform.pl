defstar {
	name { IIDUniform }
	domain {CGC}
	desc {
Generate pseudo-IID-uniform random variables.  The values range from
-range to range where range is a parameter.
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
		name { range }
		type { float }
		default { 1.0 }
		desc { range of random number generator is [-range,+range] }
	}
	defstate {
		name { seed }
		type { int }
		default { 1 }
	}
	initCode {
		addGlobal("double drand48();\n");
		gencode(initSeed);
	}
	go {
		gencode(random);
	}
	// "code" to initialize the seed
	codeblock(initSeed) {
    srand48($val(seed));
	}
	// "common" part of random number generation
	codeblock(random) {
		float scale = $val(range) * 2.0;
		$ref(output) = scale * (drand48() - 0.5);
	}
}
