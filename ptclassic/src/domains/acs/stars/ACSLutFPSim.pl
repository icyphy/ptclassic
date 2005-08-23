defcore {
    name { Lut }
    domain { ACS }
    coreCategory { FPSim }
    corona { Lut } 
    desc { This is an amplifier; the output is the input multiplied by the integer "gain" (default 1). }
    version { @(#)ACSLutFPSim.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    
    go {
      corona.output%0 << double(corona.input%0);
    }
}
