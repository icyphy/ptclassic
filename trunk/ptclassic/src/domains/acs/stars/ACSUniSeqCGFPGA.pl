defcore {
	name { UniSeq }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { UniSeq }
	desc {
	    Core for the unirate sequencer
	}
	version {$Id$
	author { K. Smith }
	copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
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
	    name {macro_query}
	    access {public}
	    type {int}
	    code {
		// BEGIN-USER CODE
		return(NORMAL_STAR);
		// END-USER CODE
	    }
	}
	method {
	    name {macro_build}
	    access {public}
	    arglist { "(int inodes,int* acs_ids)" }
	    type {SequentialList}
	    code {
		return(NULL);
	    }
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(SequentialList* input_list,SequentialList* output_list)" }
	    type {int}
	    code {
		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_resources}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Return happy condition
		return(1);
		}
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		// General definitions
		acs_type=BOTH;
		acs_existence=HARD;

		// Input port definitions

		// Output port definitions

		// Bidir port definitions

		// Control port definitions

		if (sg_language==VHDL_BEHAVIORAL)
		{
		    // General definitions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions

		    // Output port definitions
		    
		    // Bidir port definitions
		    
		    // Control port_definitions
		    
		}
		else
		    return(0);

	        // Return happy condition
		return(1);
	    }
	}
        method {
	    name {acs_build}
	    access {public}
	    type {int}
	    code {
		if (DEBUG_STARS)
		    printf("ACSUniSeqCGFPGA.acs_build invoked\n");
		// Return happy condition
		return(1);
	    }
	}
	setup {
	    // void
	}
        go {
	    // Funny the original didn't have a go method!
	}
}
