defstar {
	name {VDecoder}
	domain {SDF}
	desc { Viterbi Decoder }
	location { SDF user contribution library }
	version { @(#)SDFVDecoder.pl	1.7	2/3/96 }
	author { N. Becker }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	    Decoder decoder;
	}
	conscalls{ decoder(double(Gain)) }
	setup {
	    decoder.Reset();
	    decoder.SetGain( double(Gain) );
	}
	go {
	    out%0 << decoder(int(I%0), int(Q%0));
	}
}
