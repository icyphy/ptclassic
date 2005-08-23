defcorona {
    name { Fork }
    domain { ACS }
    desc { Copy input particles to each output. }
    version { @(#)ACSFork.pl	1.4 09/08/99}
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
        type { ANYTYPE }
    }
    outmulti {
        name { output }
        type { = input }
    }
}
