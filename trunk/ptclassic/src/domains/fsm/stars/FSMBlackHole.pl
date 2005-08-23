defstar {
	name {BlackHole}
	domain {FSM}
	version {@(#)FSMBlackHole.pl	1.2 01/08/99}
	desc {
Discard all inputs.  This star is useful for terminating
signals that are not useful.
	}
	author { Bilung Lee }
	copyright {
Copyright (c) 1998-1999 The Regents of the University of California.
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
		type{float}
	}
}
