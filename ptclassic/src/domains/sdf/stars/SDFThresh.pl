defstar {
	name { FloatThresh }
	domain { SDF }
	desc {
Compares input values to "threshold" (default 0.5).
Output is 0 if input <= threshold, otherwise it is 1.
	}
	version {$Revision$ $Date$}
	author { D. G. Messerschmitt }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name { threshold }
		type { float }
		default {0.5}
		desc { Threshold applied to input. }
	}
	go {
		if( float(input%0) <= double(threshold) )
			output%0 << 0;
		else output%0 << 1;
	}
}
