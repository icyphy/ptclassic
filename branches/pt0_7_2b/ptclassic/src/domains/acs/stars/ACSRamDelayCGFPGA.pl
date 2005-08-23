defcore {
	name { RamDelay }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { RamDelay }
	desc {
	    Generates a pipelined delay element using FPGA RAM
	}
	version {@(#)ACSRamDelayCGFPGA.pl	1.10 07/30/01}
	author { K. Smith }
	copyright {
Copyright (c) 1998-2000 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
	}
        ccinclude { <sys/wait.h> }
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
	    desc {The total number of bits represented at the output}
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
	    name {Delay_Impact}
	    type {string}
	    desc {How does this delay affect scheduling? (Algorithmic or None)}
	    default {"Algorithmic"}
	}
	defstate {
	    name {Domain}
	    type {string}
	    desc {Where does this function reside (HW/SW)}
	    default{"HW"}
	}
        defstate {
	    name {Device_Number}
	    type {int}
	    desc {Which device (e.g. fpga, mem)  will this smart generator build for (if applicable)}
	    default{0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Device_Lock}
	    type {int}
	    default {"NO"}
	    desc {Flag that indicates that this function must be mapped to the specified Device_Number}
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

	    int pipe_count;

	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(SequentialList* input_list,SequentialList* output_list)" }
	    type {int}
	    code {
		input_list->append((Pointer) "Input_Major_Bit");
		input_list->append((Pointer) "Input_Bit_Length");
		output_list->append((Pointer) "Output_Major_Bit");
		output_list->append((Pointer) "Output_Bit_Length");

		// BEGIN-USER CODE
		// Add one delay for preloading
		pipe_count = corona.delays;
		// END-USER CODE
		    
		// Return happy condition
		return(1);
	    }
	}
	method {
	    name {macro_query}
	    access {public}
	    type {int}
	    code {
		// BEGIN-USER CODE
		return(NORMAL_STAR);
		// END-USER CODE
	    }
	}
	method {
	    name {sg_cost}
	    access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file, ofstream& schedule_file)" }
	    type {int}
	    code {
		// BEGIN-USER CODE
		cost_file << "cost=ceil(" << pipe_count << "/16) * "
  		        << "ceil(0.5*insizes) + " 
			<< "ceil(log2(" << pipe_count << "));" << endl;

		// numsim_file << "y=x;" << endl;
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k};" << endl;
                numsim_file <<  " end " << endl;
                numsim_file <<  " " << endl;

		rangecalc_file << "orr=inputrange;" << endl;
		natcon_file << "yesno=ones(1,size(insizes,2));" << endl;

                // this is ok because ram delay latency does not depend on wordlength
                schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << "outdel=" << pipe_count << ";" << endl;
                schedule_file << "racts=cell(1,size(insizes,2));" << endl;
                schedule_file << "for k=1:size(insizes,2)" << endl;
                schedule_file << "  racts1=[0 1 vl1-1 ; outdel(k) 1 vl1-1+outdel(k)];" << endl;
                schedule_file << "  racts{k}=racts1;" << endl;
                schedule_file << "end"  << endl;
                schedule_file << "minlr=vl1*ones(1,size(insizes,2)); " << endl;
                schedule_file << "if sum(numforms)>0 " << endl;
                schedule_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                schedule_file << "end " << endl;

		// END-USER CODE

		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_resources}
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
		if (pins->query_preclock(0)==LOCKED)
		    pins->set_precision(1,
					pins->query_majorbit(0),
					pins->query_bitlen(0),
					lock_mode);

		// Calculate CLB sizes
		int depth=(int) ceil(pipe_count/16);
		int bit_depends=(int) ceil(pins->query_bitlen(0) / 2);
		int control_clb=(int) ceil((log(pipe_count)/log(2)));
		resources->set_occupancy(1,depth*bit_depends+control_clb);
		
		// Calculate pipe delay
		acs_delay = corona.delays;

		// Return happy condition
		return(1);
		}
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		output_filename << name();

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=BOTH;
		acs_existence=HARD;

	        // Input port definitions
		pins->add_pin("din","input",INPUT_PIN);
		pins->set_min_vlength(0,2);
		pins->set_max_vlength(0,32);

		// Output port definitions
		pins->add_pin("dout","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",INPUT_PIN_CLK);
		pins->add_pin("ce",INPUT_PIN_CE);

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
		    pins->set_datatype(0,STD_LOGIC);  // d port
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC); // q port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // clk pin
		    pins->set_datatype(3,STD_LOGIC);  // ce pin
		    pins->set_precision(2,1,1,LOCKED);
		    pins->set_precision(3,1,1,LOCKED);
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
		    output_filename << ends;
		    if (DEBUG_BUILD)
			printf("Building %s\n",output_filename.str());
		    
		    ostrstream filename;
		    filename << dest_dir << tolowercase(output_filename.str()) << ends;
		    ofstream out_fstr(filename.str());

		    // Build instructions
		    black_box=1;

		    out_fstr << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_fstr << "SET XilinxFamily = All_XC4000_Families" 
			     << endl;
		    out_fstr << "SET BusFormat = BusFormatAngleBracket" 
			     << endl;
		    out_fstr << "SET TargetSymbolLibrary = primary" << endl;
		    out_fstr << "SET OVERWRITEFILES=true" << endl;
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Memories/Delay Element" 
			     << endl;
		    out_fstr << "GSET Pipeline_Stages = " << pipe_count << endl;
		    out_fstr << "GSET Create_RPM = true" << endl;
		    out_fstr << "GSET Port_Width = " 
			     << pins->query_bitlen(0) 
			     << endl;
		    out_fstr << "GSET Component_Name = " 
			     << tolowercase(output_filename.str()) << endl;
		    out_fstr << "GENERATE" << endl;
		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   filename.str(),
				   (char*) 0)==-1)
			    printf("ACSDelayCGFPGA:Error, Unable to spawn a coregen session\n");

		    }
		    else
			wait((int*) 0);
		}
                // END-USER CODE
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

