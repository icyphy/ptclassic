defcore {
    name { Shift }
    domain { ACS }
    coreCategory { FPSim }
    corona { Shift } 
    desc { This is a shifter; the output is shifted down or up by dividing (negative shift) or multiplying (positive shift) (default 0). }
    version { @(#)ACSShiftFPSim.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    ccinclude { <math.h> }
    go {
      double gain;
      // compute 2^shift
      gain = exp(double(corona.shift)*log(2.0));
      corona.output%0 << double(gain) * double(corona.input%0);
    }
}
