defcore {
	name { AbsPhaser }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { AbsPhaser }
	desc {
	    Every phase is relevant for HW purposes
	}
	version {@(#)ACSAbsPhaserCGFPGA.pl	1.0 12/29/00}
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

	    ACSIntArray* phase_info;
	    int divide_rate;
	    int repeat;
	    int numstate_pins;
	}
	constructor {
	    phase_info=NULL;
	}
	method {
	    name {sg_get_privaledge}
	    access {public}
	    type {"ACSIntArray*"}
	    code {
		return(phase_info);
	    }
	}
	method {
	    name {sg_set_privaledge}
	    access {public}
	    arglist { "(const int index, const int value)" }
	    type {int}
	    code {
		phase_info->set(index,value);
		return(1);
	    }
	}
	method {
	    name {sg_add_privaledge}
	    access {public}
	    arglist { "(const int value)" }
	    type {int}
	    code {
		if (phase_info==NULL)
		    phase_info=new ACSIntArray;
		phase_info->add(value);

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

		if (phase_info->population()!=2)
		    return(0);

		divide_rate=phase_info->query(0);
		repeat=phase_info->query(1);
		numstate_pins=(int) ceil(log((double) divide_rate)/log(2.0));
		
		char* phase_name=new char[MAX_STR];
		
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
		pins->add_pin("step",INPUT_PIN_EXTCTRL);
		pins->add_pin("clk",INPUT_PIN_CLK);
		pins->add_pin("reset",INPUT_PIN_RESET);
		for (int loop=0;loop<numstate_pins;loop++)
		{
		    sprintf(phase_name,"switch%d",loop);
		    int ctrl_pin=pins->add_pin(phase_name,OUTPUT_PIN_CTRL);
		    pins->set_pinpriority(ctrl_pin,loop);
		}

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		delete []phase_name;

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
		    for (int loop=0;loop<3+numstate_pins;loop++)
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
	    name {bselcode}
	    access {public}
	    arglist { "(const int value, const int bit_pos)" }
	    type {int}
	    code {
		int bit_power=(int) pow(2.0,bit_pos);

		// Check for trivial answer
		if (value < bit_power)
		    return(0);
		if (value == bit_power)
		    return(1);

		// Constrain shaving loop
		// log2(x)=log(x)/log(2)
		double precise_bit=log(value)/log(2.0);
		int max_bit=(int) ceil(precise_bit);

		int local_val=value;
		int curr_bit=max_bit;
		for (int bit_loop=max_bit;bit_loop>bit_pos;bit_loop--)
		{
		    curr_bit=bit_loop;
		    int power=(int) pow(2.0,bit_loop);
		    if ((local_val-power) >= 0)
			local_val-=power;
		    if (local_val<=0)
			break;
		}
		
		// Now examine bit position in question
		if (curr_bit==bit_pos+1)
		{
		    curr_bit--;
		    int power=(int) pow(2.0,curr_bit);
		    if (((local_val-power)>=0) && (curr_bit==bit_pos))
			return(1);
		    else
			return(0);
		}
		return(0);
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
		char* currstate_name=new char[MAX_STR];
		char* outpin_name=new char[MAX_STR];
		phase_states->add("Init_State");
		for (int loop=0;loop<divide_rate;loop++)
		{
		    sprintf(state_name,"Phase%d",loop);
		    phase_states->add(state_name);
		}
		if (!repeat)
		    phase_states->add("End_State");

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
		sync_sensies->add("reset");
		out_fstr << "-- Synchronous prcess" << endl;
		out_fstr << lang->process("Sync",sync_sensies) << endl;
		out_fstr << lang->begin_scope << endl;
		out_fstr << lang->if_statement << lang->test("reset","'1'")
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
		phase_sensies->add("step");


		ostrstream phase_state;
		phase_state << lang->if_statement << lang->test("step","'1'") << lang->then_statement << endl
		            << lang->equals("Next_Phase_State","Phase0") << lang->end_statement << endl 
		            << lang->else_statement << endl
			    << lang->equals("Next_Phase_State","Init_State") << lang->end_statement << endl
			    << lang->endif_statement << lang->end_statement << endl
		            << ends;
		phase_cases->add(phase_state.str());

		for (int loop=1;loop<=divide_rate;loop++)
		{
		    ostrstream phase_state;

		    sprintf(currstate_name,"Phase%d",loop-1);

		    // For each phase, properly assign its unique identifier
		    for (int ctrl_loop=0;ctrl_loop<numstate_pins;ctrl_loop++)
		    {
			sprintf(outpin_name,"switch%d",ctrl_loop);
			if (bselcode(loop-1,ctrl_loop)==1)
			    phase_state << lang->equals(outpin_name,"'1'");
			else
			    phase_state << lang->equals(outpin_name,"'0'");
			phase_state << lang->end_statement << endl;
		    }
		    
		    if (loop==divide_rate)
		    {
			if (repeat)
			    strcpy(state_name,"Phase0");
			else
			    strcpy(state_name,"End_State");
		    }
		    else
			sprintf(state_name,"Phase%d",loop);
		    
		    phase_state << lang->if_statement << lang->test("step","'1'") << lang->then_statement << endl
			        << lang->equals("Next_Phase_State",state_name) << lang->end_statement << endl
				<< lang->else_statement << endl
				<< lang->equals("Next_Phase_State",currstate_name) << lang->end_statement << endl
				<< lang->endif_statement << lang->end_statement << endl << ends;
		    phase_cases->add(phase_state.str());
		}

		if (!repeat)
		{
		    ostrstream phase_state;

		    for (int ctrl_loop=0;ctrl_loop<numstate_pins;ctrl_loop++)
		    {
			sprintf(outpin_name,"switch%d",ctrl_loop);
			if (bselcode(divide_rate-1,ctrl_loop)==1)
			    phase_state << lang->equals(outpin_name,"'1'");
			else
			    phase_state << lang->equals(outpin_name,"'0'");
			phase_state << lang->end_statement << endl;
		    }
		    
		    phase_state << lang->equals("Next_Phase_State","End_State")
		                << lang->end_statement << ends;
		    phase_cases->add(phase_state.str());
		}

		out_fstr << "--" << endl
		         << "-- Phase Process" << endl
			 << "--" << endl;
		out_fstr << lang->process("Phase",phase_sensies) << endl;
		out_fstr << lang->begin_scope << endl;
		for (int ctrl_loop=0;ctrl_loop<numstate_pins;ctrl_loop++)
		{
		    sprintf(outpin_name,"switch%d",ctrl_loop);
		    out_fstr << lang->equals(outpin_name,"'0'")
		             << lang->end_statement << endl;
		}
		out_fstr << lang->l_case("Phase_State",phase_states,phase_cases) << endl;
		out_fstr << lang->end_function_scope("Phase") << endl;

		// Cleanup
		delete phase_states;
		delete []state_name;
		delete []currstate_name;
		delete []outpin_name;
		delete sync_sensies;
		delete phase_sensies;
		delete phase_cases;

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
	    delete phase_info;
	}

	codeblock (trueblock) {
	}
	codeblock (falseblock) {
	}
}

