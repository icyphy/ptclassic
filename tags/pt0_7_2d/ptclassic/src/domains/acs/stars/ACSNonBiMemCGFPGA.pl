defcore {
	name { NonBiMem }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { NonBiMem }
	desc {
	    Converts a bidirectional memory interface into individual
	    source/sink memory lines
	}
	version{ @(#)ACSNonBiMemCGFPGA.pl	1.10 08/02/01 }
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
	ccinclude { "acs_vhdl_lang.h" }
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
		if ((pins->query_preclock(0)==UNLOCKED) && 
		    (pins->query_preclock(1)==UNLOCKED))
		{
		    int max_mbit=pins->query_majorbit(0);
		    int max_bits=pins->query_bitlen(0);
		    if (pins->query_bitlen(1) > max_bits)
		    {
			max_mbit=pins->query_majorbit(1);
			max_bits=pins->query_bitlen(1);
		    }
		    pins->set_precision(2,max_mbit,max_bits,UNLOCKED);
		}

		// If output is locked then fix all the input sizes accordingly
		if ((pins->query_preclock(0)==LOCKED) || 
		    (pins->query_preclock(1)==LOCKED))
		{
		    int max_mbit=0;
		    int max_bit=0;
		    if (pins->query_preclock(0)==LOCKED)
		    {
			max_mbit=pins->query_majorbit(0);
			max_bit=pins->query_bitlen(0);
		    }
		    if (pins->query_preclock(1)==LOCKED)
		    {
			max_mbit=pins->query_majorbit(1);
			max_bit=pins->query_bitlen(1);
		    }
		    pins->set_precision(0,max_mbit,max_bit,LOCKED);
		    pins->set_precision(1,max_mbit,max_bit,LOCKED);
		    pins->set_precision(2,max_mbit,max_bit,lock_mode);
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
		acs_existence=HARD;

		// Input port definitions
		pins->add_pin("in_mem",INPUT_PIN);

		// Output port definitions
		pins->add_pin("out_mem",OUTPUT_PIN);

		// Bidir port definitions
		pins->add_pin("bidir_mem",BIDIR_PIN);
		
		// Control port definitions
		pins->add_pin("dir_control",INPUT_PIN_DMUX);

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
		    pins->set_datatype(2,STD_LOGIC);

   		    // Control port definitions
		    pins->set_datatype(3,STD_LOGIC);
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

		StringArray* switch_sensies=new StringArray;
		switch_sensies->add("dir_control");
		switch_sensies->add("in_mem");
		switch_sensies->add("bidir_mem");

		out_fstr << lang->begin_scope << endl;

		out_fstr << lang->process("Switch",switch_sensies) 
		    << endl;
		delete switch_sensies;

		out_fstr << lang->begin_scope << endl;

		// BEGIN-USER CODE
		out_fstr << lang->if_statement 
		         << lang->test(pins->query_pinname(3),"'1'")
			 << lang->then_statement << endl
			 << "\t" 
			 << lang->equals(pins->query_pinname(2),
					 pins->query_pinname(0))
			 << lang->end_statement << endl
			 << "\t" 
			 << lang->equals(pins->query_pinname(1),
					 "(others=>'Z')")
			 << lang->end_statement << endl
			 << lang->else_statement << endl
			 << "\t"
			 << lang->equals(pins->query_pinname(2),"(others=>'Z')")
			 << lang->end_statement << endl
			 << "\t" 
			 << lang->equals(pins->query_pinname(1),
					 pins->query_pinname(2))
			 << lang->end_statement << endl
			 << lang->endif_statement 
			 << lang->end_statement << endl;
		// END-USER CODE
		out_fstr << lang->end_function_scope("Switch") << endl
			 << lang->end_scope << lang->end_statement << endl;
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

