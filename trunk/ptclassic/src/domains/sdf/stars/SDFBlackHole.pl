defstar {
	name {BlackHole}
	domain {SDF}
	version {$Id$}
	desc {
Discard all inputs.  This star is useful for terminating
signals that are not useful.
	}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
