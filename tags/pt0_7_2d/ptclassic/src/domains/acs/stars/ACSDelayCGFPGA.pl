defcore {
	name { Delay }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Delay }
	desc {
	    Generates a single delay for multiple lines
	}
	version{ @(#)ACSDelayCGFPGA.pl	1.12 08/02/01 }
	author { K. Smith }
	copyright {
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
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
	    Connection_List* connections;
	}
	constructor {
	    connections=NULL;
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
	    name {macro_query}
	    access {public}
	    type {int}
	    code {
		if (DEBUG_BUILD)
		    printf("ACSDelay:Macro_query for %s, target=%d, pipe=%d, acs_delay=%d\n",
			   name(),
			   target_implementation,
			   pipe_delay,
			   acs_delay);
		// BEGIN-USER CODE
		if ((target_implementation==0) && (acs_delay > 1))
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
		// NOTE:This is only applicable for implementation #0
		ACSCGFPGACore* base_delay=construct->
		    add_sg("ACS","UnitDelay","CGFPGA",BOTH,SIGNED,white_stars);
		ACSCGFPGACore* prev_delay=base_delay;
		ACSCGFPGACore* next_delay=NULL;

		// Connect external input
                connections->add(-1,-1,-1,
				 0,base_delay->acs_id,-1);

		// Connect initial chip enable
		connections->add(-1,-1,-1,
				 3,base_delay->acs_id,base_delay->find_hardpin(INPUT_PIN_CE));

		// Notify for external connection of the Clr pin
		connections->add(-1,-1,-1,
				 4,base_delay->acs_id,base_delay->find_hardpin(INPUT_PIN_CLR));

		for (int loop=1;loop<acs_delay;loop++)
		{
		    next_delay=construct->add_sg("ACS","UnitDelay","CGFPGA",
						 BOTH,SIGNED,white_stars);
		    construct->connect_sg(prev_delay,next_delay);
		    prev_delay=next_delay;

		    // Connect chip enable
		    connections->add(-1,-1,-1,
				     3,next_delay->acs_id,
				     next_delay->find_hardpin(INPUT_PIN_CE));

		    // Notify for external connection of the Clr pin
		    connections->add(-1,-1,-1,
				     4,next_delay->acs_id,next_delay->find_hardpin(INPUT_PIN_CLR));
		}

		// Connect external output
                connections->add(1,next_delay->acs_id,-1,
				 -1,-1,-1);

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
		if (target_implementation==0)
		{
		    cost_file << "cost=ceil(0.5*insizes).*" << pipe_delay << ";" << endl;

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
		    schedule_file << "  disp('ERROR - use parallel numeric form only' )  " 
			          << endl;
		    schedule_file << " end " << endl;
		}
		else // target_implementation==1
		{
		    cost_file << "cost=ceil(" << pipe_delay << "/16) * "
  		              << "ceil(0.5*insizes) + " 
			      << "ceil(log2(" << pipe_delay << "));" << endl;

		    numsim_file <<  " y=cell(1,size(x,2));" << endl;
		    numsim_file <<  " for k=1:size(x,2) " << endl;
		    numsim_file <<  "   y{k}=x{k};" << endl;
		    numsim_file <<  " end " << endl;
		    numsim_file <<  " " << endl;

		    rangecalc_file << "orr=inputrange;" << endl;
		    
		    natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

		    // this is ok because ram delay latency does not depend on wordlength
		    schedule_file << " vl1=veclengs(1); " << endl;
		    schedule_file << "outdel=" << pipe_delay << ";" << endl;
		    schedule_file << "racts=cell(1,size(insizes,2));" << endl;
		    schedule_file << "for k=1:size(insizes,2)" << endl;
		    schedule_file << "  racts1=[0 1 vl1-1 ; outdel(k) 1 vl1-1+outdel(k)];" 
			          << endl;
		    schedule_file << "  racts{k}=racts1;" << endl;
		    schedule_file << "end"  << endl;
		    schedule_file << "minlr=vl1*ones(1,size(insizes,2)); " << endl;
		    schedule_file << "if sum(numforms)>0 " << endl;
		    schedule_file << "  disp('ERROR - use parallel numeric form only' )  " 
			          << endl;
		    schedule_file << "end " << endl;
		}
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

	// FIX:Coded for XC4000
	// Standard design will use D-type registers
	// Design 1 will use a RamBlock
	method {
	    name {sg_designs}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		if (DEBUG_BUILD)
		    printf("ACSDelay:sg_designs target=%d, pipe=%d, acs_delay=%d\n",
			   target_implementation,
			   pipe_delay,
			   acs_delay);
		if (implementation_lock==UNLOCKED)
		{
		    implementation_lock=lock_mode;

		    float cost0=0;
		    float cost1=0;
		    int bitwidth=pins->query_bitlen(0);
		    
		    if (DEBUG_BUILD)
			printf("bitwidth=%d\n",bitwidth);

		    // Test for trivial solution for implementation choice
		    if ((bitwidth==1) && (acs_delay==1))
		    {
			target_implementation=0;
			return(1);
		    }
		    if (acs_delay > 17)
		    {
			target_implementation=1;
			return(1);
		    }
		
		    cost0=(bitwidth*corona.delays)/2;
		    if (acs_delay < 5)
			cost1=ceil(bitwidth/2)+1;
		    else
		    {
			cost1=(4+(bitwidth-2)/2);
			if ((bitwidth % 2) != 0)
			    cost1+=0.5;
		    }
		    if (DEBUG_BUILD)
			printf("cost1=%f, cost0=%f\n",cost1,cost0);

		    if (cost1 > cost0)
			target_implementation=1;
		    else
			target_implementation=0;
		}   
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
		if (acs_origin==USER_GENERATED)
		    acs_delay=corona.delays;

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

		if (fpga_type()==XC4000)
		{
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
		}
		else if (fpga_type()==VIRTEX)
		{
		    // Input port definitions
		    pins->add_pin("D","input",INPUT_PIN);
		    pins->set_min_vlength(0,1);
		    pins->set_max_vlength(0,64);

		    // Output port definitions
		    pins->add_pin("Q","output",OUTPUT_PIN);

		    // Bidir port definitions
		
		    // Control port definitions
		    pins->add_pin("CLK",INPUT_PIN_CLK);
		}
		else
		    fprintf(stderr,"ERROR:ACSDelayCGFPGA invoked for unknown fpga type!\n");

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
		    pins->set_precision(2,1,1,LOCKED);

		    if (fpga_type()==XC4000)
		    {
			pins->set_datatype(3,STD_LOGIC);  // ce pin
			pins->set_precision(3,1,1,LOCKED);
		    
			pins->set_datatype(4,STD_LOGIC);  // clr pin
			pins->set_precision(4,1,1,LOCKED);
		    }
		    // END-USER CODE
		}
		else
		    return(0);

	        // Return happy condition
		return(1);
	    }
	}
	method {
	    name {revisit_pins}
	    access {public}
	    type {int}
	    code {
		// Should an alternate target implementation be chosen, pins may need to change
		if (DEBUG_BUILD)
		    printf("Core %s, revisit pins, fpga_type=%d, target=%d\n",name(),fpga_type(),target_implementation);
		if ((fpga_type()==XC4000) && (target_implementation==1))
		{
		    pins->change_pinname(0,"din");
		    pins->change_pinname(1,"dout");
		    pins->set_skip(4,1);
		}
		if (fpga_type()==VIRTEX)
		{
		    pins->change_pinname(0,"D");
		    pins->change_pinname(1,"Q");
		    pins->change_pinname(2,"CLK");
		}


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
		    // Added .xco for Coregen 2.1+
		    output_filename << ends;
		    ostrstream filename;
		    filename << dest_dir << tolowercase(output_filename.str()) << ".xco" << ends;
		    ofstream out_fstr(filename.str());

		    // Build instructions
		    black_box=1;

/*		    
		    // Needed for Coregen 2.1+
		    ostrstream corelib_filename;
		    corelib_filename << dest_dir << "coregen.prj" << ends;
		    ofstream corelib_fstr(corelib_filename.str());
		    ostrstream core_edif;
		    ostrstream core_vho;
		    ostrstream core_asy;
		    core_edif << output_filename.str() << ".edn" << ends;
		    core_vho << output_filename.str() << ".vho" << ends;
		    core_asy << output_filename.str() << ".asy" << ends;

		    printf("sg_build fpga_type=%d\n",fpga_type());

		    int type=fpga_type();
		    if (type==XC4000)
		    {
			// Library info
			corelib_fstr << "Register|xilinx|xc4000_all|1.0=active" << endl;
			corelib_fstr << "Delay_Element|xilinx|xc4000_all|1.0=active" << endl;

			// Core generator info
			out_fstr << "SET BusFormat = BusFormatParen" << endl;
			out_fstr << "SET SimulationOutputProducts = VHDL" << endl;
			out_fstr << "SET ViewlogicLibraryAlias = primary" << endl;
			out_fstr << "SET XilinxFamily = XC4000" << endl;
			out_fstr << "SET DesignFlow = VHDL" << endl;
			out_fstr << "SET FlowVendor = Synplicity" << endl;
			if (target_implementation==0)
			    out_fstr << "SELECT Register  XC4000 Xilinx 1.0" << endl;
			else
			{
			    out_fstr << "SELECT Delay_Element  XC4000 Xilinx 1.0" << endl;
			    out_fstr << "CSET pipeline_stages = " << pipe_delay << endl;
			}
			out_fstr << "CSET port_width = " << pins->query_bitlen(0) << endl;
			out_fstr << "CSET component_name = " << tolowercase(output_filename.str()) << endl;
			out_fstr << "GENERATE" << endl;
		    }
		    else if (type==VIRTEX)
		    {
			// Library info
			corelib_fstr << "FD-based_Parallel_Register|xilinx|virtex_all|1.0=active" << endl;
			corelib_fstr << "RAM-based_Shift_Register|xilinx|virtex_all|1.0=active" << endl;

			// Core generator info
			out_fstr << "SET BusFormat = BusFormatParen" << endl;
			out_fstr << "SET SimulationOutputProducts = VHDL" << endl;
			out_fstr << "SET ViewlogicLibraryAlias = primary" << endl;
			out_fstr << "SET XilinxFamily = Virtex" << endl;
			out_fstr << "SET DesignFlow = VHDL" << endl;
			out_fstr << "SET FlowVendor = Synplicity" << endl;
			if (target_implementation==0)
			{
			    out_fstr << "SELECT FD-based_Parallel_Register  Virtex Xilinx,_Inc. 1.0" << endl;
			    out_fstr << "CSET ce_overrides = sync_controls_override_ce" << endl;
			    out_fstr << "CSET set_clear_priority = clear_overrides_set" << endl;
			    out_fstr << "CSET asynchronous_settings = none" << endl;
			    out_fstr << "CSET sync_init_value = 0" << endl;
			    out_fstr << "CSET async_init_value = 0" << endl;
			}

			else
			{
			    out_fstr << "SELECT RAM-based_Shift_Register  Virtex Xilinx,_Inc. 1.0" << endl;
			    out_fstr << "CSET shift_type = Fixed_Length" << endl;
			    out_fstr << "CSET depth = " << pipe_delay << endl;
			    out_fstr << "CSET synchronous_init_value = 0" << endl;
			    out_fstr << "CSET asynchronous_init_value = 0" << endl;
			}

			out_fstr << "CSET component_name =" << tolowercase(output_filename.str()) << endl;
			out_fstr << "CSET data_width = " << pins->query_bitlen(0) << endl;
			out_fstr << "CSET clock_enable = FALSE" << endl;
			out_fstr << "CSET create_rpm = TRUE" << endl;
			out_fstr << "GENERATE" << endl;
		    }

		    corelib_fstr.close();
		    out_fstr.close();

                    if (fork()==0)
		    {
			chdir(dest_dir);
			
			// Coregen cannot overwrite files from previous runs, delete them here
			if (execlp("rm",
				   "rm",
				   core_edif.str(),
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, unable to remove old edif file\n");
		    }
		    else
			wait ((int*) 0);
//		    if (access(core_vho.str(),0)!=-1)
                    if (fork()==0)
		    {
			chdir(dest_dir);
			
			if (execlp("rm",
				   "rm",
				   core_vho.str(),
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, unable to remove old vho file\n");
		    }
		    else
			wait ((int*) 0);
                    if (fork()==0)
		    {
			chdir(dest_dir);
			
			if (execlp("rm",
				   "rm",
				   core_asy.str(),
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, unable to remove old asy file\n");
		    }
		    else
			wait ((int*) 0);
		    
		    if (fork()==0)
		    {
			chdir(dest_dir);
			
			if (execlp("coregen",
				   "coregen",
				   "-b",
				   filename.str(),
				   "-p",
				   ".",
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, unable to spawn a coregen session\n");
		    }
	            else
			wait ((int*) 0);
*/

// Old code for Coregen 1.5, XC4000 only
		    out_fstr << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_fstr << "SET XilinxFamily = All_XC4000_Families" 
			     << endl;
		    out_fstr << "SET BusFormat = BusFormatAngleBracket" 
			     << endl;
		    out_fstr << "SET TargetSymbolLibrary = primary" << endl;
		    out_fstr << "SET OVERWRITEFILES=true" << endl;
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/";
		    if (target_implementation==0)
			out_fstr << "Basic Elements/Register" << endl;
		    else
		    {
			out_fstr << "Memories/Delay Element" << endl;
			out_fstr << "GSET Pipeline_Stages = " << pipe_delay << endl;
			out_fstr << "GSET Create_RPM = true" << endl;
		    }
		    out_fstr << "GSET Port_Width = " << pins->query_bitlen(0) << endl;
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

