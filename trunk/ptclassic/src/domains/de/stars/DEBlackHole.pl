defstar {
	name {BlackHole}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc { Discards all input particles. }
	explanation {
A BlackHole accepts input Particles, but doesn't do anything with
them \(em it is
typically used to discard unwanted signals from other blocks.
	}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
