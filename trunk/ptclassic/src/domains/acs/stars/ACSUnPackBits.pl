defcorona {
    name { UnPackBits }
    domain { ACS }
    desc { Given one input stream, splice and produce two streams}
    version { @(#)ACSUnPackBits.pl	1.0 03/03/00}
    author { Ken Smith }
    copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
        name { input_stream }
        type { float }
    }
    output {
        name { output_stream1 }
        type { float }
    }
    output {
        name { output_stream2 }
        type { float }
    }
}
