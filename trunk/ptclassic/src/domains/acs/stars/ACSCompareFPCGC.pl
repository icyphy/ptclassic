defcore {
    name { Compare }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Compare } 
    desc { Output 1 (True) if left input is less than right input }
    version { @(#)ACSCompareFPCGC.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    constructor {
      noInternalState();
    }
    go {
      addCode("if ($ref(left) < $ref(right)) $ref(output) = 1.0; else $ref(output) = 0.0;");
    }
    exectime {
      return 1;
    }
}
