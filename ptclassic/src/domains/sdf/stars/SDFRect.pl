ident {
// $Id$
// This star produces an impulse or impulse train.
// From the gabriel "impulse" star.
// Programmer: J. Buck
// Date of creation: 5/31/90
// Converted to use preprocessor: 10/3/90

// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.
}
defstar {
	name { FloatRect }
	domain { SDF }
	desc {
	   "Generates an rectangular pulse of height 'height' (default 1.0).\n"
	   "with width 'width' (default 8)."
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { height }
		type { float }
		default { 1.0 }
		desc { "height of rectangular pulse" }
	}
	defstate {
		name { width }
		type { int }
		default { 8 }
		desc { "width of rectangular pulse" }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { "internal counting state" }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	go {
		double t = 0.0;
		if (int(count) < int(width)) t = height;
		count = int(count) + 1;
		output%0 << t;
	}
}

