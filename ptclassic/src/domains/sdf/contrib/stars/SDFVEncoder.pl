defstar {
	name {VEncoder}
	domain {SDF}
	desc { Viterbi Encoder }
	location { SDF user contribution library }
	author { N. Becker }
        copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF main library }
	hinclude { "Encoder.h" }
	input {
	    name {input}
	    type {int}
	}
	output {
	    name {I}
	    type {int}
	}
	output {
	    name {Q}
	    type {int}
	}
	private {
	    Encoder E;
	}
	setup {
	    E.Reset();
	}
	go {
	    int output[ 2 ];
	    E( input%0, output );
	    I%0 << output[ 0 ];
	    Q%0 << output[ 1 ];
	}
}
