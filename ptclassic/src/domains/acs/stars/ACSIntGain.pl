defcorona {
    name { IntGain }
    domain { ACS }
    desc { This is an amplifier; the output is the input multiplied by the integer "gain" (default 1). }
    version { $Id$}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
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
      type { int }
      default { 1 }
      desc { Gain of the star, must be an integer }
    }
}
