defcore {
	name { FIR }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { FIR }
	desc {FIR Filter}
	version{ @(#)ACSFIRCGFPGA.pl	1.14 08/02/01 }
	author { K. Smith }
	copyright {
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
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
	    name {Coefficient_Precision}
	    type {int}
	    desc {Number of bits to represent coefficients (4 to 24)}
	    default {4}
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

	    int taps;
	    int coef_prec;
	    int input_width;
	    int output_width;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		input_list->add("Input_Major_Bit");
		input_list->add("Input_Bit_Length");
		output_list->add("Output_Major_Bit");
		output_list->add("Output_Bit_Length");
		    
		// Return happy condition
		return(1);
	    }
	}
	method {
	    name {sg_cost}
	    access {public}
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file, ofstream& schedule_file)" }
	    type {int}
	    code {
		// BEGIN-USER CODE
		cost_file << "% Xilinx core sketchy on costs, need table" << endl
		          << "% for now, a first order approximation" << endl
			  << "cost=ceil(max(insizes)-1/32)+ceil("
			  << corona.taps.size() << "/4)*(" 
			  << coef_prec << "+2)"
			  << ";" << endl;
		double sum=0;
		for (int loop=corona.taps.size()-1;loop >= 0;loop--)
		{
		    if (corona.taps[loop] < 0)
			sum-=corona.taps[loop];
		    else
			sum+=corona.taps[loop];
		}
		
//		numsim_file << "y=x*" << sum << ";" << endl << "%Note: for range and variance calc only!" << endl;

                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k}* " << sum << ";" << endl;
                numsim_file <<  " end " << endl;
		numsim_file << "%Note: for range and variance calc only!" << endl;
                numsim_file <<  " " << endl;

		rangecalc_file << "orr=inputrange*" << sum << ";" << endl;
		natcon_file << "yesno=(insizes>=4 & insizes<=16);" << endl;

                schedule_file << "outdel= " << corona.taps.size() << ";" << endl;
                schedule_file << "vl1=veclengs(1); " << endl;
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
	    name {sg_bitwidths}
	    access {public}
	    arglist { "(int lock_mode)" }
	    type {int}
	    code {
		//
		// Calculate BW
		//
		// FIX:
		int out_bitlen=0;
		if (pins->query_preclock(1)==UNLOCKED)
		{
		    int in_majorbit=pins->query_majorbit(0);
		    int in_bitlen=pins->query_bitlen(0);
		    int out_majorbit=in_majorbit+1;
		    out_bitlen=out_majorbit - (in_majorbit-in_bitlen);
		    
		    input_width=in_bitlen;
		    output_width=out_bitlen;
		    
		    // Set
		    pins->set_precision(1,out_majorbit,out_bitlen,lock_mode);
		}

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
		acs_delay=corona.taps.size();
			    
		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		output_filename << name() << ends;

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=BOTH;
		acs_existence=HARD;

		coef_prec=intparam_query("Coefficient_Precision");

	        // Input port definitions
		pins->add_pin("data_in","input",INPUT_PIN);

		// Output port definitions
		pins->add_pin("data_out","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("ck",0,1,INPUT_PIN_CLK);

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

		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");
		    
		    // Input port definitions
		    pins->set_datatype(0,STD_LOGIC); // in port
		    pins->set_min_vlength(0,4);
		    pins->set_max_vlength(0,16);
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC); // out port
//		    pins->set_min_vlength(1,2);
//		    pins->set_max_vlength(1,32);

		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // clk pin
		    pins->set_precision(2,0,1,LOCKED);
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
		    input_width=pins->query_bitlen(0);
		    output_width=pins->query_bitlen(1);

		// Trap for language and generate appropriate code
		if (sg_language==VHDL_BEHAVIORAL)
		// BEGIN-USER CODE
		{
		    if (DEBUG_BUILD)
			printf("Building %s\n",output_filename.str());

		    ostrstream dired_filename;
		    dired_filename << dest_dir << tolowercase(output_filename.str())
			           << ends;
		    ofstream out_core(dired_filename.str());
		    if (DEBUG_BUILD)
			printf("open file for coregen as %s\n",dired_filename.str());
		    // Build instructions
		    black_box=1;

		    out_core << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_core << "SET XilinxFamily = All_XC4000_Families" << endl;
		    out_core << "SET BusFormat = BusFormatAngleBracket" << endl;
		    out_core << "SET TargetSymbolLibrary = primary" << endl;
		    out_core << "SET OVERWRITEFILES=true" << endl;
		    out_core << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/DSP/Filters/FIR Filters/Parallel Distributed Arithmetic/PDA FIR Filter" << endl;
		    out_core << "GSET Coef_Width = " << coef_prec << endl;

		    if (DEBUG_BUILD)
			printf("coefficients.size=%d\n",corona.taps.size());

		    // scale coefficients
		    float max_amp = 0.0;
		    float amp;
		    long * new_coefficients = new long[corona.taps.size()];
		    int loop;

		    // find maximum tap amplitude
		    for (loop=corona.taps.size()-1;loop >= 0;loop--)
		      {
			amp = abs(corona.taps[loop]);
			if (amp > max_amp) 
			  max_amp = amp;
		      }
		    if (max_amp >= 1.0)
		      {
			long msb_weight = 1;
			long bit_count = 1;
			while (max_amp > double(msb_weight))
			  {
			    msb_weight *= 2;
			    bit_count++;
			  }
			// bit_count has number of integer bits needed
			// (one additional bit needed for sign bit)
			int frac_bits = Coefficient_Precision - 1 - bit_count;
			long scale = 1;
			for (loop = 0; loop < frac_bits; loop++)
			  scale *= 2;
			for (loop=corona.taps.size()-1;loop >= 0;loop--)
			  new_coefficients[loop] = int(double(scale)*corona.taps[loop]+0.5);
		      }
		    else  // max_amp < 1.0, purely fractional coefficients
		      {
			// determine scale factor based on Coefficient_Precision (one sign bit)
			long scale = 1;
			for (loop = 0; loop < Coefficient_Precision - 1; loop++)
			  scale *= 2;
			for (loop=corona.taps.size()-1;loop >= 0;loop--)
			  new_coefficients[loop] = int(double(scale)*corona.taps[loop]+0.5);
		      }

		    for (loop=corona.taps.size()-1;loop >= 0;loop--)
			out_core << "GSET CoefData" << loop << " = "
			    << new_coefficients[loop] << endl;

		    delete []new_coefficients;

		    out_core << "GSET Signed_Input_Data = true" << endl;
		    out_core << "GSET Number_Of_Taps = " << corona.taps.size() << endl;

		    // FIX:Fixed for now:
		    out_core << "GSET Antisymmetry = false" << endl;
//		    out_core << "GSET Trim_Empty_ROMS = true" << endl;
		    out_core << "GSET Cascade = false" << endl;
		    out_core << "GSET Symmetry = true" << endl;
		    
		    
		    out_core << "GSET Output_Width = " 
			     << output_width << endl;

		    out_core << "GSET Input_Width = " 
			     << input_width << endl;
		    out_core << "GSET Component_Name = " 
			     << tolowercase(output_filename.str()) << endl;
		    out_core << "GENERATE" << endl;
		    out_core.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   dired_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSFIRCGFPGA:Error, unable to spawn a coregen session\n");
		    }
	            else
			wait ((int*) 0);
		}
                // END-USER CODE
		else
		    return(0);

             // Return happy condition
	     return(1);
	    }
	}
	go {
		addCode(block);
	}
	codeblock (block) {
// Multi Input star
	}
}


