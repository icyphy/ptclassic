defstar {
	name { FloatRect }
	domain { SDF }
	desc {
Generates a rectangular pulse of height "height" (default 1.0).
with width "width" (default 8).
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { height }
		type { float }
		default { 1.0 }
		desc { Height of the rectangular pulse. }
	}
	defstate {
		name { width }
		type { int }
		default { 8 }
		desc { Width of the rectangular pulse. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal counting state. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	go {
		double t = 0.0;
		if (int(count) < int(width)) t = height;
		count = int(count) + 1;
		output%0 << t;
	}
}

