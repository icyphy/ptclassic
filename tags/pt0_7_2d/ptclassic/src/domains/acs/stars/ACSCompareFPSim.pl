defcore {
    name { Compare }
    domain { ACS }
    coreCategory { FPSim }
    corona { Compare } 
    desc { Output 1 (True) if left input is less than right input }
    version { @(#)ACSCompareFPSim.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    go {
      if ((double)(corona.left%0) < (double)(corona.right%0))
        corona.result%0 << (double) (1.0);
      else
        corona.result%0 << (double) (0.0);
    }
}
