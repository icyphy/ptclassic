defstar {
	name { Floor }
	domain { SDF } 
	desc { Outputs the greatest integer <= input. }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
	}
	go {
		float t = input%0;
		int it = int(t);
		// handle rounding towards 0 on some processors
		if (it > t) it -= 1;
		output%0 << it;
	}
}
