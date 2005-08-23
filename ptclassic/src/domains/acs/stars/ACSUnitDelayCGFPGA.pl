defcore {
	name { UnitDelay }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { UnitDelay }
	desc {
	    Generates a single delay for multiple lines
	}
	version {@(#)ACSUnitDelayCGFPGA.pl	1.4 09/10/99}
	author { K. Smith }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
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

		// numsim_file << "y=x;" << endl;
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k};" << endl;
                numsim_file <<  " end " << endl;
                numsim_file <<  " " << endl;

		rangecalc_file << "orr=inputrange;" << endl;
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

                // this is ok because single delay latency does not depend on wordlength
                schedule_file << " vl1=veclengs(1); " << endl;
                schedule_file << " racts1=[0 1 vl1-1; 1 1 vl1];" << endl;
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
		if (pins->query_preclock(1)==UNLOCKED)
		    pins->set_precision(1,
					pins->query_majorbit(0),
					pins->query_bitlen(0),
					UNLOCKED);
		if (pins->query_preclock(0)==LOCKED)
		    pins->set_precision(1,
					pins->query_majorbit(0),
					pins->query_bitlen(0),
					lock_mode);

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
		int input_width=pins->query_bitlen(0);
		if (input_width > 32)
		    return(WHITE_STAR);
		else
		    return(NORMAL_STAR);
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

		// BEGIN-USER CODE
		int input_width=pins->query_bitlen(0);
		int delay_count=(int) ceil(input_width/32);
//		printf("UnitDelay delay_count=%d\n",delay_count);
		
		//FIX: Why isnt some of this functionality delagated to the UnitDelay smart generator?
		ACSCGFPGACore* unpacker_core=construct->add_unpacker(delay_count,white_stars);
		ACSCGFPGACore* packer_core=construct->add_packer(delay_count,white_stars);
		unpacker_core->add_comment("Unpacker for oversized delay element",name());
		packer_core->add_comment("Packer for oversized delay element",name());
		unpacker_core->bitslice_strategy=PRESERVE_LSB;
		packer_core->bitslice_strategy=PRESERVE_LSB;
		unpacker_core->bw_exempt=1;
		packer_core->bw_exempt=1;
		for (int loop=0;loop<delay_count;loop++)
		{
		    ACSCGFPGACore* delay_core=construct->add_sg("ACS","UnitDelay","CGFPGA",
								BOTH,SIGNED,white_stars);
		    construct->connect_sg(unpacker_core,delay_core);
		    construct->connect_sg(delay_core,packer_core);

		    // Notify for external connection of the Ce pin
		    connections->add(-1,-1,-1,
				     3,delay_core->acs_id,delay_core->find_hardpin(INPUT_PIN_CE));

		    // Notify for external connection of the Clr pin
		    connections->add(-1,-1,-1,
				     4,delay_core->acs_id,delay_core->find_hardpin(INPUT_PIN_CLR));
		}

		// Hook up the externals
		// NOTE:Clk should bind properly
		// FIX:Once multiple clocks, binding should be made local here and then
		//     left to the parent to resolve
		// D pin
		connections->add(0,unpacker_core->acs_id,unpacker_core->find_hardpin(OUTPUT_PIN),
				 -1,-1,-1);
		// Q pin
		connections->add(-1,-1,-1,
				 1,packer_core->acs_id,packer_core->find_hardpin(INPUT_PIN_SET));

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
		output_filename << name();

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=BOTH;
		acs_existence=HARD;

	        // Input port definitions
		pins->add_pin("d","input",INPUT_PIN);
		pins->set_min_vlength(0,2);
		pins->set_max_vlength(0,32);

		// Output port definitions
		pins->add_pin("q","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_CE,AH);
		pins->add_pin("clr",INPUT_PIN_CLR,AL);

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
		    pins->set_datatype(0,STD_LOGIC);  // d port
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC); // q port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // clk pin
		    pins->set_datatype(3,STD_LOGIC);  // ce pin
		    pins->set_datatype(4,STD_LOGIC);  // clr pin
		    pins->set_precision(2,1,1,LOCKED);
		    pins->set_precision(3,1,1,LOCKED);
		    pins->set_precision(4,1,1,LOCKED);
		    // END-USER CODE
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
		if (DEBUG_BUILD)
		    printf("Building %s\n",output_filename.str());

		// Trap for language and generate appropriate code
	        if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    output_filename << ends;
		    ostrstream filename;
		    filename << dest_dir << tolowercase(output_filename.str()) << ends;
		    ofstream out_fstr(filename.str());

		    // Build instructions
		    black_box=1;

		    out_fstr << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_fstr << "SET XilinxFamily = All_XC4000_Families" 
			     << endl;
		    out_fstr << "SET BusFormat = BusFormatAngleBracket" 
			     << endl;
		    out_fstr << "SET TargetSymbolLibrary = primary" << endl;
		    out_fstr << "SET OVERWRITEFILES=true" << endl;
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Basic Elements/Register" 
			     << endl;
		    out_fstr << "GSET Port_Width = " 
			     << pins->query_bitlen(0) 
			     << endl;
		    out_fstr << "GSET Component_Name = " 
			     << tolowercase(output_filename.str()) << endl;
		    out_fstr << "GENERATE" << endl;
		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   filename.str(),
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, Unable to spawn a coregen session\n");

		    }
		    else
			wait((int*) 0);
		}
                // END-USER CODE
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

