defcore {
	name { AddressCounter }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { AddressCounter }
	desc {
	    Generates a counter where no decoding is needed, all values relevant
	    Multiple base counts can be utilized
	}
	version {@(#)ACSAddressCounterCGFPGA.pl	1.0 12/29/00}
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
	    // count_info contains, in order, count_duration|repeater(BOOLEAN)|count_start(0)|count_start(1)|....|count_start(X)
	    // If it is a repeater, then the order is count_duration|repeater(BOOLEAN)|index mbit|index bitlen|acs id of external conn|count_start(0)|yada yada
	    ACSIntArray* count_info;
	    int repeater;
	    int ext_acsid;
	    int index_mbit;
	    int index_bitlen;
	    int count_start;
	    int count_duration;

	    static const int DEBUG_ADDRESSCOUNTER=0;
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

		if (DEBUG_ADDRESSCOUNTER)
		    count_info->print("addresscounter count_info");

		// BEGIN-USER CODE
		// Generate the initial count value as HW constants
		int total_counts=0;
		if (repeater)
		    total_counts=count_info->population()-5;
		else
		    total_counts=count_info->population()-2;
		ACSCGFPGACore* base_mux=construct->add_mux(total_counts,total_counts,SIGNED,white_stars);
		base_mux->bitslice_strategy=PRESERVE_LSB;
		if (repeater)
		    base_mux->add_comment("Repeating base address MUX");
		else
		    base_mux->add_comment("Base address MUX");
		    
		// Connect the constants.  
		CoreList* consts=new CoreList;
		if (repeater)
		    for (int loop=5;loop<count_info->population();loop++)
		    {
			int base=count_info->query(loop);
			ACSCGFPGACore* start_const=construct->add_const(base,SIGNED,white_stars);
			start_const->add_comment("Count index",name(),loop-5);
			consts->append(start_const);

			// Connect the const to one adder input
			construct->connect_sg(start_const,base_mux,INPUT_PIN_MUX_SWITCHABLE);
		    }
		else
		    for (int loop=2;loop<count_info->population();loop++)
		    {
			int base=count_info->query(loop);
			ACSCGFPGACore* start_const=construct->add_const(base,SIGNED,white_stars);
			start_const->add_comment("Count index",name(),loop-2);
			consts->append(start_const);
			
			// Connect to the base MUX
			construct->connect_sg(start_const,base_mux,INPUT_PIN_MUX_SWITCHABLE);
		    }

		// Adjust bitwidths on constants
		construct->balance_bw(consts);

		// Add base address controller and connect to base mux controls
		ACSCGFPGACore* phaser=construct->add_phaser(total_counts,repeater,white_stars);

		// Connect the phaser to the MUX control pins
		Pin* mux_pins=base_mux->pins;
		Pin* phaser_pins=phaser->pins;
		int numstate_pins=(int) ceil(log((double) total_counts)/log(2.0));
		for (int loop=0;loop<numstate_pins;loop++)
		{
		    int muxin_pin=mux_pins->retrieve_type(INPUT_PIN_CTRL,loop);
		    int phaseout_pin=phaser_pins->retrieve_type(OUTPUT_PIN_CTRL,loop);
		    construct->connect_sg(phaser,phaseout_pin,UNASSIGNED,
					  base_mux,muxin_pin,UNASSIGNED,
					  CTRL_NODE);
		}

		// Now generate the increment value as HW constants
		ACSCGFPGACore* count_const=construct->add_const(1,SIGNED,white_stars);
		count_const->set_precision(0,1,2,LOCKED);
		count_const->add_comment("Countup stride value",name());
		count_const->bitslice_strategy=PRESERVE_LSB;

		// Add MUX in order to route the constants to an adder
		ACSCGFPGACore* wc_mux=construct->add_mux(2,2,SIGNED,white_stars);
		wc_mux->add_comment("Count value MUX",name());
		wc_mux->bitslice_strategy=PRESERVE_LSB;
		    
		// Add the address generating adder and inform the memory port
		ACSCGFPGACore* wc_add=construct->add_adder(SIGNED,white_stars);
		wc_add->bitslice_strategy=PRESERVE_LSB;
		wc_add->add_comment("Counter adder",name());

		// Add a latch to control the clear function on the adder
		ACSCGFPGACore* wc_latch=construct->add_sg("ACS","Latch","CGFPGA",BOTH,
							  UNSIGNED,white_stars);
		wc_latch->add_comment("Counter clear function",name());

		// Fix output width to match total count
		int decode_bitlen=(int) ceil(log((double) count_duration)/log(2.0));
		Pin* add_pins=wc_add->pins;
		int add_pin=add_pins->retrieve_type(OUTPUT_PIN);
		if (DEBUG_ADDRESSCOUNTER)
		    printf("AbsCounter:count_duration=%d, decode_bitlen=%d\n",
			   count_duration,
			   decode_bitlen);
		add_pins->set_precision(add_pin,decode_bitlen,decode_bitlen+1,LOCKED);
		pins->set_precision(0,decode_bitlen,decode_bitlen+1,LOCKED);

		// Hook up the internals
		construct->connect_sg(count_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		
		if (repeater)
		{
			ACSCGFPGACore* adder_core=construct->add_adder(SIGNED,white_stars);
			adder_core->bitslice_strategy=PRESERVE_LSB;
			adder_core->add_comment("LUT index adder",name());

			// Connect the base MUX to the adder
			construct->connect_sg(base_mux,OUTPUT_PIN_MUX_RESULT,adder_core);

			// Remap the remaining adder input for the resovler to find
			Pin* adder_pins=adder_core->pins;
			int input_pin=adder_pins->free_pintype(INPUT_PIN);
			adder_pins->set_pintype(input_pin,INPUT_PIN_LUTINDEX);
			adder_pins->set_pinpriority(input_pin,ext_acsid);

			// This will now map to a control node, best to update precisions now
			adder_pins->set_precision(input_pin,index_mbit,index_bitlen,LOCKED);

			// Connect the adder to the wc MUX
			construct->connect_sg(adder_core,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		}
		else
		    construct->connect_sg(base_mux,OUTPUT_PIN_MUX_RESULT,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
//		construct->connect_sg(start_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		construct->connect_sg(wc_mux,OUTPUT_PIN_MUX_RESULT,wc_add);
		construct->connect_sg(wc_add,wc_add);
		construct->connect_sg(wc_latch,wc_add,INPUT_PIN_CLR);
		
		// Hook up the externals
		// NOTE:Clk should bind properly
		// FIX:Once multiple clocks, binding should be made local here and then
		//     left to the parent to resolve
		// Count pin
		connections->add(0,wc_add->acs_id,wc_add->find_hardpin(OUTPUT_PIN),
				 -1,-1,-1);
		// Go pin
		connections->add(-1,-1,-1,
				 1,wc_mux->acs_id,wc_mux->find_hardpin(INPUT_PIN_CTRL));
		// Ce pin
		connections->add(-1,-1,-1,
				 3,wc_add->acs_id,wc_add->find_hardpin(INPUT_PIN_CE));
		// Reset pin
		connections->add(-1,-1,-1,
				 4,wc_latch->acs_id,wc_latch->find_hardpin(INPUT_PIN_CE));
		// Clr pin
		connections->add(-1,-1,-1,
				 5,wc_latch->acs_id,wc_latch->find_hardpin(INPUT_PIN_CLR));

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

	method {
	    name {sg_designs}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		count_duration=count_info->get(0);
                repeater=count_info->get(1);
		if (repeater)
		{
		    ext_acsid=count_info->get(2);
		    index_mbit=count_info->get(3);
		    index_bitlen=count_info->get(4);
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
		count_duration=count_info->get(0);
                repeater=count_info->get(1);

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
		pins->add_pin("count_out",OUTPUT_PIN);
		
		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("go",INPUT_PIN_EXTCTRL);
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_CE,AH);
		pins->add_pin("reset",INPUT_PIN_RESET,AH);
		pins->add_pin("clr",INPUT_PIN_CLR,AH);

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
		    pins->set_datatype(0,STD_LOGIC);
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(1,STD_LOGIC);
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_datatype(3,STD_LOGIC);
		    pins->set_datatype(4,STD_LOGIC);
		    pins->set_datatype(5,STD_LOGIC);
		    pins->set_precision(1,0,1,LOCKED);
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);
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

