defcore {
	name { Const }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Const }
	desc {
	    Generates a single delay for multiple lines
	}
        version{ @(#)ACSConstCGFPGA.pl	1.10 08/02/01 }
        author { Ken Smith }
        copyright {
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
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
                name {word_count}
                type {int}
                default {1}
                desc { Number of valid output clock cycles. }
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
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
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
		int bitlen=0;
		int majorbits=0;
		if (intparam_query("Output_Bit_Length")==0)
		{
		    bitlen=pins->query_bitlen(0);
		    majorbits=bitlen-1;
		}
		else
		{
		    bitlen=intparam_query("Output_Bit_Length");
		    majorbits=intparam_query("Output_Major_Bit");
		}

                cost_file << "cost=zeros(1,size(insizes,2));" << endl;
                cost_file << " if sum(numforms)>0 " << endl;
                cost_file << "  disp('ERROR - use parallel numeric form only' )" << endl;
                cost_file << " end " << endl;

		// numsim_file << "y=" << sg_constants->query_str(0,majorbits,bitlen) << ";" << endl;
                numsim_file <<  "t=" << sg_constants->query_str(0,majorbits,bitlen) << ";" << endl;
		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=t; " << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;

		rangecalc_file << "orr=[" 
		               << sg_constants->query_str(0,majorbits,bitlen) 
		               << " " << sg_constants->query_str(0,majorbits,bitlen) 
			       << "];" << endl;
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

		// this is ok because const latency does not depend on wordlength
		schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << " racts1=[0 1 vl1-1 ];" << endl;
		schedule_file << " racts=cell(1,size(outsizes,2));" << endl;
		schedule_file << " racts(:)=deal({racts1});" << endl;
		schedule_file << " minlr=vl1*ones(1,size(outsizes,2)); " << endl;
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
		if (pins->query_preclock(0)==UNLOCKED)
		{
		    pins->set_precision(0,0,sg_constants->query_bitsize(0,intparam_query("Output_Bit_Length")),LOCKED);
		}
		if (DEBUG_STARS)
		    printf("ACSConstCGFPGA, computed output bandwith to be %d\n",
			   sg_constants->query_bitsize(0,intparam_query("Output_Bit_Length")));
		
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
                pins->set_wordcount(0,intparam_query("word_count"));
		
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
		if (pins->query_preclock(0)==UNLOCKED)
		{
		    if (intparam_query("Output_Bit_Length")==0)
		    {
			bitlen=pins->query_bitlen(0);
			majorbits=pins->query_majorbit(0);
		    }
		    else
		    {
			bitlen=intparam_query("Output_Bit_Length");
			majorbits=intparam_query("Output_Major_Bit");
		    }
		}
		else
		{
		    bitlen=pins->query_bitlen(0);
		    majorbits=pins->query_majorbit(0);
		}
		    
		char* const_cval=new char[MAX_STR];
		if (DEBUG_STARS)
		    printf("Consts star, evaluating constant to precision %d.%d\n",
			   majorbits,
			   bitlen);
		strcpy(const_cval,sg_constants->query_bitstr(0,majorbits,bitlen));
		out_fstr << lang->equals(pins->query_pinname(0),
					 lang->val(const_cval))
		         << lang->end_statement << endl;
		delete []const_cval;
		printf("Core %s has been built\n",name());

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

