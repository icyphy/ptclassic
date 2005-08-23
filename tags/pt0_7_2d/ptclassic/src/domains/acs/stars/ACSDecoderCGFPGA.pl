defcore {
	name { Decoder }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Decoder }
	desc {
	    Generate a n to m decoder
	}
	version {@(#)ACSDecoderCGFPGA.pl	1.0 06/16/00}
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
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;
	    ACSIntArray** decoder_info;
	    int input_pins;
	    int output_pins;
	    
	    static const int DEBUG_DECODER=0;
	}
	constructor {
	    decoder_info=NULL;
	}
	method {
	    name {sg_get_privaledge}
	    access {public}
	    arglist { "(const int id)" }
	    type {"ACSIntArray*"}
	    code {
		if (id < output_pins)
		    return(decoder_info[id]);
		else
		    return(NULL);
	    }
	}
	method {
	    name {sg_set_privaledge}
	    access {public}
	    arglist { "(const int id, const int index, const int value)" }
	    type {int}
	    code {
		if (id < output_pins)
		    decoder_info[id]->set(index,value);
		else
		    return(0);
		return(1);
	    }
	}
	method {
	    name {sg_add_privaledge}
	    access {public}
	    arglist { "(const int id, const int value)" }
	    type {int}
	    code {
		// Remap global to local variables for clarity
		input_pins=input_count;
		output_pins=total_count-input_pins-1;
		
		if (DEBUG_DECODER)
		    printf("sg_add_privaledge, input_pins=%d, output_pins=%d\n",
			   input_pins,output_pins);
		
		if (decoder_info==NULL)
		{
		    decoder_info=new ACSIntArray*[output_pins+2];
		    for (int loop=0;loop<output_pins+2;loop++)
			decoder_info[loop]=new ACSIntArray;
		}
		if (id >= output_pins+2)
		    return(0);
		decoder_info[id]->add(value);
		
		if (DEBUG_DECODER)
		    decoder_info[id]->print("sg_add_privaledge");
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
		
		if (DEBUG_DECODER)
		{
		    decoder_info[0]->print("decoder_info[0] sg_setup");
		    pins->print_pins("sg_setup");
		}

		char* pin_name=new char[80];
		for (int loop=0;loop<input_pins;loop++)
		{
		    sprintf(pin_name,"in%d",loop);
		    int the_pin=pins->add_pin(pin_name,INPUT_PIN);
		    
		    int priority=decoder_info[0]->query(loop);
		    pins->set_pinpriority(the_pin,priority);
		}
		for (int loop=0;loop<output_pins;loop++)
		{
		    sprintf(pin_name,"out%d",loop);
		    int the_pin=pins->add_pin(pin_name,OUTPUT_PIN);

		    int priority=decoder_info[0]->query(input_pins+loop);
		    pins->set_pinpriority(the_pin,priority);
		}
		delete []pin_name;

		
		// Control port definitions

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
		    if (DEBUG_DECODER)
			pins->print_pins("sg_setup language phase");
		    output_filename << ".vhd" << ends;

		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions
		    for (int loop=0;loop<input_pins;loop++)
		    {
			pins->set_datatype(loop,STD_LOGIC);
			pins->set_precision(loop,0,1,LOCKED);
		    }
		    
		    // Output port definitions
		    for (int loop=0;loop<output_pins;loop++)
		    {
			pins->set_datatype(loop+input_pins,STD_LOGIC);
			pins->set_precision(loop+input_pins,0,1,LOCKED);
		    }

		    // Bidir port definitions

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

		    for (int oloop=0;oloop<output_pins;oloop++)
		    {
			int pin_counter=0;
			int extent=decoder_info[oloop+2]->population();

			out_fstr << pins->query_pinname(input_pins+oloop) << lang->equals();
			if (decoder_info[1]->query(oloop)==AL)
			    out_fstr << lang->not() << "(";

			if (DEBUG_DECODER)
			    printf("extent=%d\n",extent);
			if (extent==0)
			{
			    // No activation state clause
			    if (decoder_info[1]->query(oloop)==AL)
				out_fstr << "'0'";
			    else
				out_fstr << "'1'";
			}
			else
			    for (int iloop=0;iloop<extent;iloop++)
			    {
				if (pin_counter==0)
				    out_fstr << "(";
				
				int logic_setting=decoder_info[oloop+2]->query(iloop);
				if (DEBUG_DECODER)
				    printf("iloop=%d, logic_setting=%d\n",iloop,logic_setting);
				switch (logic_setting)
				{
				    case 1 :
					out_fstr << pins->query_pinname(pin_counter++);
				    break;
				    case 0 :
					out_fstr << lang->not(pins->query_pinname(pin_counter++));
				    break;
				    case -1 :
					pin_counter++;
				    out_fstr << "'1'";
				    break;
				}
				if (DEBUG_DECODER)
				    printf("pin_counter=%d, input_pins=%d\n",pin_counter,input_pins);
				if (pin_counter==input_pins)
				{
				    out_fstr << ")";
				    
				    if (iloop!=extent-1)
					out_fstr << lang->or();
				    pin_counter=0;
				}
				else
				    out_fstr << lang->and();
			    }
			if (decoder_info[1]->query(oloop)==AL)
			    out_fstr << ")";

			out_fstr << lang->end_statement << endl;
		    }

		    // END-USER CODE
		    out_fstr << lang->end_scope << lang->end_statement << endl;
		    out_fstr.close();
		    if (DEBUG_DECODER)
			printf("Core %s has been built\n",name());
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

