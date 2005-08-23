defcore {
	name { MemLUT }
	domain { ACS }
	coreCategory { FixSim }
	corona { MemLUT }
	desc {
	    Should be filled in:)
	}
	version {@(#)ACSMemLUTFixSim.pl	1.0 01/08/01}
	author { Ken Smith }
	copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
    defstate {
      name { InputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
    }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
    defstate {
      name { OutputPrecision }
      type { precision }
      default { 2.14 }
      desc {
	  Precision of the output in bits.
	  This is the precision that will hold the result
	  of the arithmetic operation on the inputs.
	  When the value of the product extends outside of the precision,
	  the OverflowHandler will be called.
		      }
  }
}

