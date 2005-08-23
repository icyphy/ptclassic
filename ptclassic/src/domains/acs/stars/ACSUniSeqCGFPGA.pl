defcore {
	name { UniSeq }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { UniSeq }
	desc {
	    Core for the unirate sequencer
	}
	version{ @(#)ACSUniSeqCGFPGA.pl	1.10 08/02/01 }
	author { K. Smith }
	copyright {
Copyright (c) 1998-2001 The Regents of the University of California
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

	    static const int DEBUG_SEQUENCER=0;
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
	    type {"CoreList*"}
	    code {
		return(NULL);
	    }
	}
        method {
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
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
		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		output_filename << dest_dir << name();

		// General definitions
		acs_type=BOTH;
		acs_existence=HARD;

		// Input port definitions

		// Output port definitions

		// Bidir port definitions

		// Control port definitions

		if (sg_language==VHDL_BEHAVIORAL)
		{
		    output_filename << ".vhd" << ends;

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
		  if (DEBUG_SEQUENCER)
		      printf("sequencer file=%s\n",output_filename.str());
		  ofstream fstr(output_filename.str());
		  
		  // Generate code
		  fstr << lang->gen_libraries(libs,incs);
		  fstr << lang->gen_entity(name(),pins);
		  fstr << lang->gen_architecture(name(),
						 NULL,
						 BEHAVIORAL,
						 pins,
						 data_signals,
						 ctrl_signals,
						 constant_signals);
		  fstr << sg_declarative.str() << endl;
		  fstr << lang->begin_scope << endl;
		  fstr << sg_body.str() << endl;
		  fstr << lang->end_scope << lang->end_statement << endl;
		
		  fstr.close();

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
