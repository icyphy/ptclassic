defcore {
	name { Latch }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Latch }
	desc {
	    Implements a synthesizable D-type latch with synchronous set and clear
	}
	version {@(#)ACSLatchCGFPGA.pl	1.0 08/25/00}
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
	}
 	method {
	    name {sg_cost}
	    access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file, ofstream& schedule_file)" }
	    type {int}
	    code {
		// BEGIN-USER CODE
		cost_file << "cost=ceil(0.5*insizes).*" << pipe_delay << ";" << endl;

		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=x{k};" << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;
		
		rangecalc_file << "orr=inputrange;" << endl;
		
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;
		
		// this is ok because single delay latency does not depend on wordlength
		schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << " racts1=[0 1 vl1-1; 1 1 vl1];" << endl;
		schedule_file << " racts=cell(1,size(insizes,2));" << endl;
		schedule_file << " racts(:)=deal({racts1});" << endl;
		schedule_file << " minlr=vl1*ones(1,size(insizes,2)); " << endl;
		schedule_file << " if sum(numforms)>0 " << endl;
		schedule_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
		schedule_file << " end " << endl;
		// END-USER CODE

		// Return happy condition
		return(1);
	    }
	}
       method {
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		// Calculate BW
		if (pins->query_preclock(1)==UNLOCKED)
		    pins->set_precision(1,
					pins->query_majorbit(0),
					pins->query_bitlen(0),
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
		acs_delay=1;
		
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
		pins->add_pin("latch_in",INPUT_PIN,AH);

		// Output port definitions
		pins->add_pin("latch_out",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("clk",INPUT_PIN_CLK);
		pins->add_pin("clr",INPUT_PIN_CLR,AL);
		pins->add_pin("ce",INPUT_PIN_CE,AH);
		pins->add_pin("mode",INPUT_PIN_MODE,AH);
		pins->add_pin("input_state",INPUT_PIN_CTRL,AH);

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
		    pins->set_precision(0,0,1,UNLOCKED);
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC);
		    pins->set_precision(1,0,1,UNLOCKED);
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_datatype(3,STD_LOGIC);
		    pins->set_datatype(4,STD_LOGIC);
		    pins->set_datatype(5,STD_LOGIC);
		    pins->set_datatype(6,STD_LOGIC);
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);
		    pins->set_precision(6,0,1,LOCKED);
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
		// FIX:Use vhdl library!
		//     latch is also been disable on purpose for set/reset only use!
		out_fstr << "process(clk,ce,clr,latch_in)" << endl
		         << "begin" << endl
			 << "if (clk'event) and (clk='1') then" << endl
			 << "\tif (clr='1') then" << endl
			 << "\t\tlatch_out <= '0';" << endl
                         << "\telsif (ce='1') then" << endl
			 << "\t\tif (mode='1') then" << endl
			 << "\t\t\tlatch_out <= input_state;" << endl
			 << "\t\telse" << endl
			 << "\t\t\tlatch_out <= latch_in;" << endl
			 << "\t\tend if;" << endl
			 << "\tend if;" << endl
			 << "end if;" << endl
			 << "end process;" << endl;
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
	    delete sg_constants;
	}

	codeblock (trueblock) {
	}
	codeblock (falseblock) {
	}
}

