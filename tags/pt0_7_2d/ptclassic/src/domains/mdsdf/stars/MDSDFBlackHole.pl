defstar {
	name { BlackHole }
	domain { MDSDF }
	version { @(#)MDSDFBlackHole.pl	1.5 01 Oct 1996 }
	desc {
Discard all inputs.  This star is useful for terminating signals that are
not useful.
	}
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	htmldoc {
A BlackHole accepts input Matrices, but doesn't do anything with them.
It is typically used to discard unwanted outputs from other blocks.
	}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
