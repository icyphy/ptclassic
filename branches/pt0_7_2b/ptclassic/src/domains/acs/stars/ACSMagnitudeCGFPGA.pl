defcore {
	name { Magnitude }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Magnitude }
	desc { Output the magnitude of a complex number. }
	version {@(#)ACSMagnitude.pl	1.0 04/17/00}
	author { Ken Smith }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
        ccinclude { <sys/wait.h> }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	defstate {
	    name {Real_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input of the real component}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Real_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input of the real component}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Imag_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input of the imaginary component}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Imag_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input of the imaginary component}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Magnitude_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the output}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Magnitude_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the output}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Sign_Convention}
	    type {string}
	    desc {Signed or Unsigned}
	    default {"Signed"}
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

	    // White Star Constructor
	    Sg_Constructs* construct;
	    CoreList* white_stars;

	    ACSCGFPGACore* sqr1;
	    ACSCGFPGACore* sqr2;
	    ACSCGFPGACore* summer;
	    ACSCGFPGACore* sqrt;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		input_list->add("Real_Major_Bit");
		input_list->add("Real_Bit_Length");
		input_list->add("Imag_Major_Bit");
		input_list->add("Imag_Bit_Length");
		output_list->add("Magnitude_Major_Bit");
		output_list->add("Magnitude_Bit_Length");
		    
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
		return(WHITE_STAR);
		// END-USER CODE
	    }
	}
	method {
	    name {macro_build}
	    access {public}
	    arglist { "(int* free_id, int* free_netid)" }
	    type {"CoreList*"}
	    code {
		white_stars=new CoreList;
		construct=new Sg_Constructs(free_id,free_netid,dest_dir);

		// BEGIN-USER CODE
		sqr1=construct->add_sg("ACS","Mpy","CGFPGA",BOTH,SIGNED,white_stars);
		sqr2=construct->add_sg("ACS","Mpy","CGFPGA",BOTH,SIGNED,white_stars);
		summer=construct->add_adder(SIGNED,white_stars);
		sqrt=construct->add_sg("ACS","Sqrt","CGFPGA",BOTH,SIGNED,white_stars);

		construct->connect_sg(sqr1,summer);
		construct->connect_sg(sqr2,summer);
		construct->connect_sg(summer,sqrt);

		delete construct;
		// END-USER CODE

		// Return happy condition
		return(white_stars);
	    }
	}
	method {
	    name {macro_connections}
	    access {public}
	    type {"Connection_List*"}
	    code {
		// Map how this core's pins map to the underlying white stars
	        Connection_List* connections=new Connection_List;
                
		// BEGIN-USER CODE
                // Real input connects to sqr1 twice
                connections->add(-1,-1,-1,0,sqr1->acs_id,-1);
                connections->add(-1,-1,-1,0,sqr1->acs_id,-1);

                // Imaginary input connects to sqr2 twice
                connections->add(-1,-1,-1,1,sqr2->acs_id,-1);
                connections->add(-1,-1,-1,1,sqr2->acs_id,-1);

                // Connect the result from square rooting to the output pin
                connections->add(2,sqrt->acs_id,-1,-1,-1,-1);
		// END-USER CODE

                return(connections);
	    }
	}

	method {
	    name {sg_cost}
	    access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file, ofstream& schedule_file)" }
	    type {int}
	    code {
		// BEGIN-USER CODE
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
                // White star, all precisions will be determined by children
			    
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
                // White star, all delays well be determined by children

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
		acs_type=BOTH;
		acs_existence=HARD;

	        // Input port definitions
		pins->add_pin("a","real",INPUT_PIN);
		pins->add_pin("b","imag",INPUT_PIN);

		// Output port definitions
		pins->add_pin("s","magnitude",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",0,1,INPUT_PIN_CLK);
		pins->add_pin("ce",0,1,INPUT_PIN_CE,AH);
		pins->add_pin("ci",0,1,INPUT_PIN_CARRY,AL);
		pins->add_pin("clr",0,1,INPUT_PIN_CLR,AH);

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		/////////////////////////////////
		// Language-dependent assignments
		/////////////////////////////////
		if (sg_language==VHDL_BEHAVIORAL)
		{

		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");
		    
		    // Input port definitions
		    pins->set_datatype(0,STD_LOGIC); // a port
		    pins->set_datatype(1,STD_LOGIC); // b port
		    pins->set_min_vlength(0,2);
		    pins->set_max_vlength(0,32);
		    pins->set_min_vlength(1,2);
		    pins->set_max_vlength(1,32);
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);// c port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(3,STD_LOGIC);  // clk pin
		    pins->set_datatype(4,STD_LOGIC);  // oe pin
		    pins->set_datatype(5,STD_LOGIC);  // ci pin
		    pins->set_datatype(6,STD_LOGIC);  // clr pin
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);
		    pins->set_precision(6,0,1,LOCKED);
		    // END-USER CODE
		}
		else
		    return(0);

	        // Return happy condition
		return(1);
	    }
	}
	go {
		addCode(block);
	}
	codeblock (block) {
// Multi Input star
	}
}

