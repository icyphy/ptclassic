defcore {
	name { Phaser }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Phaser }
	desc {
	}
	version {@(#)ACSPhaserCGFPGA.pl	1.4 05/30/00}
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
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;

	    int divide_rate;
	}
	method {
	    name {sg_add_privaledge}
	    access {public}
	    arglist { "(const int value)" }
	    type {int}
	    code {
		divide_rate=value;

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

		// Output port definitions

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("go",INPUT_PIN_EXTCTRL);
		pins->add_pin("stop",INPUT_PIN_STOP);
		pins->add_pin("clk",INPUT_PIN_CLK);
		pins->add_pin("pre_clkout",OUTPUT_PIN_PREPHASE);
		pins->add_pin("clk_out",OUTPUT_PIN_PHASE);

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
		
		    // Bidir port definitions

   		    // Control port definitions
		    for (int loop=0;loop<5;loop++)
		    {
			pins->set_datatype(loop,STD_LOGIC);
			pins->set_precision(loop,0,1,LOCKED);
		    }
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

		// BEGIN-USER CODE
		// Declaritives
		StringArray* phase_states=new StringArray;
		char* state_name=new char[MAX_STR];
		phase_states->add("Init_State");
		for (int loop=0;loop<divide_rate;loop++)
		{
		    sprintf(state_name,"Phase%d",loop);
		    phase_states->add(state_name);
		}

		out_fstr << "-- Dividing state machine" << endl;
		out_fstr << lang->type_def("Phase_States",phase_states) << endl;

		out_fstr << "-- State control" << endl;
		out_fstr << lang->signal("Phase_State","Phase_States");
		out_fstr << lang->signal("Next_Phase_State","Phase_States") << endl;
		// END-USER CODE

		out_fstr << lang->begin_scope << endl;

		// BEGIN-USER CODE
		// Execution statement
		StringArray* sync_sensies=new StringArray;
		sync_sensies->add("clk");
		sync_sensies->add("stop");
		out_fstr << "-- Synchronous prcess" << endl;
		out_fstr << lang->process("Sync",sync_sensies) << endl;
		out_fstr << lang->begin_scope << endl;
		out_fstr << lang->if_statement << lang->test("stop","'1'")
		         << lang->then_statement << endl
			 << "\t" << lang->equals("Phase_State","Init_State")
			 << lang->end_statement << endl
			 << lang->elseif_statement << lang->test("clk","'1'")
			 << lang->and() << "(clk'event)"
			 << lang->then_statement << endl
			 << "\t" << lang->equals("Phase_State","Next_Phase_State")
			 << lang->end_statement << endl
			 << lang->endif_statement << lang->end_statement << endl;
		out_fstr << lang->end_function_scope("Sync") << endl;

		StringArray* phase_sensies=new StringArray;
		StringArray* phase_cases=new StringArray;
		phase_sensies->add("Phase_State");
		phase_sensies->add("go");

		ostrstream phase_state;
		phase_state << lang->if_statement << lang->test("go","'1'")
		            << lang->then_statement << endl
			    << "\t" << lang->equals("Next_Phase_State","Phase0")
			    << lang->end_statement << endl
			    << lang->else_statement << endl
			    << "\t" << lang->equals("Next_Phase_State","Init_State")
			    << lang->end_statement << endl
		            << lang->endif_statement << lang->end_statement << endl << ends;
		phase_cases->add(phase_state.str());

		for (int loop=1;loop<=divide_rate;loop++)
		{
		    ostrstream phase_state;
		    if (loop==divide_rate)
		    {
			phase_state << lang->equals("clk_out","'1'") 
			            << lang->end_statement << endl;
			strcpy(state_name,"Phase0");
		    }
		    else
			sprintf(state_name,"Phase%d",loop);
		    
		    if ((loop==divide_rate-1) || (divide_rate==1))
			phase_state << lang->equals("pre_clkout","'1'")
			            << lang->end_statement << endl;

		    phase_state << lang->equals("Next_Phase_State",state_name) 
			        << lang->end_statement << ends;
		    phase_cases->add(phase_state.str());
		}

		out_fstr << "--" << endl
		         << "-- Phase Process" << endl
			 << "--" << endl;
		out_fstr << lang->process("Phase",phase_sensies) << endl;
		out_fstr << lang->begin_scope << endl;
		out_fstr << lang->equals("clk_out","'0'") << lang->end_statement << endl;
		out_fstr << lang->equals("pre_clkout","'0'") << lang->end_statement << endl;
		out_fstr << lang->l_case("Phase_State",phase_states,phase_cases) << endl;
		out_fstr << lang->end_function_scope("Phase") << endl;

		// Cleanup
		delete phase_states;
		delete []state_name;
		delete sync_sensies;
		delete phase_sensies;
		delete phase_cases;

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
	}

	codeblock (trueblock) {
	}
	codeblock (falseblock) {
	}
}

