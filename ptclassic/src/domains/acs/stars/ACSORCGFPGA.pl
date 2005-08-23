defcore {
	name { OR }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { OR }
	desc {
	    Provide a logical OR function
	}
	version {@(#)ACSORCGFPGA.pl	1.0 09/19/99}
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
/*
		int in_size1=pins->query_bitlen(0);
		int in_size2=pins->query_bitlen(1);
		int out_size=(int) max(in_size1,in_size2);
		
		pins->set_precision(2,0,out_size,lock_mode);
*/
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
		pins->add_pin("in1",INPUT_PIN);
		pins->add_pin("in2",INPUT_PIN);

		// Output port definitions
		pins->add_pin("or_out",OUTPUT_PIN);

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
		    pins->set_precision(0,0,1,LOCKED);
		    pins->set_precision(1,0,1,LOCKED);
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_precision(2,0,1,LOCKED);
		
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
		int in_size1=pins->query_bitlen(0);
		int in_size2=pins->query_bitlen(1);

		// Trap for trivial solution
		if (in_size1==in_size2)
		    out_fstr << lang->equals(pins->query_pinname(2),pins->query_pinname(0))
			     << lang->or(pins->query_pinname(1));
		else
		{
		    // FIX:Functionality needed here!!!
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

