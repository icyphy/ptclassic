defcorona {
    name { Shift }
    domain { ACS }
    desc { This star shifts the input left (+) or right (-) a number of specificed bits }
    version { $Id$
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company.
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
      name { shift }
      type { int }
      default { "1" }
      desc { Shift down or up using factor of 2^shift. }
    }
}
