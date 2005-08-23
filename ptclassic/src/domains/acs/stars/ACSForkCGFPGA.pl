defcore {
	name { Fork }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Fork }
	desc {
	    Fork for CGFPGA dummy domain
	}
	version{ @(#)ACSForkCGFPGA.pl	1.5 08/02/01 }
	author { K. Smith }
	copyright {
Copyright (c) 1998-2001 The Regents of the University of California
and Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;
	}
        method {
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW
		pins->set_precision(1,0,pins->query_bitlen(0),lock_mode);
		pins->set_precision(2,0,pins->query_bitlen(0),lock_mode);

		// Return happy condition
		return(1);
		}
	}
	method {
	    name {sg_designs}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Return happy condition
		return(1);
	    }
	}
	method {
	    name {sg_delays}
	    access {public}
	    type {int}
	    code {
		// Calculate pipe delay
		// NOTE:It should never come to this point!
		acs_delay=0;

		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=FORK;
		acs_existence=SOFT;

	        // Return happy condition
		return(1);
	    }
	}
        method {
	    name {acs_build}
	    access {public}
	    type {int}
	    code {
		ofstream out_fstr(output_filename.str());

		// Trap for language and generate appropriate code
		if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    //out_fstr << "CGFork contribution" << endl;
		}
                // END-USER CODE
		else
		    return(0);

		// Return happy condition
		return(1);
	    }
	}
	setup {
	    forkInit(corona.input,corona.output);
	}
        go {
	    // Funny the original didn't have a go method!
	}
}
