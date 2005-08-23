defcore {
	name { Add }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Add }
	desc {
	    Sums two inputs
	    Only the first two connections are used for hardware
	    }
	version{ @(#)ACSAddCGFPGA.pl	1.17 08/02/01 }
	author { K. Smith }
	copyright {
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
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
		input_list->add("Input1_Major_Bit");
		input_list->add("Input1_Bit_Length");
		input_list->add("Input2_Major_Bit");
		input_list->add("Input2_Bit_Length");
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
		cost_file << "wl=max(msbranges(1:2)'*ones(1,size(insizes,2))-insizes+1);" << endl;
		cost_file << "wu=max(msbranges(1:2)');" << endl;
		cost_file << "cost=ceil((wu-wl+1)/2);" << endl;
		cost_file << " if sum(numforms)>0 " << endl;
		cost_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
		cost_file << " end " << endl;

		//  numsim_file << "y=sum(x);" << endl; 
		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=x{1,k}+x{2,k}; " << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;

		rangecalc_file << "orr=sum(inputrange);" << endl;

		natcon_file << "wi=min(msbranges(1:2)'*ones(1,size(insizes,2)) -insizes+1);" << endl;
		natcon_file << "wo=msbranges(3)-outsizes+1;" << endl;
		natcon_file << "yesno=(wo>=wi);" << endl;
		natcon_file << "yesno=yesno & (max(insizes)<33) & (min(insizes)>1);" << endl;

		// this is ok because adder latency does not depend on wordlength
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
		//
		// Calculate BW
		//
		int S_bitlen=0;

		if (pins->query_preclock(2)==UNLOCKED)
		{
		    int A_majorbit=pins->query_majorbit(0);
		    int A_bitlen=pins->query_bitlen(0);
		    int B_majorbit=pins->query_majorbit(1);
		    int B_bitlen=pins->query_bitlen(1);
		    
		    int S_majorbit=(int) max(A_majorbit,B_majorbit)+1;
		    S_bitlen=(int) abs(S_majorbit - 
				       min((A_majorbit-A_bitlen),(B_majorbit-B_bitlen)));
		    
		    // Set
		    pins->set_precision(2,S_majorbit,S_bitlen,lock_mode);
		}

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


		if (fpga_type()==XC4000)
		{
		    // Input port definitions
		    pins->add_pin("a","input#1",INPUT_PIN);
		    pins->add_pin("b","input#2",INPUT_PIN);

		    // Output port definitions
		    pins->add_pin("s","output",OUTPUT_PIN);

		    // Bidir port definitions
		
		    // Control port definitions
		    pins->add_pin("c",0,1,INPUT_PIN_CLK);
		    pins->add_pin("ce",0,1,INPUT_PIN_CE,AH);
		    pins->add_pin("ci",0,1,INPUT_PIN_CARRY,AL);
		    pins->add_pin("clr",0,1,INPUT_PIN_CLR,AL);
		}
		else if (fpga_type()==VIRTEX)
		{
		    // Input port definitions
		    pins->add_pin("A","input#1",INPUT_PIN);
		    pins->add_pin("B","input#2",INPUT_PIN);

		    // Output port definitions
		    pins->add_pin("Q","output",OUTPUT_PIN);

		    // Bidir port definitions
		
		    // Control port definitions
		    pins->add_pin("CLK",0,1,INPUT_PIN_CLK);
		    pins->add_pin("CE",0,1,INPUT_PIN_CE,AH);
		    pins->add_pin("C_IN",0,1,INPUT_PIN_CARRY,AL);
		    pins->add_pin("SCLR",0,1,INPUT_PIN_CLR,AL);
		}
		else
		    fprintf(stderr,"ERROR:ACSAddCGFPGA invoked for unknown fpga type!\n");

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
	method {
	    name {revisit_pins}
	    access {public}
	    type {int}
	    code {
		// Should an alternate target implementation be chosen, pins may need to change
		// ASSUMPTION: Core fpga_type still defaults to XC4000 in constructor
		if (fpga_type()==VIRTEX)
		{
		    pins->change_pinname(0,"A");
		    pins->change_pinname(1,"B");
		    pins->change_pinname(2,"Q");
		    pins->change_pinname(3,"CLK");
		    pins->change_pinname(4,"CE");
		    pins->change_pinname(5,"C_IN");
		    pins->change_pinname(6,"SCLR");
		}
		
		// Return happy condition
		return(1);
	    }
	}        method {
	    name {acs_build}
	    access {public}
	    type {int}
	    code {
		// Trap for language and generate appropriate code
		// if (fpga_type()==XC4000)
		//{
		//}
		// else if (fpga_type()==VIRTEX)
	        //{
		//}    
		// else
		// fprintf(stderr,"ACSAddCGFPGA::Unknown device targetted\n");
		if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    Pin* new_pins=NULL;
		    ostrstream core_entity;

		    // Retrieve requests
		    int A_majorbit=pins->query_majorbit(0);
		    int A_bitlen=pins->query_bitlen(0);
		    int B_majorbit=pins->query_majorbit(1);
		    int B_bitlen=pins->query_bitlen(1);
		    int S_bitlen=pins->query_bitlen(2);

		    // Calculate sign extensions
		    int MSB=(int) max(A_majorbit,B_majorbit);
		    int extend_A=MSB-A_majorbit;
		    int extend_B=MSB-B_majorbit;

		    // LSB padding
		    int LSB=(int) min(A_majorbit-A_bitlen,B_majorbit-B_bitlen);
		    int pad_A=A_majorbit-A_bitlen-LSB;
		    int pad_B=B_majorbit-B_bitlen-LSB;

		    // Calculate (new?) adder length
		    int adder_length=MSB-LSB;

		    
		    // Generate new port definition
		    // from existing parameters
                    new_pins=dup_pins();  
		    new_pins->set_precision(0,MSB,adder_length,LOCKED);
		    new_pins->set_precision(1,MSB,adder_length,LOCKED);
		    new_pins->set_precision(2,MSB+1,adder_length+1,LOCKED);

		    VHDL_LANG* lang=new VHDL_LANG;
		    ostrstream statements;
		    ostrstream patch_filename;
			
		    core_entity << "ACSpatch" << output_filename.str() << ends;
		    patch_filename << dest_dir << output_filename.str() 
			           << ".vhd" << ends;
		    ofstream patch_fstr(patch_filename.str());
			
		    // Coregen tool requires matched inputs, add padding-logic
		    patch_fstr << "library IEEE;" << endl;
		    patch_fstr << "use IEEE.std_logic_1164.all;" << endl << endl;
		    patch_fstr << lang->gen_entity(output_filename.str(),pins) 
			       << endl;
		    patch_fstr << lang->gen_architecture(name(),
							 NULL,
							 STRUCTURAL,
							 pins,
							 data_signals,
							 ctrl_signals,
							 constant_signals);
		    patch_fstr << lang->start_component(core_entity.str());
		    patch_fstr << lang->start_port() << endl;
		    patch_fstr << lang->set_port(new_pins) << endl;
		    patch_fstr << lang->end_port() << endl;
		    patch_fstr << lang->end_component() << endl;
			
		    patch_fstr << lang->signal("in_a",STD_LOGIC,adder_length);
		    patch_fstr << lang->signal("in_b",STD_LOGIC,adder_length);
		    patch_fstr << lang->signal("out_s",STD_LOGIC,adder_length+1);
			
		    // Correct Input A
		    int a_bitlen=pins->query_bitlen(0);
		    statements << lang->equals(lang->slice("in_a",
							   pad_A+a_bitlen-1,
							   pad_A),
					       pins->query_pinname(0))
			       << lang->end_statement << endl;
                    int loop;
		    for (loop=0;loop < extend_A;loop++)
			statements << lang->equals(lang->
						   slice("in_a",loop+a_bitlen+pad_A),
						   lang->
						   slice("a",A_bitlen-1))
				
			           << lang->end_statement << endl;
		    for (loop=0;loop < pad_A;loop++)
			statements << lang->equals(lang->slice("in_a",loop),"GND")
			    << lang->end_statement << endl;
			
			
		    // Correct Input B
		    int b_bitlen=pins->query_bitlen(1);
		    statements << lang->equals(lang->slice("in_b",
							   pad_B+b_bitlen-1,
							   pad_B),
					       pins->query_pinname(1))
			       << lang->end_statement << endl;
		    for (loop=0;loop < extend_B;loop++)
			statements << lang->equals(lang->
						   slice("in_b",loop+b_bitlen+pad_B),
						   lang->
						   slice("b",b_bitlen-1))
			           << lang->end_statement << endl;
		    for (loop=0;loop < pad_B;loop++)
			statements << lang->equals(lang->slice("in_b",loop),"GND")
			           << lang->end_statement << endl;
			
			
		    // Correct Output S
                    if (S_bitlen-1 != adder_length)
		    {
			if (S_bitlen-1 > adder_length)
			{
			    statements << lang->slice(pins->query_pinname(2),adder_length,0)
				       << lang->equals() 
				       << "out_s"
				       << lang->end_statement << endl;
			    for (int bloop=adder_length+1;bloop<S_bitlen;bloop++)
				statements << lang->slice(pins->query_pinname(2),bloop,bloop)
				           << lang->equals() 
				           << lang->slice("out_s",adder_length,adder_length)
				           << lang->end_statement << endl;
			}
			else
			{
			    if (bitslice_strategy==PRESERVE_LSB)
				statements << lang->equals(pins->query_pinname(2),
							   lang->slice("out_s",S_bitlen-1,0))
				           << lang->end_statement << endl;
			    else
				statements << lang->equals(pins->query_pinname(2),
							   lang->slice("out_s",adder_length,
								       adder_length-S_bitlen+1))
				           << lang->end_statement << endl;
			}
		    }
		    else
			statements << lang->equals(pins->query_pinname(2),"out_s")
			           << lang->end_statement << endl;
						   

			
		    statements << "U_" << output_filename.str() << ":" 
			       << core_entity.str() << " port map(" << endl;

		    if (fpga_type()==XC4000)
		    {
			statements << "a=>in_a," << endl;
			statements << "b=>in_b," << endl;
			statements << "s=>out_s," << endl;
			statements << "c=>c," << endl
			           << "ce=>ce," << endl
				   << "ci=>ci," << endl
				   << "clr=>clr);" << endl;
		    }
		    else if (fpga_type()==VIRTEX)
		    {
			statements << "A=>in_a," << endl;
			statements << "B=>in_b," << endl;
			statements << "Q=>out_s," << endl;
			statements << "CLK=>CLK," << endl
			           << "CE=>CE," << endl
				   << "C_IN=>C_IN," << endl
				   << "SCLR=>SCLR);" << endl;
		    }
		    else
			statements << "-- ERROR  Unknown fpga type requested" << endl;

		    statements << ends;
		    patch_fstr << lang->begin_scope << endl;
		    patch_fstr << statements.str();
		    patch_fstr << lang->end_architecture(STRUCTURAL) << endl;
		    delete lang;
			
		    // Build instructions
		    ostrstream child_filename;
		    child_filename << core_entity.str() << ".vhd" << ends;
		    child_filenames->add(tolowercase(child_filename.str()));

		    // Added .xco for Coregen 2.1+
		    ostrstream pcore_filename;
		    pcore_filename << dest_dir 
			           << tolowercase(core_entity.str()) << ".xco" << ends;
		    ofstream out_core(pcore_filename.str());

/*
		    // Needed for Coregen 2.1+
		    ostrstream corelib_filename;
		    corelib_filename << dest_dir << "coregen.prj" << ends;
		    ofstream corelib_fstr(corelib_filename.str());
		    ostrstream core_edif;
		    ostrstream core_vho;
		    ostrstream core_asy;
		    core_edif << core_entity.str() << ".edn" << ends;
		    core_vho << core_entity.str() << ".vho" << ends;
		    core_asy << core_entity.str() << ".asy" << ends;

		    if (fpga_type()==XC4000)
		    {
			// Library info
			corelib_fstr << "Registered_Adder|xilinx|xc4000_all|1.0=active" << endl;

			// Core generator info
			out_core << "SET BusFormat = BusFormatParen" << endl;
			out_core << "SET SimulationOutputProducts = VHDL" << endl;
			out_core << "SET ViewlogicLibraryAlias = primary" << endl;
			out_core << "SET XilinxFamily = XC4000" << endl;
			out_core << "SET DesignFlow = VHDL" << endl;
			out_core << "SET FlowVendor = Synplicity" << endl;
			out_core << "SELECT Registered_Adder XC4000 Xilinx 1.0" << endl;
			out_core << "CSET component_name = " << tolowercase(core_entity.str()) << endl;
			out_core << "CSET input_width = " << adder_length << endl;
			out_core << "CSET sign = TRUE" << endl;
			out_core << "GENERATE" << endl;
		    }
		    else if (fpga_type()==VIRTEX)
		    {
			// Library info
			corelib_fstr << "Adder_Subtracter|xilinx|virtex_all|1.0=active" << endl;

			// Core generator info
			out_core << "SET BusFormat = BusFormatParen" << endl;
			out_core << "SET SimulationOutputProducts = VHDL" << endl;
			out_core << "SET ViewlogicLibraryAlias = primary" << endl;
			out_core << "SET XilinxFamily = Virtex" << endl;
			out_core << "SET DesignFlow = VHDL" << endl;
			out_core << "SET FlowVendor = Synplicity" << endl;
			out_core << "SELECT Adder_Subtracter Virtex Xilinx,_Inc. 1.0" << endl;
			out_core << "CSET synchronous_settings = clear" << endl;
			out_core << "CSET port_a_sign = signed" << endl;
			out_core << "CSET port_a_width = " << adder_length << endl;
			out_core << "CSET port_b_sign = signed" << endl;
			out_core << "CSET port_b_width = " << adder_length << endl;
			out_core << "CSET component_name =" << tolowercase(core_entity.str()) << endl;
			out_core << "CSET carry_borrow_input = TRUE" << endl;
			out_core << "CSET carry_borrow_output = FALSE" << endl;
			out_core << "CSET output_options = registered" << endl;
			out_core << "CSET ce_overrides = sync_controls_override_ce" << endl;
			out_core << "CSET ce_override_for_bypass = TRUE" << endl;
			out_core << "CSET sync_init_value = 0" << endl;
			out_core << "CSET async_init_value = 0" << endl;
			out_core << "CSET operation = add" << endl;
			out_core << "CSET bypass_sense = active_high" << endl;
			out_core << "CSET set_clear_priority = clear_overrides_set" << endl;
			out_core << "CSET output_width = " << pins->query_bitlen(2) << endl;
			out_core << "CSET clock_enable = TRUE" << endl;
			out_core << "CSET bypass = FALSE" << endl;
			out_core << "CSET asynchronous_settings = none" << endl;
			out_core << "CSET overflow_output = FALSE" << endl;
			out_core << "CSET create_rpm = TRUE" << endl;
			out_core << "CSET port_b_constant = FALSE" << endl;
			out_core << "GENERATE" << endl;
		    }

		    corelib_fstr.close();
		    out_core.close();

//		    if (access(core_edif.str(),0)!=-1)
                    if (fork()==0)
		    {
			chdir(dest_dir);
			
			// Coregen cannot overwrite files from previous runs, delete them here
			if (execlp("rm",
				   "rm",
				   core_edif.str(),
				   (char*) 0)==-1)
			    printf("ACSAddCGFPGA:Error, unable to remove old edif file\n");
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
			    printf("ACSAddCGFPGA:Error, unable to remove old vho file\n");
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
			    printf("ACSAddCGFPGA:Error, unable to remove old asy file\n");
		    }
		    else
			wait ((int*) 0);
		    
		    if (fork()==0)
		    {
			chdir(dest_dir);
			
			if (execlp("coregen",
				   "coregen",
				   "-b",
				   pcore_filename.str(),
				   "-p",
				   ".",
				   (char*) 0)==-1)
			    printf("ACSAddCGFPGA:Error, unable to spawn a coregen session\n");
		    }
	            else
			wait ((int*) 0);
*/		    
    // Old code for Coregen 1.5, XC4000 only
	            out_core << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_core << "SET XilinxFamily = All_XC4000_Families" << endl;
		    out_core << "SET BusFormat = BusFormatAngleBracket" << endl;
		    out_core << "SET TargetSymbolLibrary = primary" << endl;
		    out_core << "SET OVERWRITEFILES=true" << endl;
		    out_core << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Math/Adders and Subtracters/Registered Adder" << endl;
		    out_core << "GSET Signed = true" << endl;
		    
		    out_core << "GSET Input_Width = " 
			     << adder_length << endl;
		    out_core << "GSET Component_Name = " 
			     << tolowercase(core_entity.str()) << endl;
		    out_core << "GENERATE" << endl;
		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   pcore_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSAddCGFPGA:Error, unable to spawn a coregen session\n");
		    }
	            else
			wait ((int*) 0);
		}
                // END-USER CODE
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

