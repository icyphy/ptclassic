defcore {
    name { HRRTemplates }
    domain { ACS }
    coreCategory { CGFPGA }
    corona { HRRTemplates }
    desc { Produces a subset of the template data based on the incoming angle }
    version {@(#)ACSHRRTemplatesCGFPGA.pl	1.0 12/18/00}
    author { J. Ramanathan }
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

		ACSCGFPGACore* negativefive=construct->add_const(-5,SIGNED,white_stars);
		ACSCGFPGACore* subtractor=construct->add_sg("ACS","Sub","CGFPGA",BOTH,SIGNED,white_stars);
		ACSCGFPGACore* timeseighty=construct->add_sg("ACS","IntGain","CGFPGA",BOTH,SIGNED,white_stars);
		int baseaddr1,baseaddr2,baseaddr3,baseaddr4,baseaddr5,baseaddr6,baseaddr7;
		baseaddr1=0;
		baseaddr2=0;
		baseaddr3=0;
		baseaddr4=0;
		baseaddr5=0;
		baseaddr6=0;
		baseaddr7=0;

		ACSCGFPGACore* base1=construct->add_const(baseaddr1,SIGNED,white_stars);
		ACSCGFPGACore* base2=construct->add_const(baseaddr2,SIGNED,white_stars);
		ACSCGFPGACore* base3=construct->add_const(baseaddr3,SIGNED,white_stars);
		ACSCGFPGACore* base4=construct->add_const(baseaddr4,SIGNED,white_stars);
		ACSCGFPGACore* base5=construct->add_const(baseaddr5,SIGNED,white_stars);
		ACSCGFPGACore* base6=construct->add_const(baseaddr6,SIGNED,white_stars);
		ACSCGFPGACore* base7=construct->add_const(baseaddr7,SIGNED,white_stars);
		ACSCGFPGACore* reg=construct->add_sg("ACS","Register","CGFPGA",BOTH,SIGNED,white_stars);
		ACSCGFPGACore* mux=construct->add_mux(7,7,SIGNED,white_stars);

		ACSCGFPGACore* adder1=construct->add_sg("ACS","Add","CGFPGA",BOTH,SIGNED,white_stars);
		ACSCGFPGACore* adder2=construct->add_sg("ACS","Add","CGFPGA",BOTH,SIGNED,white_stars);

		// BELOW ASSUMES INPUT PINS OF SUBTRACTOR ARE
		// 0-POS 1-NEG
		// ASSUMES OUTPUT OF MUX IS PIN 0, FOLLOWED BY 7 INPUT ON
		// PINS 1-7
		// Hook up internals
		construct->connect_sg(negativefive,UNASSIGNED,UNASSIGNED,subtractor,1,UNASSIGNED,DATA_NODE);
		construct->connect_sg(subtractor,timeseighty);
		construct->connect_sg(timeseighty,UNASSIGNED,UNASSIGNED,adder1,0,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base1,UNASSIGNED,UNASSIGNED,mux,1,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base2,UNASSIGNED,UNASSIGNED,mux,2,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base3,UNASSIGNED,UNASSIGNED,mux,3,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base4,UNASSIGNED,UNASSIGNED,mux,4,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base5,UNASSIGNED,UNASSIGNED,mux,5,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base6,UNASSIGNED,UNASSIGNED,mux,6,UNASSIGNED,DATA_NODE);
		construct->connect_sg(base7,UNASSIGNED,UNASSIGNED,mux,7,UNASSIGNED,DATA_NODE);
		// This may not work exactly as is...
		construct->connect_sg(reg,mux,INPUT_PIN_CTRL);
		construct->connect_sg(mux,UNASSIGNED,UNASSIGNED,adder1,1,UNASSIGNED,DATA_NODE);
		construct->connect_sg(adder1,UNASSIGNED,UNASSIGNED,adder2,0,UNASSIGNED,DATA_NODE);
		//
		// Hook up externals
		//

		// Input pin
		connections->add(-1,-1,-1,
				 0,subtractor->acs_id,0);

		// Output pin
		connections->add(-1,-1,-1,
				 1,adder2->acs_id,adder2->find_hardpin(OUTPUT_PIN));

		// Clock pin
	        // Left to resolver

		// Ce pin
		connections->add(-1,-1,-1,
				 3,subtractor->acs_id,subtractor->find_hardpin(INPUT_PIN_CE));
		connections->add(-1,-1,-1,
				 3,adder1->acs_id,adder1->find_hardpin(INPUT_PIN_CE));
		connections->add(-1,-1,-1,
				 3,adder2->acs_id,adder2->find_hardpin(INPUT_PIN_CE));
		connections->add(-1,-1,-1,
				 3,reg->acs_id,reg->find_hardpin(INPUT_PIN_CE));
		// Clr pin
		connections->add(-1,-1,-1,
				 4,subtractor->acs_id,subtractor->find_hardpin(INPUT_PIN_CLR));
		connections->add(-1,-1,-1,
				 4,adder1->acs_id,adder1->find_hardpin(INPUT_PIN_CLR));
		connections->add(-1,-1,-1,
				 4,adder2->acs_id,adder2->find_hardpin(INPUT_PIN_CLR));
		connections->add(-1,-1,-1,
				 4,reg->acs_id,reg->find_hardpin(INPUT_PIN_CLR));


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
                schedule_file << " racts1=[0 1 vl1-1 ; 3 1 vl1+2];" << endl;
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
		acs_delay=3;

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
		pins->add_pin("a","input",INPUT_PIN);

		// Output port definitions
		pins->add_pin("b","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",0,1,INPUT_PIN_CLK);
		pins->add_pin("ce",0,1,INPUT_PIN_CE,AH);
		pins->add_pin("clr",0,1,INPUT_PIN_CLR,AL);

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
		    pins->set_datatype(0,STD_LOGIC);  // a port
		    pins->set_min_vlength(0,2);
		    pins->set_max_vlength(0,32);
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC); // b port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // clk pin
		    pins->set_datatype(3,STD_LOGIC);  // ce pin
		    pins->set_datatype(4,STD_LOGIC);  // clr pin
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);

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

