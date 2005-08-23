defcorona {
    name { Sub }
    domain { ACS }
    desc { }
    version { @(#)ACSSub.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { pos }
        type { float }
    }
    input {
	name { neg }
	type { float }
    }
    output {
        name { output }
        type { float }
    }
}
