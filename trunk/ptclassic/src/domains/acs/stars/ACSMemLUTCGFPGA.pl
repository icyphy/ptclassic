defcore {
	name { MemLUT }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { MemLUT }
	desc {
	    Utilize a local memory as a lookup table
	}
	version {@(#)ACSMemLUTCGFPGA.pl	1.0 01/08/01}
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
	    name {starting_address}
	    type {intarray}
	    default {"0"}
	    desc { Base address of the LUT, or an array of multiple base addresses }
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
	    default {"YES"}
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

	    ACSIntArray* address_starts;
	}
	constructor {
	    address_starts=new ACSIntArray;
	}
	destructor {
	    delete address_starts;
	}
	method {
	    name {sg_get_privaledge}
	    access {public}
	    type {"ACSIntArray*"}
	    code {
		return(address_starts);
	    }
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray*  output_list)" }
	    type {int}
	    code {
		input_list->add("Input_Major_Bit");
		input_list->add("Input_Bit_Length");
		output_list->add("Output_Major_Bit");
		output_list->add("Output_Bit_Length");
		    
		// Return happy condition
		return(1);
	    }
	}
        method {
            name {sg_cost}
            access {public}
            arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file, ofstream& schedule_file)" }
            type {int}
            code {
                // BEGIN-USER CODE
                cost_file << "cost=ceil(0.5*insizes);" << endl;

                //  numsim_file << "y=sum(x);" << endl; 
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k}; " << endl;
                numsim_file <<  " end " << endl;
                numsim_file <<  " " << endl;

                rangecalc_file << "orr=[0 268288];" << endl;

                natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

                // this is ok because adder latency does not depend on wordlength
                schedule_file << " vl1=veclengs(1); " << endl;
                schedule_file << " racts1=[0 1 vl1-1 ; 4 1 vl1+3];" << endl;
                schedule_file << " racts=cell(1,size(insizes,2));" << endl;
                schedule_file << " racts(:)=deal({racts1});" << endl;
                schedule_file << " minlr=vl1*ones(1,size(insizes,2)); " << endl;
                schedule_file << " if sum(numforms)>0 " << endl;
                schedule_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                schedule_file << " end " << endl;


                // END-USER CODE

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
		// User must dictate this

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
		acs_delay=4;
		
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
		acs_type=SOURCE_LUT;
		acs_existence=SOFT;

		// Differing from algorithmic stars, this star is driven by a lsb-weighted address!
		bitslice_strategy=PRESERVE_LSB;

		// Translate lut addresses into reserved array for stitcher
		for (int loop=0;loop<starting_address.size();loop++)
		    address_starts->add(starting_address[loop]);

	        // Input port definitions
		pins->add_pin("IN","input",INPUT_PIN);

		// Output port definitions
		pins->add_pin("A","output",OUTPUT_PIN);
//		pins->set_wordcount(0,intparam_query("word_count"));

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
		pins->set_datatype(0,STD_LOGIC);  // IN port
		    
		// Output port definitions
		pins->set_datatype(1,STD_LOGIC);  // A port
		
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

