defcore {
	name { Abs }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Abs }
	desc {
Produces the cosine of the input, that is assumed to be in radians
	}
	version {@(#)ACSAbsCGFPGA.pl	1.0	4 August 1999}
	author { P. Fiore }
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
        ccinclude { <sys/ddi.h> }
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
	    name {Delay_Impact}
	    type {string}
	    desc {How does this delay affect scheduling? (Algorithmic or None)}
	    default {"None"}
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
		output_list->append((Pointer) "Output_Major_Bit");
		output_list->append((Pointer) "Output_Bit_Length");

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
	    name {sg_cost}
	    access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file)" }
	    type {int}
	    code {

		// BEGIN-USER CODE
		    cost_file << "cost=ceil(insizes/2)" << endl;
		numsim_file << "y=abs(x);" << endl;
		rangecalc_file << " if ( prod(inputrange>=0)  " << endl;
		rangecalc_file << "   orr=inputrange; " << endl;
		rangecalc_file << " elseif ( prod(inputrange<0) " << endl;
		rangecalc_file << "   orr=-fliplr(inputrange); " << endl;
		rangecalc_file << " else " << endl;
		rangecalc_file << "   orr=[0 max(abs(inputrange))]; " << endl;
		rangecalc_file << " end " << endl;
                natcon_file 
		    << "yesno=(insizes>=3 & insizes<=32 & outsizes<=insizes);"
		    << endl;
		// END-USER CODE

		// Return happy condition
		return(1);
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
		pins->add_pin("A","input",INPUT_PIN);
		pins->set_min_vlength(0,3);
		pins->set_max_vlength(0,32);

		// Output port definitions
		pins->add_pin("Q","output",OUTPUT_PIN);
		pins->set_min_vlength(1,3);
		pins->set_max_vlength(1,32);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_AH);

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
		    pins->set_datatype(0,STD_LOGIC);  // theta port
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC);  // dout port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // c pin
		    pins->set_datatype(3,STD_LOGIC);  // ctrl pin
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);
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

       constant_signals->add_pin("GND",0,1,STD_LOGIC);

                    Pin* new_pins=NULL;
                    ostrstream core_entity;

		    output_filename << ends;


                    // Generate new port definition
                    new_pins=new Pin;
                    *new_pins=*pins;  // Copy existing parameters

                    VHDL_LANG* lang=new VHDL_LANG;
                    ostrstream statements;
                    ostrstream patch_filename;

                    core_entity << "ACSpatch" << output_filename.str() << ends;
                    patch_filename << dest_dir << output_filename.str() 
                                   << ".vhd" << ends;
                    ofstream patch_fstr(patch_filename.str());

                    // Coregen tool requires matched inputs, add padding-logic
                    patch_fstr << "library IEEE;" << endl;
                    patch_fstr << "use IEEE.std_logic_1164.all;" << endl << endl
;
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

                    patch_fstr << lang->signal("tween_A",STD_LOGIC,pins->query_bitlen(0));
                    patch_fstr << lang->signal("tween_Q",STD_LOGIC,pins->query_bitlen(1));
                    patch_fstr << lang->signal("tween_c",STD_LOGIC,pins->query_bitlen(2));
                    patch_fstr << lang->signal("tween_ce",STD_LOGIC,pins->query_bitlen(3));
                    patch_fstr << lang->signal("tween_inv",STD_LOGIC,1);


                  // Correct Input A


                    statements << lang->equals( "tween_A",pins->retrieve_pinname(0))  << lang->end_statement << endl;
                    statements << lang->equals( "tween_inv",lang->slice(pins->retrieve_pinname(0),pins->query_bitlen(0)-1,
                               pins->query_bitlen(0)-1))   << lang->end_statement << endl;


                    statements << lang->equals( "tween_c",pins->retrieve_pinname(2))  << lang->end_statement << endl;
                    statements << lang->equals( "tween_ce",pins->retrieve_pinname(3))  << lang->end_statement << endl;
                    statements << lang->equals( pins->retrieve_pinname(1),"tween_Q")  << lang->end_statement << endl;






                    statements << "U_" << output_filename.str() << ":" 
                               << core_entity.str() << " port map(" << endl;

                    statements << "A=>tween_A," << endl;
                    statements << "inv=>tween_inv," << endl;
                    statements << "ce=>tween_ce," << endl;
                    statements << "c=>tween_c," << endl;

                    statements << ends;
                    patch_fstr << lang->begin_scope << endl;
                    patch_fstr << statements.str();
                    patch_fstr << lang->end_architecture(STRUCTURAL) << endl;
                    delete lang;


                 // Build instructions
                    ostrstream child_filename;
                    child_filename << core_entity.str() << ".vhd" << ends;
                    child_filenames->add(tolowercase(child_filename.str()));

                    ostrstream pcore_filename;
                    pcore_filename << dest_dir 
                                   << tolowercase(core_entity.str()) << ends;
                    ofstream out_fstr(pcore_filename.str());

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
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Math/Complementers/1's or 2's Complementer" 
			     << endl;
		    out_fstr << "GSET Twos_Complement = true" << endl;
		    out_fstr << "GSET Create_RPM = true" << endl;
		    out_fstr << "GSET Port_Width = " 
			     << pins->query_bitlen(0) << endl;
		    out_fstr << "GSET Component_Name = " 
			     << tolowercase(output_filename.str()) 
			     << endl;
		    out_fstr << "GENERATE" << endl;
		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   pcore_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSAbsCGFPGA:Error, unable to spawn a coregen session\n");		
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

