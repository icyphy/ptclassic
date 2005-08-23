defcore {
	name { Ramp }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Ramp }
	desc {
	    Generic code generator source star.
	}
	version{ @(#)ACSRampCGFPGA.pl	1.9 08/02/01 }
	author { Eric K. Pauer }
	copyright {
Copyright (c) 1998-2001 The Regents of the University of California
and Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	defstate {
	    name {Output_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the output}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Output_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the output}
	    default {1}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {address_start}
	    type {int}
	    desc {address for source variable}
	    default {"-1"}
	}
	defstate {
	    name {address_step}
	    type {int}
	    desc {skip amount between addresses}
	    default {"1"}
	}
	defstate {
	    name {word_count}
	    type {int}
	    desc {total number of words for this io star, default will cause automatic unirate calcuation}
	    default {"-1"}
	}
	defstate {
	    name {Domain}
	    type {string}
	    desc {Where does this function reside (HW/SW)}
	    default{"HW"}
	}
        defstate {
	    name {Device_Number}
	    type {int}
	    desc {Which device (e.g. fpga, mem)  will this smart generator build for (if applicable)}
	    default{0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Device_Lock}
	    type {int}
	    default {"NO"}
	    desc {Flag that indicates that this function must be mapped to the specified Device_Number}
	}
        defstate {
	    name {Language}
	    type {string}
	    desc {What language should this function be described in (e.g, VHDL, C, XNF)}
	    default{"VHDL"}
	}
        defstate {
	    name {Comment}
	    type {string}
	    desc {A user-specified identifier}
	    default{""}
	}
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		output_list->add("Output_Major_Bit");
		output_list->add("Output_Bit_Length");
		    
		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW
		// Variable, user must describe

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
		output_filename << name() << ends;

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=SOURCE;
		acs_existence=SOFT;

	        // Input port definitions

		// Output port definitions
		pins->add_pin("A","output",OUTPUT_PIN);
		pins->set_wordcount(0,intparam_query("word_count"));

		// Bidir port definitions
		
		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		// General defintions
		libs->add("IEEE");
		incs->add(".std_logic_1164.all");

		// Input port definitions
		    
		// Output port definitions
		pins->set_datatype(0,STD_LOGIC);  // A port
		
		// Bidir port definitions

   		// Control port definitions
		// END-USER CODE

		// Return happy condition
		return(1);
		}
	}
        method {
	    name {acs_build}
	    access {public}
	    type {int}
	    code {
		// Return happy condition
		return(1);
	    }
	}
	setup {
//	    corona.output.setSDFParams(n,int(n)-1);
	}

	go {
		addCode(blockname);
	}

	codeblock (blockname) {
// Source star: write $val(n) samples
// Line 2 from Source star
	}
}

