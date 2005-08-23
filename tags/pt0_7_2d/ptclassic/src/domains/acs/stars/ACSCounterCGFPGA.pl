defcore {
	name { Counter }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Counter }
	desc {
	    Generates a counter
	}
	version {@(#)ACSCounterCGFPGA.pl	1.0 05/17/00}
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
        ccinclude { <sys/wait.h> }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;
	    Connection_List* connections;

	    // Counter settings
	    ACSIntArray* count_info;
	    int duration;
	    int decode_count;

	    static const int DEBUG_COUNTER=0;
	}
	constructor {
	    connections=NULL;
	    count_info=NULL;
	}
	method {
	    name {sg_get_privaledge}
	    access {public}
	    type {"ACSIntArray*"}
	    code {
		return(count_info);
	    }
	}
	method {
	    name {sg_set_privaledge}
	    access {public}
	    arglist { "(const int index, const int value)" }
	    type {int}
	    code {
		count_info->set(index,value);
		return(1);
	    }
	}
	method {
	    name {sg_add_privaledge}
	    access {public}
	    arglist { "(const int value)" }
	    type {int}
	    code {
		if (count_info==NULL)
		    count_info=new ACSIntArray;
		count_info->add(value);

		return(1);
	    }
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
	    name {macro_build}
	    access {public}
	    arglist { "(int* free_id, int* free_netid)" }
	    type {"CoreList*"}
	    code {
		CoreList* white_stars=new CoreList;
		Sg_Constructs* construct=new Sg_Constructs(free_id,free_netid,dest_dir);
	        connections=new Connection_List;
		CoreList* consts=new CoreList;

		// BEGIN-USER CODE
		if (target_implementation==0)
		{
		    // Generate the initial count value as HW constant 
		    // Add an additional clock period to avoid premature execution of last trigger
		    duration++;
		    ACSCGFPGACore* start_const=construct->add_const(-duration,SIGNED,white_stars);
		    start_const->add_comment("Initial countdown",name());

		    // Now generate the increment value as HW constants
		    ACSCGFPGACore* count_const=construct->add_const(1,SIGNED,white_stars);
		    count_const->add_comment("Countup value",name());

		    // Adjust bitwidths on constants
		    consts->append(start_const);
		    consts->append(count_const);
		    construct->balance_bw(consts);

		    // Add MUX in order to route the constants to an adder
		    ACSCGFPGACore* wc_mux=construct->add_mux(2,2,SIGNED,white_stars);
		    wc_mux->add_comment("Count value MUX",name());
		    wc_mux->bitslice_strategy=PRESERVE_LSB;
		    
		    // Add the address generating adder and inform the memory port
		    ACSCGFPGACore* wc_add=construct->add_adder(SIGNED,white_stars);
		    wc_add->bitslice_strategy=PRESERVE_LSB;
		    wc_add->add_comment("Word counter adder",name());

		    // Add a latch to control the clear function on the adder
		    ACSCGFPGACore* wc_latch=construct->add_sg("ACS","Latch","CGFPGA",BOTH,
							      UNSIGNED,white_stars);
		    wc_latch->add_comment("Word counter clear function",name());

		    // Fix output width to match total count
		    Pin* const_pins=count_const->pins;
		    int outpin=const_pins->retrieve_type(OUTPUT_PIN);
		    int decode_bitlen=const_pins->query_bitlen(outpin);
		    Pin* add_pins=wc_add->pins;
		    int add_pin=add_pins->retrieve_type(OUTPUT_PIN);
		    add_pins->set_precision(add_pin,decode_bitlen,decode_bitlen+1,LOCKED);

		    // Hook up the internals
		    construct->connect_sg(count_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		    construct->connect_sg(start_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		    construct->connect_sg(wc_mux,OUTPUT_PIN_MUX_RESULT,wc_add);
		    construct->connect_sg(wc_add,wc_add);
		    construct->connect_sg(wc_latch,wc_add,INPUT_PIN_CLR);

		    // Add and connect decoders
		    // FIX:Last decoder should tap off of the MSB of the adder instead of a 
		    //     full decode for efficiency sake
		    for (int loop=0;loop<count_info->population();loop++)
		    {
//			if (loop+1!=loop<count_info->population())
                        if (1)
			{
			    // Add needed cores
			    int count_instance=count_info->query(loop);
			    ACSCGFPGACore* decode_const=construct->
				add_const(count_instance-duration,SIGNED,white_stars);
			    decode_const->add_comment("Decoder const",count_instance);
			    ACSCGFPGACore* decoder=construct->add_sg("BitXNOR",BOTH,SIGNED,white_stars);
			    decoder->add_comment("Decoder",count_instance);
			    
			    // Set the precision of the constant to match the precision of the
			    // adders output
			    decode_const->set_precision(decode_const->find_hardpin(OUTPUT_PIN),
							decode_bitlen,
							decode_bitlen+1,
							LOCKED);
			
			    // Annotate its timing
			    decoder->set_pinpriority(decoder->find_hardpin(OUTPUT_PIN),count_instance);

			    // Make connections
			    construct->connect_sg(decode_const,decoder);
			    construct->connect_sg(wc_add,decoder);

			    // Hook up the externals
			    connections->add(loop+5,decoder->acs_id,decoder->find_hardpin(OUTPUT_PIN),
					     -1,-1,-1);
			}
			else
			{
			    int count_instance=count_info->query(loop);

			    // Add needed cores
			    ACSCGFPGACore* inverter=construct->add_sg("Inverter",BOTH,SIGNED,white_stars);
			    inverter->add_comment("Final decoder");

			    // Tell the inverter to only utilize the MSB of the counter
			    inverter->set_precision(inverter->find_hardpin(INPUT_PIN),
						    0,
						    1,
						    LOCKED);
			    inverter->set_precision(inverter->find_hardpin(OUTPUT_PIN),
						    0,
						    1,
						    LOCKED);

			    // Annotate its timing
			    inverter->set_pinpriority(inverter->find_hardpin(OUTPUT_PIN),count_instance);

			    // Make connections
			    construct->connect_sg(wc_add,inverter);

			    // Hook up the externals
			    connections->add(loop+5,inverter->acs_id,inverter->find_hardpin(OUTPUT_PIN),
					     -1,-1,-1);
			}
		    }
			
		    
		    // Hook up the remaining externals
		    // NOTE:Clk should bind properly
		    // FIX:Once multiple clocks, binding should be made local here and then
		    //     left to the parent to resolve
		    // Go pin
		    connections->add(-1,-1,-1,
				     0,wc_mux->acs_id,wc_mux->find_hardpin(INPUT_PIN_CTRL));
		    
		    // Ce pin
		    connections->add(-1,-1,-1,
				     2,wc_add->acs_id,wc_add->find_hardpin(INPUT_PIN_CE));

		    // Reset pin
		    connections->add(-1,-1,-1,
				     3,wc_latch->acs_id,wc_latch->find_hardpin(INPUT_PIN_CE));


		    // Clr pin
		    connections->add(-1,-1,-1,
				     4,wc_latch->acs_id,wc_latch->find_hardpin(INPUT_PIN_CLR));

		}
		else // target_implementation==1
		{
		    //NOTE:Clr connection not utilized as this is a non-mathematical process

		    // Add trigger mechanism
		    // Generate the idle signal for the trigger
		    ACSCGFPGACore* idle_const=construct->add_const(0,UNSIGNED,white_stars);
		    idle_const->add_comment("Idle trigger",name());

		    // Now generate the trigger value
		    ACSCGFPGACore* trigger_const=construct->add_const(1,UNSIGNED,white_stars);
		    trigger_const->add_comment("Trigger value",name());

		    // Adjust bitwidths on constants
		    consts->append(idle_const);
		    consts->append(trigger_const);
		    construct->balance_bw(consts);

		    // Add MUX in order to route the constants to an adder
		    ACSCGFPGACore* trigger_mux=construct->add_mux(2,2,SIGNED,white_stars);
		    trigger_mux->add_comment("Trigger MUX",name());
		    trigger_mux->bitslice_strategy=PRESERVE_LSB;

		    // Connect constants to the MUX
		    construct->connect_sg(idle_const,trigger_mux,INPUT_PIN_MUX_SWITCHABLE);
		    construct->connect_sg(trigger_const,trigger_mux,INPUT_PIN_MUX_SWITCHABLE);

		    // Go pin
		    connections->add(-1,-1,-1,
				     0,trigger_mux->acs_id,
				     trigger_mux->find_hardpin(INPUT_PIN_CTRL));
		    
		    // Add timing mechanism
		    // Start time takes into account the initial delay and therefore should
		    //   be removed from the iterative process of delay chaining
		    int start_time=0;
		    ACSCGFPGACore* prev_core=NULL;
		    ACSCGFPGACore* delay_core=NULL;
		    ACSCGFPGACore* buff_core=NULL;
		    if (DEBUG_COUNTER)
			count_info->print("Counter sg count_info");

		    // Add an initial delay, this necessary in order for proper synchronization
		    // with target implementation zero for this smart generator!
		    delay_core=construct->add_delay(1,white_stars);

		    // Ce pin
		    connections->add(-1,-1,-1,
				     2,
				     delay_core->acs_id,
				     delay_core->find_hardpin(INPUT_PIN_CE));

		    // Reset pin
		    // FIX:Depending on implementation, the clr pin may not be available
		    connections->add(-1,-1,-1,
				     3,
				     delay_core->acs_id,
				     delay_core->find_hardpin(INPUT_PIN_CLR));

		    // Hook up initial delay to mux and prepare delay chain for the upcoming loop
		    construct->connect_sg(trigger_mux,OUTPUT_PIN_MUX_RESULT,
					  delay_core);
		    prev_core=delay_core;
		    
		    for (int loop=0;loop<count_info->population();loop++)
		    {
			// Add needed cores
			int count_instance=count_info->query(loop);
			int delta_time=count_instance-start_time;
			if (loop==count_info->population()-1)
			    delta_time--;
			if (delta_time!=0)
			{
			    // Add the delay
				if (DEBUG_COUNTER)
				    printf("Counter sg, adding delay of %d\n",delta_time);
			    delay_core=construct->add_delay(delta_time,white_stars);

			    // Add a buffer as a tapping mechanism
			    buff_core=construct->add_buffer(UNSIGNED,white_stars);

			    // Ce pin
			    connections->add(-1,-1,-1,
					     2,
					     delay_core->acs_id,
					     delay_core->find_hardpin(INPUT_PIN_CE));

			    // Reset pin
			    // FIX:Depending on implementation, the clr pin may not be available
			    connections->add(-1,-1,-1,
					     3,
					     delay_core->acs_id,
					     delay_core->find_hardpin(INPUT_PIN_CLR));
					     
			    // Hook up the externals
			    connections->add(loop+5,buff_core->acs_id,
					     buff_core->find_hardpin(OUTPUT_PIN),
					     -1,-1,-1);
			    // Connect to the previous core
			    construct->connect_sg(prev_core,delay_core);
			    
			    // Connect delay to the announcement buffer core
			    construct->connect_sg(delay_core,buff_core);

			}
			else
			{
			    // Add a buffer as a tapping mechanism
			    buff_core=construct->add_buffer(UNSIGNED,white_stars);
			 
			    // Hook up the externals
			    connections->add(loop+5,buff_core->acs_id,
					     buff_core->find_hardpin(OUTPUT_PIN),
					     -1,-1,-1);
			    construct->connect_sg(prev_core,buff_core);
			}

			// Issue decoder phase for connecting to phaser
			int phase_pin=buff_core->alter_pintype(OUTPUT_PIN,
							       OUTPUT_PIN_PHASE_START);
			buff_core->set_pinpriority(phase_pin,start_time);

			// Prep for next iteration
			start_time=count_instance;
			prev_core=delay_core;
		    }

		}

		// END-USER CODE
		delete consts;
		delete count_info;
		delete construct;

		// Return happy condition
		return(white_stars);
	    }
	}
	method {
	    name {macro_connections}
	    access {public}
	    type {"Connection_List*"}
	    code {
		return(connections);
	    }
	}

	// FIX:Coded for XC4000
	// Standard design will use a traditional feedback countdown adder
	// Design 1 will use a delay chain
	method {
	    name {sg_designs}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		duration=count_info->tail();
		decode_count=count_info->population();

		if (implementation_lock==UNLOCKED)
		{
		    implementation_lock=lock_mode;

		    float cost0=0;
		    float cost1=0;

		    //////////////////////
		    // Cost0 determination
		    //////////////////////
		    int adder_bits=(int) ceil(log((double) duration)/log(2.0));

		    // Size of adder and input mux
		    cost0+=adder_bits/2+((adder_bits*3)/8);

		    // Size of decode stages
		    int start_time=0;
		    for (int loop=1;loop<decode_count;loop++)
			cost0+=(adder_bits*2)/8;
		    
		    //////////////////////
		    // Cost1 determination
		    //////////////////////
		    start_time=0;
		    for (int loop=1;loop<decode_count;loop++)
		    {
			float cost_ram=0.0;
			float cost_unit=0.0;

			// Determine time between decodes
			int curr_time=count_info->query(loop);
			int delta_time=curr_time-start_time;
			start_time=curr_time;

			// Cost for ram_delay
			int large_blocks=(int) floor(delta_time/17);
			int small_blocks=delta_time % 17;
			cost_ram=4*large_blocks;
			if (small_blocks < 5)
			    cost_ram+=2;
			else
			    cost_ram+=4;
			cost_unit=delta_time/2;

			if (cost_ram > cost_unit)
			    cost1+=cost_unit;
			else
			    cost1+=cost_ram;
		    }

		    //////////////////////
		    // Final determination
		    //////////////////////
		    if (adder_bits < 4)
			target_implementation=1;
		    else
			target_implementation=0;

		    // A register based delay for a duration of one isnt compatible with multiple executions
		    // of the sequencer
		    if (duration==1)
			target_implementation=0;
/*
		    if (cost0 > cost1)
			target_implementation=1;
		    else
			target_implementation=0;
*/
		}   
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
		// NOTE:Should not be a scheduled item

		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		decode_count=count_info->population();
		output_filename << name();

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=BOTH;
		acs_existence=HARD;

	        // Input port definitions

		// Output port definitions
		
		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("go",INPUT_PIN_EXTCTRL);
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_CE,AH);
		pins->add_pin("reset",INPUT_PIN_RESET,AH);
		pins->add_pin("start",INPUT_PIN_START);
		for (int loop=0;loop<decode_count;loop++)
		{
		    int act_time=count_info->query(loop);
		    char* pin_name=make_name("phase",act_time);
		    pins->add_pin(pin_name,OUTPUT_PIN);
		    delete []pin_name;
		}

		// Capability assignments
		sg_capability->add_domain("HW");
		sg_capability->add_architecture("any");
		sg_capability->add_language(VHDL_BEHAVIORAL);
		// END-USER CODE

		//////////////////////////////////
		// Language-dependnent assignments
		//////////////////////////////////
		if (sg_language==VHDL_BEHAVIORAL)
		{
		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions
		    
		    // Output port definitions
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(0,STD_LOGIC);
		    pins->set_datatype(1,STD_LOGIC);
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_datatype(3,STD_LOGIC);
		    pins->set_datatype(4,STD_LOGIC);
		    pins->set_precision(0,1,1,LOCKED);
		    pins->set_precision(1,1,1,LOCKED);
		    pins->set_precision(2,1,1,LOCKED);
		    pins->set_precision(3,1,1,LOCKED);
		    pins->set_precision(4,1,1,LOCKED);

		    for (int loop=0;loop<decode_count;loop++)
		    {
			pins->set_datatype(loop+5,STD_LOGIC);
			pins->set_precision(loop+5,0,1,LOCKED);
			pins->set_pinpriority(loop+5,count_info->query(loop));
		    }
		    
		    // END-USER CODE
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

