defcorona {
    name { PackBits }
    domain { ACS }
    desc { Given two input streams, merge and produce one stream}
    version { @(#)ACSPackBits.pl	1.0 03/03/00}
    author { Ken Smith }
    copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { stream1_in }
        type { float }
    }
    input {
	name { stream2_in }
	type { float }
    }
    output {
        name { merged_stream }
        type { float }
    }
}
