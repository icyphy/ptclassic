defstar {
	name { Fix }	
	domain {SDF}
	desc {
Based star for the fixed-point stars in the SDF domain.
	}
	version { $Id$ }
        author { Brian L. Evans }
	copyright {
Copyright (c) 1993, 1994 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location { SDF main library }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
Overflow characteristic for the output.
If the result of the sum cannot be fit into the precision of the output,
then overflow occurs and the overflow is taken care of by the method
specified by this parameter.
The keywords for overflow handling methods are:
"saturate" (the default), "zero_saturate", "wrapped", and "warning".
The "warning" option will generate a warning message whenever overflow occurs.
		}
        }
        defstate {
                name { ReportOverflow }
                type { int }
                default { "YES" }
                desc {
If non-zero, i.e. YES or TRUE, then report any errors (if any) that
occurred during the fixed-point computations in this star, after
the simulation is over.
                }
        }
        protected {
		int overflows, totalChecks;
        }
	inline method {
		name { checkOverflow }
		type { "int" }
		arglist { "(Fix& fix)" }
		access { protected }
		code {
			int overflag = fix.ovf_occurred();
			totalChecks++;
			if ( overflag ) {
			  overflows++;
			  fix.clear_errors();
			}
			return overflag;
		}
	}
        method {
		name { initialize }
		type { "void" }
		arglist { "()" }
		access { public }
		code {
			overflows = 0;
			totalChecks = 0;
		}
	}
	// derived stars should call this method if they defined their 
	// own wrapup method
	wrapup {
		if ( int(ReportOverflow) && ( overflows > 0 ) ) {
		  StringList msg;
		  double percentage = 0;
		  if ( totalChecks > 0 )
		    percentage = (100.0*overflows)/totalChecks;
		  msg << " experienced overflow in " << overflows 
		      << " out of " << totalChecks
		      << " fixed-point calculations checked ("
		      << percentage << "%)";
		  Error::warn( *this, msg );
		}
	}
}
