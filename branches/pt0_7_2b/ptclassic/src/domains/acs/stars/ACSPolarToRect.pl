defcorona {
    name { PolarToRect }
    domain { ACS }
    desc { Convert magnitude and phase to rectangular form. }
    version { @(#)ACSPolarToRect.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
	input {
		name { magnitude }
		type { float }
	}
	input {
		name { phase }
		type { float }
	}
	output {
		name { x }
		type { float }
	}
	output {
		name { y }
		type { float }
	}
}
