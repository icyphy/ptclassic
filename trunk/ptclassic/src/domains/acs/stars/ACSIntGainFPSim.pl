defcore {
    name { IntGain }
    domain { ACS }
    coreCategory { FPSim }
    corona { IntGain } 
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
    
    go {
      corona.output%0 << double(corona.gain) * double(corona.input%0);
    }
}
