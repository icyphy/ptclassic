defstar {
	name {BlackHole}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
