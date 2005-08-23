defcorona {
    name { Cos }
    domain { ACS }
    desc { This star computes the cosine of its input, assumed to be an angle in radians. }
    version { @(#)ACSCos.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { input }
        type { float }
    }
    output {
        name { output }
        type { float }
    }
}
