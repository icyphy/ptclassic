defstar {
	name {BlackHole}
	domain {SDF}
	version {$Id$}
	desc {
Discards all inputs.
	}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
A BlackHole accepts input Particles, but doesn't do anything with
them.  It is typically used to discard unwanted outputs from other blocks.
	}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
