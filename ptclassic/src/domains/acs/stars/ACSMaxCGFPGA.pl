defcore {
	name { Max }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Max }
	desc { Produced the maximum of two inputs }
	version {@(#)ACSMaxCGFPGA.pl	1.0 12/13/00}
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
	    name {Input1_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Input1_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Input2_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Input2_Bit_Length}
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
	    arglist { "(int* free_id, int* free_netid)" }
	    type {"CoreList*"}
	    code {
		CoreList* white_stars=new CoreList;
		Sg_Constructs* construct=new Sg_Constructs(free_id,free_netid,dest_dir);
	        connections=new Connection_List;

		ACSCGFPGACore* comparator=construct->add_sg("ACS","Compare","CGFPGA",BOTH,SIGNED,white_stars);
		comparator->add_comment("Comparator",name());
		ACSCGFPGACore* mux=construct->add_mux(2,2,SIGNED,white_stars);
		mux->add_comment("Switch",name());
		ACSCGFPGACore* d0_reg=construct->add_sg("ACS","UnitDelay","CGFPGA",BOTH,SIGNED,white_stars);
		ACSCGFPGACore* d1_reg=construct->add_sg("ACS","UnitDelay","CGFPGA",BOTH,SIGNED,white_stars);

		// Establish comparator output priorities to match ctrlpins
		Pin* comparator_pins=comparator->pins;
		int out_pin=comparator_pins->retrieve_type(OUTPUT_PIN);
		comparator_pins->set_pinpriority(out_pin,0);
		comparator->bitslice_strategy=PRESERVE_MSB;

		// Hook up internals
		int result_pin=comparator->find_hardpin(OUTPUT_PIN);
		int ctrl_pin=mux->find_hardpin(INPUT_PIN_CTRL);
		mux->replace_pintype(ctrl_pin,INPUT_PIN);
		construct->connect_sg(comparator,result_pin,UNASSIGNED,
				      mux,ctrl_pin,UNASSIGNED,DATA_NODE);

		construct->connect_sg(d0_reg,mux,INPUT_PIN_MUX_SWITCHABLE);
		construct->connect_sg(d1_reg,mux,INPUT_PIN_MUX_SWITCHABLE);
/*
		construct->connect_sg(d0_reg,NULL,NULL,
				      mux,UNASSIGNED,UNASSIGNED,INPUT_PIN_MUX_SWITCHABLE,
				      DATA_NODE);
		construct->connect_sg(d1_reg,NULL,NULL,
				      mux,UNASSIGNED,UNASSIGNED,INPUT_PIN_MUX_SWITCHABLE,
				      DATA_NODE);
*/

		// FIX: Buffer insertion seems to be confused about there being a priority on the comparator pin:(
		// Since connected reset the pin priority
		comparator_pins->set_pinpriority(out_pin,-1);

		//
		// Hook up externals
		//

		// BELOW ASSUMES PIN NUMBERS OF A 2-1 MUX AS FOLLOWS:
		// 0: OUTPUT
		// 1: INPUT0
		// 2: INPUT1
		// 3: CTRL
		// ALSO ASSUMES INPUTS OF COMPARE ARE PINS 0,1

		// Input pin
		connections->add(-1,-1,-1,
				 0,comparator->acs_id,0);
		connections->add(-1,-1,-1,
				 0,d0_reg->acs_id,0);
		connections->add(-1,-1,-1,
				 1,comparator->acs_id,1);
		connections->add(-1,-1,-1,
				 1,d1_reg->acs_id,0);
		
		// Output pin
		connections->add(2,mux->acs_id,0,
				 -1,-1,-1);

		// Clock pin
	        // Left to resolver

		// Ce pin
		connections->add(-1,-1,-1,
				 4,comparator->acs_id,comparator->find_hardpin(INPUT_PIN_CE));
		// Clr pin
		connections->add(-1,-1,-1,
				 5,comparator->acs_id,comparator->find_hardpin(INPUT_PIN_CLR));

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
		input_list->add("Input1_Major_Bit");
		input_list->add("Input1_Bit_Length");
		input_list->add("Input2_Major_Bit");
		input_list->add("Input2_Bit_Length");
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
		cost_file << "wl=max(msbranges(1:2)'*ones(1,size(insizes,2))-insizes+1);" << endl;
		cost_file << "wu=max(msbranges(1:2)');" << endl;
		cost_file << "cost=ceil((wu-wl+1)/2);" << endl;

		// numsim_file << "y= x(:,1) < x(:,2);" << endl;
		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=max(x{1,k},x{2,k}); " << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;


		rangecalc_file << "orr=[max(inputrange(:,1)) max(inputrange(:,2))];" << endl;
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

		// this is ok because compare latency does not depend on wordlength
		schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << " racts1=[0 1 vl1-1 ;0 1 vl1-1; 1 1 vl1];" << endl;
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
		acs_delay=1;

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
		pins->add_pin("a","input1",INPUT_PIN);
		pins->add_pin("b","input2",INPUT_PIN);

		// Output port definitions
		pins->add_pin("m","output",OUTPUT_PIN);

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
		    pins->set_datatype(1,STD_LOGIC);  // b port
		    pins->set_min_vlength(1,2);
		    pins->set_max_vlength(1,32);
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC); // m port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(3,STD_LOGIC);  // clk pin
		    pins->set_datatype(4,STD_LOGIC);  // ce pin
		    pins->set_datatype(5,STD_LOGIC);  // clr pin
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);

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

