defstar {
	name { FloatRect }
	domain { SDF }
	desc {
Generates a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
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
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, the period of the pulse stream. }
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
		output%0 << t;
		count = int(count) + 1;
		if (int(period) > 0 && int(count) >= int(period)) count = 0;
	}
}

