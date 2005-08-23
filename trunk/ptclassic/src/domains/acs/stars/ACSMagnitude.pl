defcorona {
    name { Magnitude }
    domain { ACS }
    desc { Output the magnitude of a complex number. }
    version { @(#)ACSMagnitude.pl 1.0 04/17/00}
    author { Ken Smith }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { real }
        type { float }
    }
    input {
        name { imag }
        type { float }
    }
    output {
        name { magnitude }
        type { float }
    }
}
