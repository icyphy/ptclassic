defcorona {
    name { Lut }
    domain { ACS }
    desc { Look-up Table }
    version { @(#)ACSLut.pl	1.7 11/22/00}
    author { J. Ramanathan }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
	name { input }
	type { float }
    output {
        name { output }
        type { float }
    }
}
