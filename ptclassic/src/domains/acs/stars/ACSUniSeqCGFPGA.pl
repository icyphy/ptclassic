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
		pins->add_pin("MemBusGrant_n",INPUT_PIN_MEMOK);
		pins->add_pin("Reset",INPUT_PIN_RESET);
		pins->add_pin("WC_Carry",INPUT_PIN_WC);
//		pins->add_pin("XbarData_InReg",INPUT_PIN_EXTCTRL);
		pins->add_pin("Go",INPUT_PIN_EXTCTRL);
		pins->add_pin("Clock",INPUT_PIN_CLK);
		pins->add_pin("MemBusReq_n",OUTPUT_PIN_MEMREQ);
		pins->add_pin("Done",OUTPUT_PIN_DONE);
		pins->add_pin("WC_MUX",OUTPUT_PIN_WCMUX);
		pins->add_pin("WC_CE",OUTPUT_PIN_WCCE);
		pins->add_pin("ADDR_CE",OUTPUT_PIN_ADDRCE);
		pins->add_pin("ADDR_CLR",OUTPUT_PIN_ADDRCLR);


		if (sg_language==VHDL_BEHAVIORAL)
		{
		    // General definitions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions

		    // Output port definitions
		    
		    // Bidir port definitions
		    
		    // Control port_definitions
		    pins->set_datatype(0,STD_LOGIC);
		    pins->set_datatype(1,STD_LOGIC);
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_datatype(3,STD_LOGIC);
		    pins->set_datatype(4,STD_LOGIC);
		    pins->set_datatype(5,STD_LOGIC);
		    pins->set_datatype(6,STD_LOGIC);
		    pins->set_datatype(7,STD_LOGIC);
		    pins->set_datatype(8,STD_LOGIC);
		    pins->set_datatype(9,STD_LOGIC);
		    pins->set_datatype(10,STD_LOGIC);

		    pins->set_precision(0,0,1,LOCKED);
		    pins->set_precision(1,0,1,LOCKED);
		    pins->set_precision(2,0,1,UNLOCKED);
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);
		    pins->set_precision(6,0,1,LOCKED);
		    pins->set_precision(7,0,1,LOCKED);
		    pins->set_precision(8,0,1,LOCKED);
		    pins->set_precision(9,0,1,LOCKED);
		    pins->set_precision(10,0,1,LOCKED);
		    
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
