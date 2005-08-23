defcorona {
    name { Const }
    domain { ACS }
    desc { Generate a constant signal with the value given by the "level" parameter (default 0.0).
    }
    version { @(#)ACSConst.pl	1.1 05/07/98 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    output {
        name { output }
        type { float }
    }
}
