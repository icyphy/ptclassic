defcore {
    name { Compare }
    domain { ACS }
    coreCategory { FixSim }
    corona { Compare } 
    desc { Output 1 (True) if left input is less than right input }
    version { @(#)ACSCompareFixSim.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    defstate {
      name { InputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
    }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
    defstate {
      name { OutputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.  }
    }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
      constructor {
	noInternalState();
      }
      go {
	// create zero valued particle with one bit
	Fix tmp(1, 1);
	Fix left_val = corona.left%0;
	Fix right_val = corona.right%0;
	if (left_val > right_val)
	  tmp = (double)(1.0);
	corona.result%0 << tmp;
    }
}
