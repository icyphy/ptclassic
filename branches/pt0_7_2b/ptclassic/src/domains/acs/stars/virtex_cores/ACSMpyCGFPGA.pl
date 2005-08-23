defcore {
	name { Mpy }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Mpy }
	desc {
Product of two inputs
Only the first two connections are used for hardware
	}
	version {@(#)ACSMpyCGFPGA.pl	1.4 09/10/99}
	author { K. Smith }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
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
		cost_file << "%Bad coregen, bad...bad" << endl
		    << "cost=ceil((max(insizes)-1).*min(insizes));" << endl;
                cost_file << " if sum(numforms)>0 " << endl;
                cost_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                cost_file << " end " << endl;
		
		// numsim_file << "y=prod(x);" << endl;
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{1,k}.*x{2,k}; " << endl;
                numsim_file <<  " end " << endl;
		
 	        rangecalc_file << "ir=reshape(inputrange,4,1);" << endl;
                rangecalc_file 
		    << "irr=[ir(1)*ir(2) ir(1)*ir(4) ir(3)*ir(2) ir(3)*ir(4)];"
			<< endl;
                rangecalc_file << "orr=[min(irr) max(irr)];" << endl;

                natcon_file << "wi=sum(msbranges(1:2)'*ones(1,size(insizes,2))-insizes+1);" << endl;
                natcon_file << "wo=msbranges(3)-outsizes+1;" << endl;
//                natcon_file << "yesno=(wo>=wi) & (min(insizes) >=6 & max(insizes) <=32 & outsizes >= 6 & outsizes <=32);" << endl;
 //               natcon_file << "yesno=(wo==wi) & (min(insizes) >=6 & max(insizes) <=32 & outsizes >= 6 & outsizes <=32);" << endl;
               natcon_file << "yesno=(wo>=wi) & (min(insizes) >=6 & max(insizes) <=32 );" << endl;
			
		schedule_file << "outdel= 4*ones(1,size(insizes,2)); " << endl;
		schedule_file << "t=find(insizes(2,:)>8); " << endl;
		schedule_file << "outdel(t)=outdel(t)+1; " << endl;
		schedule_file << "t=find(insizes(2,:)>16); " << endl;
		schedule_file << "outdel(t)=outdel(t)+1; " << endl;
                schedule_file << "vl1=veclengs(1); " << endl;
		schedule_file << "racts=cell(1,size(insizes,2));" << endl;
		schedule_file << "for k=1:size(insizes,2)" << endl;
                schedule_file << "  racts1=[0 1 vl1-1 ;0 1 vl1-1; outdel(k) 1 vl1-1+outdel(k)];" << endl;
		schedule_file << "  racts{k}=racts1;" << endl;
		schedule_file << "end"  << endl;
                schedule_file << "minlr=vl1*ones(1,size(insizes,2)); " << endl;
                schedule_file << "if sum(numforms)>0 " << endl;
                schedule_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                schedule_file << "end " << endl;

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
		int A_majorbit=pins->query_majorbit(0);
		int A_bitlen=pins->query_bitlen(0);
		int B_majorbit=pins->query_majorbit(1);
		int B_bitlen=pins->query_bitlen(1);
		    
		if (pins->query_preclock(2)==UNLOCKED)
		{
		    int S_majorbit=A_majorbit+B_majorbit;
		    int S_bitlen=A_bitlen+B_bitlen;
		    
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
		int B_bitlen=pins->query_bitlen(1);
		if ((B_bitlen >=6) && (B_bitlen <= 8))
		    acs_delay=4;
		else if ((B_bitlen >=9) && (B_bitlen <= 16))
		    acs_delay=5;
		else if (B_bitlen > 16)
		    acs_delay=6;
		else
		    acs_delay=-1;

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
		pins->add_pin("a","input#1",INPUT_PIN);
		pins->add_pin("b","input#2",INPUT_PIN);
		pins->set_min_vlength(0,2);
		pins->set_max_vlength(0,32);
		pins->set_min_vlength(1,2);
		pins->set_max_vlength(1,32);

		// Output port definitions
		pins->add_pin("prod","output",OUTPUT_PIN);

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
		    pins->set_datatype(0,STD_LOGIC);  // a port
		    pins->set_datatype(1,STD_LOGIC);  // b port
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);  // prod port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(3,STD_LOGIC);  // c pin
		    pins->set_datatype(4,STD_LOGIC);  // ce pin
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
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
		// Trap for language and generate appropriate code
		if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    // Calculate Core-specified BW
		    int A_bitlen=pins->query_bitlen(0);
		    int B_bitlen=pins->query_bitlen(1);
		    int S_bitlen=A_bitlen+B_bitlen;
		    int requested_s_bitlen=pins->query_bitlen(2);

		    // Build preamble
		    output_filename << ends;

		    ostrstream poutput_filename;
		    ostrstream core_filename;
		    
		    if (S_bitlen!=requested_s_bitlen)
		    {
			Pin* new_pins=NULL;
			ostrstream core_entity, patch_filename;
			new_pins=dup_pins();

			// Establish the correct output size of the coregen
			int S_mbit=pins->query_majorbit(2);
			new_pins->set_precision(2,S_mbit,S_bitlen,LOCKED);

			VHDL_LANG* lang=new VHDL_LANG;

			// Parent build instructions
			core_entity << output_filename.str() << ends;
			patch_filename << dest_dir << core_entity.str() << ".vhd" << ends;
			ofstream patch_fstr(patch_filename.str());
			
			// Child build instructions
			ostrstream child_entity;
			ostrstream child_filename;
			child_entity << "acspatch" << tolowercase(core_entity.str()) << ends;
			child_filename << tolowercase(child_entity.str()) << ".vhd" << ends;
			child_filenames->add(tolowercase(child_filename.str()));

			// Build instructions
			patch_fstr << "library IEEE;" << endl;
			patch_fstr << "use IEEE.std_logic_1164.all;" << endl << endl;
			patch_fstr << lang->gen_entity(output_filename.str(),pins) << endl;
			patch_fstr << lang->gen_architecture(name(),
							     NULL,
							     STRUCTURAL,
							     pins,
							     data_signals,
							     ctrl_signals,
							     constant_signals);
			patch_fstr << lang->start_component(child_entity.str());
			patch_fstr << lang->start_port() << endl;
			patch_fstr << lang->set_port(new_pins) << endl;
			patch_fstr << lang->end_port() << endl;
			patch_fstr << lang->end_component() << endl;

			patch_fstr << lang->signal("out_prod",STD_LOGIC,S_bitlen);

			patch_fstr << lang->begin_scope << endl;

			patch_fstr << "prod" << lang->equals();
			if (requested_s_bitlen > S_bitlen)
			    printf("ERROR::ACSMpyCGFPGA:Unexpected wordgui expectations on this core;)\n");
			else
			{
			    if (bitslice_strategy==PRESERVE_MSB)
				patch_fstr << lang->slice("out_prod",S_bitlen-1,S_bitlen-requested_s_bitlen);
			    else
				patch_fstr << lang->slice("out_prod",requested_s_bitlen-1,0);
			}
			patch_fstr << lang->end_statement << endl;

			patch_fstr << "U_" << child_entity.str() << ":" 
			           << child_entity.str() << " port map(" << endl;
			patch_fstr << "a=>a," << endl;
			patch_fstr << "b=>b," << endl;
			
			patch_fstr << "prod=>out_prod," << endl;
			patch_fstr << "c=>c," << endl
			           << "ce=>ce);" << endl;
			patch_fstr << lang->end_architecture(STRUCTURAL) << endl;
			patch_fstr.close();

			poutput_filename << dest_dir << tolowercase(child_filename.str()) << ends;
			core_filename << child_entity.str() << ends;

			// Cleanup
			delete lang;
		    }
		    else
		    {
			// Synthesis should ignore this
			black_box=1;

			poutput_filename << dest_dir << output_filename.str() << ends;
			core_filename << tolowercase(output_filename.str()) << ends;
		    }
			
		    ofstream out_fstr(poutput_filename.str());


		    out_fstr << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_fstr << "SET XilinxFamily = All_XC4000_Families" 
			     << endl;
		    out_fstr << "SET BusFormat = BusFormatAngleBracket" 
			     << endl;
		    out_fstr << "SET TargetSymbolLibrary = primary" << endl;
		    out_fstr << "SET OVERWRITEFILES=true" << endl;
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Math/Multipliers/Parallel Multiplier - Area Optimized" 
			     << endl;
		    out_fstr << "GSET Signed = true" << endl;
		    out_fstr << "GSET A_Width = " 
			     << pins->query_bitlen(0) << endl;
		    out_fstr << "GSET B_Width = " 
			     << pins->query_bitlen(1) << endl;
		    out_fstr << "GSET Component_Name = " 
			     << tolowercase(core_filename.str()) 
			     << endl;
		    out_fstr << "GENERATE" << endl;
		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   poutput_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSMpyCGFPGA:Error, unable to spawn a coregen session\n");		
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

