defcore {
	name { Accum }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Accum }
	desc {
This is an integrator with leakage and limits.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  

Limits are controlled by the "top" and "bottom" parameters.
If top &gt;= bottom, no limiting is performed (this is the default).
Otherwise, the output is kept between "bottom" and "top".
If "saturate" = YES, saturation is performed.  If "saturate" = NO,
wrap-around is performed (this is the default).
Limiting is performed before output.

Leakage is controlled by the "feedbackGain" state (default 1.0).
The output is the data input plus feedbackGain*state, where state
is the previous output.
	}
	version {@(#)ACSIntegratorCGFPGA.pl	1.4 09/10/99}
	author { K. Smith }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
	}

        ccinclude { <sys/wait.h> }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	protected {
	    Connection_List* connections;
	}

	constructor {
	    connections=NULL;
	}
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
	    default {0}
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
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Accum_Time}
	    type {int}
	    desc {
How many clock periods are needed for accumulation.  For Scheduling ONLY!
Load signal needs to be supplied by manually. }
	    default {1}
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
	    arglist { "(int* free_id)" }
	    type {"CoreList*"}
	    code {
		CoreList* white_stars=new CoreList;
		Sg_Constructs* construct=new Sg_Constructs(free_id,dest_dir);
	        connections=new Connection_List;

		ACSCGFPGACore* integrator=construct->add_sg("ACS","Integrator","CGFPGA",
							    BOTH,UNSIGNED,white_stars);
		ACSCGFPGACore* phase_core=construct->add_phaser(acs_delay,white_stars);

		// Hook up internals
		construct->connect_sg(phase_core,OUTPUT_PIN_PHASE,integrator,INPUT_PIN_CLR);

		//
		// Hook up externals
		//
		// Input pin
		connections->add(-1,-1,-1,
				 0,integrator->acs_id,integrator->find_hardpin(INPUT_PIN));
		
		// Summation pin
		connections->add(1,integrator->acs_id,integrator->find_hardpin(OUTPUT_PIN),
				 -1,-1,-1);

		// Clock pin
	        // Left to resolver

		// Ce pin
		connections->add(-1,-1,-1,
				 3,integrator->acs_id,integrator->find_hardpin(INPUT_PIN_CE));
		connections->add(-1,-1,-1,
				 3,phase_core->acs_id,phase_core->find_hardpin(INPUT_PIN_EXTCTRL));

		// END-USER CODE
		delete construct;

		// Return happy condition
		return(white_stars);
	    }
	}
	method {
	    name {macro_connections}
	    access {public}
	    type {"Connection_List*"}
	    code {
		return(connections);
	    }
	}

	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		input_list->add("Input_Major_Bit");
		input_list->add("Input_Bit_Length");
		output_list->add("Output_Major_Bit");
		output_list->add("Output_Bit_Length");

		// BEGIN-USER CODE
		// END-USER CODE
		    
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
		cost_file << "cost=(outsizes/2)+1;" << endl;

		// numsim_file << "y=x*" << acs_delay << ";" << endl  << "%Note: for range and variance calc only!" << endl;
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k}* " << acs_delay << ";" << endl;
                numsim_file <<  " end " << endl;
		numsim_file << "%Note: for range and variance calc only!" << endl;
                numsim_file <<  " " << endl;


		rangecalc_file << "orr=inputrange*" << acs_delay << ";" << endl;

                schedule_file << "outdel=" << acs_delay <<  "; " << endl;
                schedule_file << "vl1=veclengs(1); " << endl;
                schedule_file << "racts=cell(1,size(insizes,2));" << endl;
                schedule_file << "for k=1:size(insizes,2)" << endl;
                schedule_file << "  racts1=[0 1 vl1-1 ; outdel(k) 1 vl1-1+outdel(k)];" << endl;
                schedule_file << "  racts{k}=racts1;" << endl;
                schedule_file << "end"  << endl;
                schedule_file << "minlr=vl1*ones(1,size(insizes,2)); " << endl;
                schedule_file << "if sum(numforms)>0 " << endl;
                schedule_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                schedule_file << "end " << endl;



		natcon_file << "yesno=(insizes>=2 & insizes<=32 & outsizes>=2 & outsizes<=32);" << endl;
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
		acs_delay=intparam_query("Accum_Time");

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
		pins->add_pin("b","data",INPUT_PIN);

		// Output port definitions
		pins->add_pin("s","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_CE,AH);

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		//////////////////////////////////
		// Language-dependnent assignments
		//////////////////////////////////
		if (sg_language==VHDL_BEHAVIORAL)
		{
		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions
		    pins->set_datatype(0,STD_LOGIC);  // b port
		    pins->set_min_vlength(0,2);
		    pins->set_max_vlength(0,32);
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC); // s port
		    pins->set_min_vlength(1,2);
		    pins->set_max_vlength(1,64);
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // clk pin
		    pins->set_datatype(3,STD_LOGIC);  // ce pin
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);

		    // Capability assignments
		    sg_capability->add_domain("HW");
		    sg_capability->add_architecture("any");
		    sg_capability->add_language(VHDL_BEHAVIORAL);
		    // END-USER CODE
		}
		else
		    return(0);
	        // Return happy condition
		return(1);
	    }
	}
	go {
		addCode(trueblock);
	}
	codeblock (trueblock) {
// Line 1 from CGDelay if control==TRUE
// Line 2 from CGDelay if control==TRUE
	}
	codeblock (falseblock) {
// Line 1 from CGDelay if control==FALSE
// Line 2 from CGDelay if control==FALSE
	}
}

