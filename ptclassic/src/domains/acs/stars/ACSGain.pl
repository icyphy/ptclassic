defcorona {
    name { Gain }
    domain { ACS }
    desc { This is an amplifier; the output is the input multiplied by the "gain" (default 1.0). }
    version { @(#)ACSGain.pl	1.1 03/16/98 }
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
    defstate {
      name { gain }
      type { float }
      default { "1.0" }
      desc { Gain of the star. }
    }
}
