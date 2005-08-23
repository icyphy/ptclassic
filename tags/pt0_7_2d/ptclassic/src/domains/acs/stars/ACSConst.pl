defcorona {
    name { Const }
    domain { ACS }
    desc { Generate a constant signal with the value given by the "level" parameter (default 0.0).
    }
    version { @(#)ACSConst.pl	1.7 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    output {
        name { output }
        type { float }
    }
    defstate {
	name { level }
	type{ float }
	default {"0.0"}
	desc {The constant value. }
    }

}
