defcore {
	name { BitXNOR }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { BitXNOR }
	desc {
	    Provide a logical bitwise XNOR function
	}
	version {@(#)ACSBitXORCGFPGA.pl	1.0 05/17/99}
	author { K. Smith}
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
		   
	ccinclude { "acs_starconsts.h" }
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;
	}
       method {
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW
		if (pins->query_preclock(2)==UNLOCKED)
		    pins->set_precision(2,
					0,
					1,
					UNLOCKED);

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
		acs_existence=HW;

		// Input port definitions
		pins->add_pin("a",INPUT_PIN);
		pins->add_pin("b",INPUT_PIN);

		// Output port definitions
		pins->add_pin("x",OUTPUT_PIN);

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
		    pins->set_datatype(1,STD_LOGIC);
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);
		
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

		int src_len=pins->query_bitlen(0);
		int snk_len=pins->query_bitlen(1);
		int max_len=(int) max(src_len,snk_len);

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
		out_fstr << lang->signal("bit_columns",STD_LOGIC,max_len);
		out_fstr << lang->begin_scope << endl;

		// BEGIN-USER CODE
		// Trap for simple case
		if (src_len==snk_len)
		{
		    out_fstr << lang->equals("bit_columns",
					     pins->query_pinname(1))
			     << lang->xnor(pins->query_pinname(0))
			     << lang->end_statement << endl;
		}
		else
		{
		    if (src_len < snk_len)
		    {
			if (bitslice_strategy==PRESERVE_MSB)
			    out_fstr << lang->equals("bit_columns",
						     lang->slice(pins->query_pinname(1),
								 snk_len,snk_len-src_len))
				     << lang->xnor(pins->query_pinname(0))
				     << lang->end_statement << endl;
			else
			    out_fstr << lang->equals("bit_columns",
						     lang->slice(pins->query_pinname(1),src_len))
				     << lang->xnor(pins->query_pinname(0))
				     << lang->end_statement << endl;
		    }
		    else
		    {
			if (bitslice_strategy==PRESERVE_MSB)
			    out_fstr << lang->equals("bit_columns",
						     lang->slice(pins->query_pinname(0),
								 src_len,src_len-snk_len))
				     << lang->xnor(pins->query_pinname(1))
				     << lang->end_statement << endl;
			else
			    out_fstr << lang->equals("bit_columns",
						     lang->slice(pins->query_pinname(0),snk_len))
				     << lang->xnor(pins->query_pinname(1))
				     << lang->end_statement << endl;
		    }
		}

		// FIX:MSB's sourcing this function have been getting trimmed by the tools
                //     For now, leaving them out of the final equation
		out_fstr << pins->query_pinname(2) << lang->equals();
		for (int bit_loop=0;bit_loop<max_len;bit_loop++)
		{
		    if (bit_loop<=max_len-2)
		    {
			out_fstr << lang->slice("bit_columns",bit_loop);
                        if (bit_loop!=max_len-2)
			  out_fstr << lang->and();
		    }
		}
		out_fstr << lang->end_statement << endl;

		// END-USER CODE
		
		out_fstr << lang->end_scope << lang->end_statement << endl;
		out_fstr.close();
		printf("Core %s has been built\n",name());

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

