defstar {
	name {VDecoder}
	domain {SDF}
	desc { Viterbi Decoder }
	author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF main library }
	hinclude { "Decoder.h" }
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
	private {
	    Decoder D;
	}
	defstate {
	  name{ Gain }
	  type{ float }
	  default{ 1 }
	  desc{ The nominal level of the input }
	}
	conscalls{ D ((int)Gain) }
	setup {
	  D.Reset();
	  D.SetGain( (int)Gain );
	}
	go {
	    out%0 << D( I%0, Q%0 );
	}
}
