defstar {
	name { QPSK2 }
	domain { SDF } 
	desc { 2input QPSK mapper. Inputs are int 0/1}
	author { N. Becker }
	version { $Id$ }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	input {
	  name { Iin }
	  type { int }
	}
	input {
	  name { Qin }
	  type { int }
	}
	output {
		name { output }
		type { complex }
	}
	go {
	  double r = ( int(Iin%0) == 0 ) ? 1 : -1;
	  double i = ( int(Qin%0) == 0 ) ? 1 : -1;
	  output%0 << Complex( r, i );
	}
}
