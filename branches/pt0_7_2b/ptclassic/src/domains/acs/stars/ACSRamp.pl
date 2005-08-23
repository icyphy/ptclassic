defcorona {
    name { Ramp }
    domain { ACS }
    desc { Generate a ramp signal, starting at "value" (default 0) and
incrementing by step size "step" (default 1) on each firing.
    }
    version { $Id$ }
    author { James Lundblad }
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
