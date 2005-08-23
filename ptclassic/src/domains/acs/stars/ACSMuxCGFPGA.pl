defcore {
	name { Mux }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Mux }
	desc {
	    Generates a 2,3, and 4-input MUX 
	}
	version {@(#)ACSMuxCGFPGA.pl 1.1 18 June 1999}
	author { K. Smith }
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
	    desc {The total nubmer of bits represented at the output}
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
	    int bidir_flag;
	}
	method {
	    name {macro_query}
	    access {public}
	    type {int}
	    code {
		// BEGIN-USER CODE
		return(WHITE_STAR);
		// END-USER CODE
	    }
	}
        method {
	    name {sg_resources}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW
		if (pins->query_preclock(0)==UNLOCKED)
		    pins->set_precision(0,
					pins->query_bitlen(1)-1,
					pins->query_bitlen(1),
					UNLOCKED);

		// If output is locked then fix all the input sizes accordingly
		if (pins->query_preclock(0)==LOCKED)
		    for (int loop=1;loop<=total_count;loop++)
			pins->set_precision(loop,
					    pins->query_bitlen(0)-1,
					    pins->query_bitlen(0),
					    lock_mode);
		
		// Test if all switchables are locked, if so set & lock output
		int lock_type=LOCKED;
		for (int loop=1;loop<=total_count;loop++)
		    if (pins->query_preclock(loop)==UNLOCKED)
			lock_type=UNLOCKED;

		// FIX: Should work with the input balancing routine
		if (lock_type==LOCKED)
		    pins->set_precision(0,
					pins->query_bitlen(1)-1,
					pins->query_bitlen(1),
					LOCKED);

		// Calculate CLB sizes
		resources->set_occupancy(1,pins->query_bitlen(0)/2);
		
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

		bidir_flag=0;
		if (total_count==input_count)
		    pins->add_pin("o","input",OUTPUT_PIN_MUX_RESULT);
		else if (input_count==0)
		    pins->add_pin("o","input",INPUT_PIN_MUX_RESULT);
		else
		{
		    pins->add_pin("o","input",BIDIR_PIN_MUX_RESULT);
		    bidir_flag=1;
		}

		// FIX: Corona names for the output pins described here
		char* switch_name=new char[80];
		int line=0;
		for (int loop=input_count+1;loop<=total_count;loop++)
		{
		    sprintf(switch_name,"d%d",line++);
		    pins->add_pin(switch_name,"output",OUTPUT_PIN_MUX_SWITCHABLE);
		}
		for (int loop=0;loop<input_count;loop++)
		{
		    sprintf(switch_name,"d%d",line++);
		    pins->add_pin(switch_name,"output",INPUT_PIN_MUX_SWITCHABLE);
		}
		delete []switch_name;

		
		// Control port definitions
		control_count=(int) ceil(log((double) total_count)/log(2.0));
		char* control_name=new char[80];
		for (int loop=0;loop<control_count;loop++)
		{
		    sprintf(control_name,"s%d",loop);
		    int muxpin=pins->add_pin(control_name,INPUT_PIN_CTRL);
		    pins->set_pinpriority(muxpin,loop);
		}
		delete []control_name;

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		/////////////////////////////////
		// Language-dependent assignments
		/////////////////////////////////
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
		    pins->set_datatype(0,STD_LOGIC);  // o port
		    for (int loop=1;loop<=total_count;loop++)
		    {
			if (DEBUG_STARS)
			    printf("Setting pin %d\n",loop);
			pins->set_datatype(loop,STD_LOGIC);
			pins->set_min_vlength(loop,2);
		    }

   		    // Control port definitions
		    for (int loop=total_count+1;
			 loop<total_count+control_count+1;
			 loop++)
		    {
			if (DEBUG_STARS)
			    printf("Setting pin %d\n",loop);
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
		// Trap for language and generate appropriate code
	        if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    int outsens_flag=0;
		    ofstream out_fstr(output_filename.str());

		    ctrl_signals->add_pin("sel",
					  control_count-1,
					  control_count,
					  STD_LOGIC,
					  INTERNAL_PIN);
		    
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

		    // Merge the select lines with this
		    StringArray* switch_sensies=new StringArray;
		    switch_sensies->add("sel");

		    ostrstream merge_expr;
		    char* sel_slice=new char[MAX_STR];
		    if (control_count > 1)
			for (int loop=0;loop<control_count;loop++)
			{
			    int ind=loop+total_count+1;
			    sprintf(sel_slice,"sel(%d)",loop);
			    merge_expr << lang->equals(sel_slice,
						       pins->retrieve_pinname(ind))
				<< lang->end_statement << endl;
			}
		    else
		    {
			int ind=total_count+1;
			merge_expr << lang->equals("sel",
						   pins->retrieve_pinname(ind))
			           << lang->end_statement << endl;
			switch_sensies->add(pins->retrieve_pinname(ind));
		    }
			
		    delete []sel_slice;
		    merge_expr << ends;
		    out_fstr << merge_expr.str() << endl;

		    int line=0;
		    Constants* sel_val=new Constants;
		    char* sel_char=new char[MAX_STR];
		    ostrstream select;
		    for (int loop=total_count;loop>0;loop--)
		    {
			sel_val->add(&line,CINT);
			if (control_count > 1)
			    sprintf(sel_char,"\"%s\"",sel_val->
				    query_bitstr(line++,
						 control_count-1,
						 control_count));
			else
			    sprintf(sel_char,"'%s'",sel_val->
				    query_bitstr(line++,
						 control_count-1,
						 control_count));

			if (loop <= input_count)
			{
			    if (loop==input_count)
				select << lang->if_statement;
			    select << lang->test("sel",sel_char)
				   << lang->then_statement << endl
				   << "\t";
			    select << lang->equals("o",pins->
						   retrieve_pinname(line))
				   << lang->end_statement << endl;
			    switch_sensies->add(pins->retrieve_pinname(line));
			    if ((bidir_flag) && (!outsens_flag))
			    {
				switch_sensies->add("o");
				outsens_flag=1;
			    }
			    if (loop != 1)
				select << lang->elseif_statement;
			    else
			    {
				select << lang->else_statement << endl
				       << "\t";
				if (bidir_flag)
				    select << lang->equals("o","(others=>'Z')")
					<< lang->end_statement << endl;
				else
				    select << lang->equals("o","(others=>'0')")
					<< lang->end_statement << endl;
				select << lang->endif_statement 
				       << lang->end_statement << endl;
			    }
			    
			}
			else
			{
			    if (!outsens_flag)
			    {
				switch_sensies->add("o");
				outsens_flag=1;
			    }
			    select << lang->if_statement;
			    select << lang->test("sel",sel_char)
				   << lang->then_statement << endl
				   << "\t";
			    select << lang->equals(pins->retrieve_pinname(line),"o")
				   << lang->end_statement << endl;
			    select << lang->else_statement << endl
				   << "\t";
			    if (bidir_flag)
				select << lang->equals(pins->retrieve_pinname(line),"(others=>'Z')")
				   << lang->end_statement << endl;
			    else
				select << lang->equals(pins->retrieve_pinname(line),"(others=>'0')")
				    << lang->end_statement << endl;
				
			    select << lang->endif_statement 
				   << lang->end_statement << endl;
			}
		    } 
		    select << ends;
		    delete sel_val;
		    delete []sel_char;
		    
		    out_fstr << lang->process("Switch",switch_sensies) 
			<< endl;
		    out_fstr << lang->begin_scope << endl;
		    out_fstr << select.str() << endl;
		    out_fstr << lang->end_function_scope("Switch") << endl;
		    delete switch_sensies;

		    // END-USER CODE
		    out_fstr << lang->end_scope << lang->end_statement << endl;
		    out_fstr.close();
		}
		else
		    return(0);

		// Return happy condition
		return(1);
	    }
	}
	go {
		addCode(trueblock);
	}
	codeblock (trueblock) {
// Line 1 from CGDelay if control==TRUE
// Line 2 from CGDelay if control==TRUE
	}
	codeblock (falseblock) {
// Line 1 from CGDelay if control==FALSE
// Line 2 from CGDelay if control==FALSE
	}
}

