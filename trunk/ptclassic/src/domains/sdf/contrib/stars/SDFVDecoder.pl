defstar {
	name {VDecoder}
	domain {SDF}
	desc { Viterbi Decoder }
	location { SDF user contribution library }
	version { $Id$ }
	author { N. Becker }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
	    name {I}
	    type {int}
	}
	input {
	    name {Q}
	    type {int}
	}
	output {
	    name {out}
	    type {int}
	}
	defstate {
	    name{ Gain }
	    type{ float }
	    default{ 1 }
	    desc{ The nominal level of the input }
	}
	hinclude { "Decoder.h" }
	private {
	    Decoder D;
	}
	conscalls{ D(int(Gain)) }
	setup {
	    D.Reset();
	    D.SetGain( int(Gain) );
	}
	go {
	    out%0 << D( int(I%0), int(Q%0) );
	}
}
