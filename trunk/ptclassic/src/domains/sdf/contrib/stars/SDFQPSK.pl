defstar {
	name { QPSK }
	domain { SDF } 
	desc { 1input QPSK mapper. Inputs are int 0/1}
	author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	input {
	  name { in }
	  type { int }
	  numtokens { 2 }
	}
	output {
		name { output }
		type { complex }
	}
	go {
	  double r = (int)(in%0) == 0 ? 1 : -1;
	  double i = (int)(in%1) == 0 ? 1 : -1;
	  output%0 << Complex( r, i );
	}
}
