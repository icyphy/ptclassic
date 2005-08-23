defcorona {
    name { Max }
    domain { ACS }
    desc { Produces the maximum of two inputs.}
    version { @(#)ACSMax.pl	1.0 12/13/00 }
    author { J. Ramanathan }
    copyright {
	Copyright (c) 1998 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
	name { input1 }
	type { float }
    }
    input {
	name { input2 }
	type { float }
    }
    output {
	name { output }
	type { float }
    }
}
