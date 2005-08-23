defcore {
	name { Gain }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Cos }
	desc {
Produces constant coefficient multiply
	}
	version {@(#)ACSGainCGFPGA.pl	1.0	4 August 1999}
	author { P. Fiore }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
        ccinclude { <sys/ddi.h> }
        ccinclude { <sys/wait.h> }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	defstate {
	    name {Gain_Amt}
	    type {int}
	    desc {Gain multiplier (integer)}
	    default {0}
	}
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
	    default {"None"}
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
	    arglist { "(ofstream& cost_file, ofstream& numsim_file, ofstream& rangecalc_file, ofstream& natcon_file)" }
	    type {int}
	    code {

		// BEGIN-USER CODE

		    int cwidth=0; //coefficient wordlength
		    int gain_amt = (int) (Gain_Amt);                      // the gain amount
		int	fl2ag =(int) floor(log(abs( (double) gain_amt))/log(2.0));
		    // trying to figure out coefficient wordlength (signed coeff)
		       if (fl2ag > 0)
			   cwidth =fl2ag+2;
		       if (fl2ag < 0)
			   cwidth =fl2ag+1;
		       if (fl2ag == 0)
			   cwidth =2;  //a zero multiplier



                     //output width will be cwidth+input_bit_length
    cost_file << "cost= ceil((insizes/2-1)*(" << cwidth << "/2+2)+insizes*0.7805);"  << endl;
    cost_file << "cost= max([zeros(1,size(insizes,2)) ; cost]);" << endl;
		numsim_file << "y= x*" << gain_amt << ";" << endl;
 	        rangecalc_file <<"orr=inputrange*" << gain_amt << ";"  << endl;
                natcon_file 
		    << "yesno=(insizes>=4 & insizes<=32  & outsizes<=58 & outsizes<=insizes+ " <<  cwidth  << " );"
		    << endl;
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

		    int cwidth=0; //coefficient wordlength
		    int gain_amt = (int) (Gain_Amt);                      // the gain amount
		int	fl2ag =(int) floor(log(abs( (double) gain_amt))/log(2.0));
		    // trying to figure out coefficient wordlength (signed coeff)
		       if (fl2ag > 0)
			   cwidth =fl2ag+2;
		       if (fl2ag < 0)
			   cwidth =fl2ag+1;
		       if (fl2ag == 0)
			   cwidth =2;  //a zero multiplier
			       
			       int input_majorbit = pins->query_majorbit(0);
			       int input_bitlen = pins->query_bitlen(0);

		pins->set_precision(1, cwidth-2+input_majorbit,  input_bitlen+cwidth, LOCKED);



		// Calculate CLB sizes
		    
		// Calculate pipe delay
		int in_bitlen=pins->query_bitlen(0);
		if ((in_bitlen >=4) && (in_bitlen <= 5))
		    acs_delay=1;
		else if  ((in_bitlen >=6) && (in_bitlen <= 10))
		    acs_delay=2;
		else if  ((in_bitlen >=11) && (in_bitlen <= 17))
		    acs_delay=3;
		else if (in_bitlen > 17)
		    acs_delay=4;
		else
		    acs_delay=-1;

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
		pins->add_pin("a","input",INPUT_PIN);
		pins->set_min_vlength(0,4);
		pins->set_max_vlength(0,32);

		// Output port definitions
		pins->add_pin("prod","output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("c",INPUT_PIN_CLK);
	
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
		    pins->set_datatype(0,STD_LOGIC);  // inp port
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC);  // output port
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // c pin
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_precision(3,0,1,LOCKED);
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

		    int cwidth=0; //coefficient wordlength
		 int   gain_amt = (int) (Gain_Amt);                      // the gain amount
		int	fl2ag =(int) floor(log(abs( (double) gain_amt))/log(2.0));
		    // trying to figure out coefficient wordlength (signed coeff)
		       if (fl2ag > 0)
			   cwidth =fl2ag+2;
		       if (fl2ag < 0)
			   cwidth =fl2ag+1;
		       if (fl2ag == 0)
			   cwidth =2;  //a zero multiplier

		    output_filename << ends;

		    ostrstream poutput_filename;
		    poutput_filename << dest_dir << tolowercase(output_filename.str()) << ends;
		    ofstream out_fstr(poutput_filename.str());

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
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Math/Multipliers/Constant Coefficient Multipliers/Pipelined Constant Coefficient Multiplier" << endl;
		    out_fstr << "GSET Coef_Width = " 
  		     << cwidth << endl;
		    out_fstr << "GSET Coefficient = " << gain_amt << endl;

		    out_fstr << "GSET A_Width = " 
			     << pins->query_bitlen(0) << endl;
		    out_fstr << "GSET Component_Name = " 
			     << tolowercase(output_filename.str()) 
			     << endl;
		    out_fstr << "GSET Signed_Coefficient = true" << endl;
		    out_fstr << "GENERATE" << endl;
		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   poutput_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSGainCGFPGA:Error, unable to spawn a coregen session\n");		
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

