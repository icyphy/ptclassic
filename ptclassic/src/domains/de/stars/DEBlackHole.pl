defstar {
	name {BlackHole}
	domain {DE}
	version { @(#)DEBlackHole.pl	1.9	10/01/96}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc { Discard all input particles. }
	htmldoc {
A BlackHole accepts input Particles, but doesn't do anything with
them \(em it is
typically used to discard unwanted signals from other blocks.
	}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
