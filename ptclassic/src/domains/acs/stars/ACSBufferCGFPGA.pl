defcore {
	name { Buffer }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Buffer }
	desc {
	    Buffer a line
	}
	version {$Id$}
	author { K. Smith}
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
	}
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
	    name {sg_resources}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate CLB sizes
		resources->set_occupancy(0,0);

		// Calculate BW
		if (pins->query_preclock(1)==UNLOCKED)
		    pins->set_precision(1,
					pins->query_majorbit(0),
					pins->query_bitlen(0),
					UNLOCKED);

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
		pins->add_pin("in_buf",INPUT_PIN);

		// Output port definitions
		pins->add_pin("out_buf",OUTPUT_PIN);

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
		    pins->set_datatype(1,STD_LOGIC);
		
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

		constant_signals->add_pin("GND",0,1,STD_LOGIC);

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
		int src_len=pins->query_bitlen(0);
		int snk_len=pins->query_bitlen(1);
		int src_mbit=pins->query_majorbit(0);
		int snk_mbit=pins->query_majorbit(1);
		
		// Trap for simple case
		if ((src_mbit==snk_mbit) &&
		    (src_len==snk_len))
		{
		    out_fstr << lang->equals(pins->retrieve_pinname(1),
					     pins->retrieve_pinname(0))
			     << lang->end_statement << endl;
		}
		else
		{
		    if (src_len < snk_len)
		    {
			out_fstr << lang->equals(lang->slice(pins->retrieve_pinname(1),
							     src_len-1,0),
						 pins->retrieve_pinname(0))
			         << lang->end_statement << endl;
			char* extend_name=new char[MAX_STR];
			if (sign_convention==UNSIGNED)
			    strcpy(extend_name,"GND");
			else
			{
			    ostrstream expression;
			    if (src_len==1)
				expression << pins->retrieve_pinname(0) << ends;
			    else
				expression << lang->slice(pins->retrieve_pinname(0),src_len-1) << ends;
			    strcpy(extend_name,expression.str());
			}
			for (int extend_loop=src_len;extend_loop<=snk_len-1;extend_loop++)
			    out_fstr << lang->equals(lang->slice(pins->retrieve_pinname(1),extend_loop),extend_name)
				     << lang->end_statement << endl;
							
			// Cleanup
			delete []extend_name;
		    }
		    else
		    {
			if (bitslice_strategy==PRESERVE_MSB)
			{
			    // Preserve MSB
			    out_fstr << lang->equals(pins->retrieve_pinname(1),
						     lang->slice(pins->retrieve_pinname(0),src_len-1,src_len-snk_len));
			}
			else
			{
			    // Preseve LSB
			    out_fstr << lang->equals(pins->retrieve_pinname(1),
						     lang->slice(pins->retrieve_pinname(0),snk_len-1,0));
			}
			out_fstr << lang->end_statement << endl;
		    }
		}
/*
		{
		    // Do they overlap?
		    int test_result=bits_overlap(src_mbit,src_len,
						 snk_mbit,snk_len);

		    if (test_result & OVERLAP)
		    {
			if (test_result & SRC_LARGER)
			{
			    // FIX:Ok for now, but what about dangles?
			    out_fstr << lang->equals(pins->retrieve_pinname(1),
						     lang->slice(pins->retrieve_pinname(0),
								 snk_mbit,
								 snk_mbit-snk_len+1))
				     << lang->end_statement << endl;
			}
			else
			{
			    out_fstr << lang->equals(lang->slice(pins->retrieve_pinname(1),
								src_mbit,
								src_mbit-src_len+1),
						     pins->retrieve_pinname(0))
				     << lang->end_statement << endl;

			    // Trailers?
			    if ((src_mbit-src_len) > (snk_mbit-snk_len))
				out_fstr << lang->tie_it(pins->retrieve_pinname(1),
							 src_mbit-src_len,
							 snk_mbit-snk_len+1,
							 "GND");

			    if (sign_convention==UNSIGNED)
				out_fstr << lang->tie_it(pins->retrieve_pinname(1),
							 snk_mbit,
							 src_mbit+1,
							 "GND");
			    else
			    {
				ostrstream expression;
				if (src_len==1)
				    expression << pins->retrieve_pinname(0) << ends;
				else
				    expression << lang->slice(pins->retrieve_pinname(0),src_len-1)
					<< ends;
				out_fstr << lang->tie_it(pins->retrieve_pinname(1),
							 snk_mbit,
							 src_mbit+1,
							 expression.str());
			    }
			}
		    }
		    else
		    {
			printf("ACSBufferCGFPGA::Warning, no algorithm in ");
			printf("place to handle no overlap\n");
			printf("test_result=%d\n",test_result);
			printf("src=(%d,%d), snk=(%d,%d)\n",
			       src_mbit,src_len,
			       snk_mbit,snk_len);
			if (test_result & SRC_LARGER)
			{
			}
			else
			{
			}
		    }
		}
*/
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

