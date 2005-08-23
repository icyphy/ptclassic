defcorona {
    name { 16FFT }
    domain { ACS }
    desc { Compute a 16point complex FFT }
    version { @(#)ACS16fft.pl	1.0 03/03/00 }
    author { Ken Smith }
    copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
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
        name { real_output }
        type { float }
    }
    output {
        name { imag_output }
        type { float }
    }
}
