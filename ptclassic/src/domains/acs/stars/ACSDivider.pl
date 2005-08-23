defcorona {
    name { Divider }
    domain { ACS }
    desc { }
    version { @(#)ACSDivider.pl	1.0 10/06/00}
    author { J. Ramanathan }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { dividend }
        type { float }
	desc { dividend }
    }
    input {
	name { divisor }
	type { float }
	desc { divisor }
    }
    output {
        name { output }
        type { float }
	desc { quotient }
    }
}
