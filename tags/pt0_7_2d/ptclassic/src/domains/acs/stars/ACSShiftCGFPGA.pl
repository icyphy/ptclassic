defcore {
	name { Shift }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Shift }
	desc {Scale by a power of 2}
	version{ @(#)ACSShiftCGFPGA.pl	1.13 08/02/01 }
	author { P. Fiore }
	copyright { 
Copyright (c) 1998-2001 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	htmldoc {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
	defstate {
	    name {Input_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	}
	defstate {
	    name {Input_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input}
	    default {0}
	}
	defstate {
	    name {Output_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the output}
	    default {0}
	}
	defstate {
	    name {Output_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the output}
	    default {0}
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
	    
	    int shift_amt;

	    static const int DEBUG_SHIFT=0;
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
		shift_amt=(int) (corona.shift);                // the amount to shift

		cost_file << "cost=zeros(1,size(insizes,2));" << endl;
                cost_file << " if sum(numforms)>0 " << endl;
                cost_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                cost_file << " end " << endl;

		// numsim_file << "y=x*2^(" << shift_amt << ");" << endl;
                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
                numsim_file <<  "   y{k}=x{k}*2^( "<< shift_amt << ");"  << endl;
                numsim_file <<  " end " << endl;
                numsim_file <<  " " << endl;

		rangecalc_file << "orr=inputrange*2^(" << shift_amt << ");" << endl;
		natcon_file << "wi=msbranges(1)-insizes+" << shift_amt << "+1;" << endl;
		natcon_file << "wo=msbranges(2)-outsizes+1;" << endl;
		natcon_file << "yesno=(wo>=wi);" << endl;

                // this is ok because shift latency does not depend on wordlength
                schedule_file << " vl1=veclengs(1); " << endl;
                schedule_file << " racts1=[0 1 vl1-1 ; 0 1 vl1-1 ];" << endl;
                schedule_file << " racts=cell(1,size(outsizes,2));" << endl;
                schedule_file << " racts(:)=deal({racts1});" << endl;
                schedule_file << " minlr=vl1*ones(1,size(outsizes,2)); " << endl;
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
		shift_amt=(int) (corona.shift);                // the amount to shift

		// Check if locked by user from PTOLEMY graph
		if (pins->query_preclock(1)==UNLOCKED )
                {
		    // get input size/msb
		    int inmsb=pins->query_majorbit(0);
		    int inlen=pins->query_bitlen(0);
		    
		    int outlen = inlen;
		    int outmsb = inmsb+shift_amt;
		    pins->set_precision(1,outmsb,outlen,lock_mode);
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
		acs_existence=SOFT;

	        // Input port definitions
		pins->add_pin("inp","input",INPUT_PIN);

		// Output port definitions
		pins->add_pin("outp","output",OUTPUT_PIN);

		// Bidir port definitions
		
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
		     output_filename << ".vhd" << ends;

		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");
		    
		    // Input port definitions
		    pins->set_datatype(0,STD_LOGIC); 
		    
		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC);
		
		    // Bidir port definitions

   		    // Control port definitions
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
		{
		    VHDL_LANG* lang=new VHDL_LANG;
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
		    int inlen =pins->query_bitlen(0);
		    int inmsb=pins->query_majorbit(0);
		    int outlen =pins->query_bitlen(1);
		    int outmsb=pins->query_majorbit(1);


		    //  Calc number of extra sign bits 
		    int numextrasignbits = outmsb - inmsb - shift_amt;
		    if (DEBUG_SHIFT)
			printf("outmsb=%d, inmsb=%d, shift_amt=%d\n",
			       outmsb,
			       inmsb,
			       shift_amt);


		    // Trap for error
		    if (numextrasignbits<0)
			printf("ACSShiftCGFPGA:Error, in numextrasignbits\n");

		    // Calc number of consecutive bits to copy straight over
		    int numstraightcopys = (int) min(inlen,
                       inmsb-outmsb+outlen+shift_amt); 

		    // Calc number of zeropads
		    int numzeropads= outlen-numextrasignbits-numstraightcopys; 

		    //sign extend
                    int loop;
		    for (loop=1; loop <= numextrasignbits; loop++)
			out_fstr << lang->equals(lang->slice("outp",outlen-loop),lang->slice("inp",inlen-1))
			    << lang->end_statement << endl;

		    //straightcopy
		    for (loop=1; loop <= numstraightcopys; loop++)
			out_fstr << lang->equals(lang->slice("outp",outlen-loop-numextrasignbits),
						 lang->slice("inp",inlen-loop)) 
			    << lang->end_statement << endl;

		    //zero pad
		    for (loop=1; loop <= numzeropads; loop++)
			out_fstr << lang->equals(lang->slice("outp",
			       outlen-loop-numextrasignbits-numstraightcopys),"GND")
			    << lang->end_statement << endl;

		    // END-USER CODE
		    out_fstr << lang->end_scope << lang->end_statement << endl;
		    out_fstr.close();
		    printf("Core %s has been built\n",name());
		    
		    // Return happy condition
		    return(1);
		}

		else
		    return(0);
		
	    }
	}
     go {
      addCode(block);
	}
	codeblock (block) {
// Multi Input star
	}
}

