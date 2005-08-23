defcore {
	name { UnPackBits }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { UnPackBits }
	desc {
	    Given one input stream, splice and produce two streams
	}
	version{ @(#)ACSUnPackBitsCGFPGA.pl	1.2 08/02/01 }
	author { K. Smith}
	copyright {
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
	}
	ccinclude { "acs_starconsts.h" }
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

	    int stream1_lsb;
	    int stream2_lsb;
	    int stream1_msb;
	    int stream2_msb;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
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
	        // NOTE: output majorbit becomes obscured
		int total_precision=0;
		for (int loop=0;loop<total_count;loop++)
		    total_precision+=pins->query_bitlen(loop+1);
		pins->set_precision(0,0,total_precision,LOCKED);

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
		// General defintions
		acs_type=BOTH;
		acs_existence=SOFT;

		// Input port definitions
		pins->add_pin("input","input",INPUT_PIN);

		// Output port definitions
		char* output_pname=new char[80];
		for (int loop=0;loop<total_count;loop++)
		{
		    sprintf(output_pname,"output_no%d",loop);
		    pins->add_pin(output_pname,output_pname,OUTPUT_PIN);
		}
		delete []output_pname;

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
		    pins->set_datatype(0,STD_LOGIC);
		    
		    // Output port definitions
		    for (int loop=0;loop<total_count;loop++)
			pins->set_datatype(loop+1,STD_LOGIC);
		
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
		if (DEBUG_BUILD)
		    printf("Building %s\n",output_filename.str());

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
		int stream_msb=0;
		int stream_lsb=0;
		for (int loop=0;loop<total_count;loop++)
		{
		    stream_msb=pins->query_bitlen(loop+1)-1+stream_lsb;
		    out_fstr << lang->equals(pins->query_pinname(loop+1),
					     lang->slice(pins->query_pinname(0),
							 stream_msb,stream_lsb))
			     << lang->end_statement << endl;
		    stream_lsb=stream_msb+1;
		}
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

