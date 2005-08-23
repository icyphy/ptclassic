defcore {
	name { AbsCounter }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { AbsCounter }
	desc {
	    Generates a counter where no decoding is needed, all values relevant
	}
	version {@(#)ACSAbsCounterCGFPGA.pl	1.0 09/07/00}
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
	    // count_info contains, in order, count_start|count_duration|count_stride
	    ACSIntArray* count_info;
	    int count_start;
	    int count_duration;
	    int count_stride;
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

		// BEGIN-USER CODE
		// Generate the initial count value as HW constant 
		ACSCGFPGACore* start_const=construct->add_const(count_start,SIGNED,white_stars);
		start_const->add_comment("Initial count",name());

		// Now generate the increment value as HW constants
		ACSCGFPGACore* count_const=construct->add_const(count_stride,SIGNED,white_stars);
		count_const->add_comment("Countup stride value",name());

		// Adjust bitwidths on constants
		CoreList* consts=new CoreList;
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
		wc_add->add_comment("Counter adder",name());

		// Add a latch to control the clear function on the adder
		ACSCGFPGACore* wc_latch=construct->add_sg("ACS","Latch","CGFPGA",BOTH,
							  UNSIGNED,white_stars);
		wc_latch->add_comment("Counter clear function",name());

		// Fix output width to match total count
		int decode_bitlen=(int) ceil(log((double) count_duration)/log(2.0));
		Pin* add_pins=wc_add->pins;
		int add_pin=add_pins->retrieve_type(OUTPUT_PIN);
		printf("AbsCounter:count_duration=%d, decode_bitlen=%d\n",
		       count_duration,
		       decode_bitlen);
		add_pins->set_precision(add_pin,decode_bitlen,decode_bitlen+1,LOCKED);
		pins->set_precision(0,decode_bitlen,decode_bitlen+1,LOCKED);

		// Hook up the internals
		construct->connect_sg(count_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
		construct->connect_sg(start_const,wc_mux,INPUT_PIN_MUX_SWITCHABLE);
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
		if (count_info->population()!=3)
		    return(0);
		printf("AbsCounter:sg_designs working\n");
		count_start=count_info->get(0);
		count_duration=count_info->get(1);
		count_stride=count_info->get(2);

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
		if (count_info->population()!=3)
		    return(0);
		printf("AbsCounter:sg_setup working\n");

		count_start=count_info->get(0);
		count_duration=count_info->get(1);
		count_stride=count_info->get(2);

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

