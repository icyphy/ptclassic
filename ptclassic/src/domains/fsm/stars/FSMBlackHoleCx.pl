defstar {
	name {BlackHoleCx}
	domain {FSM}
	version {$Id$}
	desc {
Discard all inputs.  This star is useful for terminating
signals that are not useful.
	}
	author { Bilung Lee }
	copyright {
Copyright (c) 1998-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { FSM main library }
	htmldoc {
A BlackHole accepts input Particles, but doesn't do anything with
them.  It is typically used to discard unwanted outputs from other blocks.
	}
	input {
		name{input}
		type{complex}
	}
}
