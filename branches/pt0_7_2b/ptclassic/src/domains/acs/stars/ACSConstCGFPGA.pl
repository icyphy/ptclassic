defcore {
	name { Const }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Const }
	desc {
	    Generates a single delay for multiple lines
	}
        version {1.0    18 June 1999}
        author { Eric K. Pauer }
        copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
	}
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
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
        constructor {
	    sg_constants=new Constants;
	}
	destructor {
	    delete sg_constants;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(SequentialList* input_list,SequentialList* output_list)" }
	    type {int}
	    code {
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
	    name {macro_build}
	    access {public}
	    arglist { "(int inodes,int* acs_ids)" }
	    type {SequentialList}
	    code {
		return(NULL);
	    }
	}
        method {
            name {sg_cost}
            access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file)" }
            type {int}
            code {
                // BEGIN-USER CODE
		int bitlen=0;
		int majorbits=0;
		if (Output_Bit_Length==0)
		{
		    bitlen=pins->query_bitlen(0);
		    majorbits=bitlen-1;
		}
		else
		{
		    bitlen=Output_Bit_Length;
		    majorbits=Output_Major_Bit;
		}

                cost_file << "cost=0;" << endl;
		numsim_file << "y=" << sg_constants->query_str(0,majorbits,bitlen) 
		            << ";" << endl;
		rangecalc_file << "orr=[" 
		               << sg_constants->query_str(0,majorbits,bitlen) 
		               << " " << sg_constants->query_str(0,majorbits,bitlen) 
			       << "];" << endl;
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;
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
		// Calculate CLB sizes
		resources->set_occupancy(0,0);

		// Calculate BW
		if (pins->query_preclock(0)==UNLOCKED)
		    pins->set_precision(0,0,sg_constants->query_bitsize(0),LOCKED);
		if (DEBUG_STARS)
		    printf("ACSConstCGFPGA, computed output bandwith to be %d\n",
			   sg_constants->query_bitsize(0));
		
		// Calculate pipe delay
		acs_delay=0;
		
		// Return happy condition
		return(1);
		}
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		output_filename << dest_dir << name();
		
		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// Constant defintions
		int majorbit=intparam_query("Output_Major_Bit");
		int bitlen=intparam_query("Output_Bit_Length");
		float constant=float(corona.level);

		if (((majorbit+1)-bitlen) < 0)
		    sg_constants->add(&constant,CFLOAT);
		else
		{
		    long long_val=(long) constant;
		    sg_constants->add(&long_val,CLONG);
		}
    
		    
		// General defintions
		acs_type=BOTH;
		acs_existence=SOFT;

	        // Input port definitions

		// Output port definitions
		pins->add_pin("const","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		//////////////////////////////////
		// Language-dependent assignments
		//////////////////////////////////
		if (sg_language==VHDL_BEHAVIORAL)
		{
		    output_filename << ".vhd" << ends;
		    
		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions
		    
		    // Output port definitions
		    pins->set_datatype(0,STD_LOGIC);  // q port
		
		    // Bidir port definitions

   		    // Control port definitions
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
		ofstream out_fstr(output_filename.str());

		out_fstr << lang->gen_libraries(libs,incs);
		out_fstr << lang->gen_entity(name(),pins);
		out_fstr << lang->gen_architecture(name(),
						   NULL,
						   BEHAVIORAL,
						   pins,
						   data_signals,
						   ctrl_signals,
						   constant_signals);
		out_fstr << lang->begin_scope << endl;

		// BEGIN-USER CODE
		// If Output_XXX is specified then this is a user-generated
		// constant star and could contain any value.  If not, then this
		// is a scheduler/stitcher generated constant and bitwidths are
		// implied.
		int bitlen=0;
		int majorbits=0;
		if (Output_Bit_Length==0)
		{
		    bitlen=pins->query_bitlen(0);
		    majorbits=bitlen-1;
		}
		else
		{
		    bitlen=Output_Bit_Length;
		    majorbits=Output_Major_Bit;
		}

		char* const_cval=new char[MAX_STR];
		strcpy(const_cval,sg_constants->query_bitstr(0,majorbits,bitlen));
		out_fstr << lang->equals(pins->retrieve_pinname(0),
					 lang->val(const_cval))
		         << lang->end_statement << endl;
		delete []const_cval;
		// END-USER CODE
		
		out_fstr << lang->end_scope << lang->end_statement << endl;
		out_fstr.close();

		// Return happy condition
		return(1);
	    }
	}

	go {
		addCode(trueblock);
	}
	destructor {
	    delete sg_constants;
	}

	codeblock (trueblock) {
	}
	codeblock (falseblock) {
	}
}

