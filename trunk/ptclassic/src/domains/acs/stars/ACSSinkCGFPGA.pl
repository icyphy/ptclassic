defcore {
	name { Sink }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Sink }
	desc {
	    Swallows an input sample
	}
	version {$Id$}
	author { K. Smith }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	defstate {
	    name {Input_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Input_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input}
	    default {1}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Delay_Impact}
	    type {string}
	    desc {How does this delay affect scheduling? (Algorithmic or None)}
	    default {"None"}
	}
	defstate {
	    name {address}
	    type {int}
	    desc {address for sink variable}
	    default {"-1"}
	}
	defstate {
	    name {row_size}
	    type {int}
	    desc {row size of sink variable}
	    default {1}
	}
	defstate {
	    name {column_size}
	    type {int}
	    desc {column size of sink variable}
	    default {1}
	    attributes {A_CONSTANT|A_NONSETTABLE}
	}
	defstate {
	    name {Domain}
	    type {string}
	    desc {Where does this function reside (HW/SW)}
	    default{"HW"}
	}
	defstate {
	    name {Technology}
	    type {string}
	    desc {What is this function to be implemented on (e.g., C30, 4025mq240-4)}
	    default{""}
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
	    arglist { "(SequentialList* input_list,SequentialList* output_list)" }
	    type {int}
	    code {
		input_list->append((Pointer) "Input_Major_Bit");
		input_list->append((Pointer) "Input_Bit_Length");
		    
		// Return happy condition
		return(1);
	    }
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
	    name {sg_resources}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW

		// Calculate CLB sizes
		resources->set_occupancy(0,0);
		
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
		acs_type=SINK;
		acs_existence=SOFT;

	        // Input port definitions
		pins->add_pin("A","input",INPUT_PIN);

		// Output port definitions

		// Bidir port definitions
		
		// Control port definitions

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		libs->add("IEEE");
		incs->add(".std_logic_1164.all");

		// Input port definitions
		pins->set_datatype(0,STD_LOGIC);  // A port
		    
		// Output port definitions
		
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
//	    corona.input.setSDFParams(n,int(n)-1);
	}

	go {
		addCode(blockname);
	}
	codeblock (blockname) {
// Sink star: read $val(n) samples
// Line 2 from Sink star
	}
}

