defcore {
	name { Divider }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Divider }
	desc {
Produce the quotient of two numbers ( in1 / in2 )
	}
	version {@(#)ACSDividerCGFPGA.pl	1.0 10/06/00}
	author { J. Ramanathan }
	copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
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
	    name {Dividend_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Dividend_Bit_Length}
	    type {int}
	    desc {The total number of bits represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Divisor_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Divisor_Bit_Length}
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
                name {smallestnum}
                type {int}
                default {1}
                desc {Ceiling of Reciprocal of Smallest nonzero denominator.}
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
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		input_list->add("Dividend_Major_Bit");
		input_list->add("Dividend_Bit_Length");
		input_list->add("Divisor_Major_Bit");
		input_list->add("Divisor_Bit_Length");
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
     	  int smallestnum=intparam_query("smallestnum");
		float sm1;
		sm1 = .00390625 / (float) smallestnum; // 2^-8


		cost_file << "%Bad coregen, bad...bad" << endl;
  	        cost_file << "cost=ceil((max(insizes)-1).*min(insizes));" << endl;
                cost_file << " if sum(numforms)>0 " << endl;
                cost_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
                cost_file << " end " << endl;

                numsim_file <<  " y=cell(1,size(x,2));" << endl;
                numsim_file <<  " for k=1:size(x,2) " << endl;
 	 //       numsim_file <<  "   t=find(abs(x{2,k}) < 2^(-24)  ); " << endl;
 	        numsim_file <<  "   t=find(abs(x{2,k}) < " << sm1 << "  ); " << endl;
		numsim_file <<  "   if ~isempty(t)" << endl;
	//	numsim_file <<  "     x{2,k}=2^(-24)*ones(length(x{2,k}),1); " << endl;
		numsim_file <<  "     x{2,k}=" << sm1 << "*ones(length(x{2,k}),1); " << endl;
		numsim_file <<  "   end" << endl;
                numsim_file <<  "   y{k}=x{1,k}./x{2,k}; " << endl;
                numsim_file <<  " end " << endl;
		
		 rangecalc_file << "ir=inputrange;" << endl;
		 rangecalc_file << "numr=max(abs(ir(1,:)));" << endl;
 		 rangecalc_file << "if(min(ir(2,:)) <= 0 &  0 <=  max(ir(2,:)) )" << endl;
	//	 rangecalc_file << "      denr=2^-24; %straddle -- hack, you better hand modify...." << endl;
		 rangecalc_file << "      denr= " << sm1 << "; %straddle " << endl;
	         rangecalc_file << "else" << endl;
         	 rangecalc_file << "   denr= min(abs(ir(2,:)));" << endl;
                 rangecalc_file << "end		     " << endl;
		 rangecalc_file << "orr=[-numr/denr numr/denr];" << endl;
 
		natcon_file << "yesno = ( insizes(2,:) >= 3  & " 
		            << "insizes(2,:) <= 24 & insizes(1,:) >= 1 & insizes(1,:) <= 24 & "
			    << "outsizes <= insizes(1,:)+24 );" << endl;

//		schedule_file << "outdel =insizes(1,:)+3+3; " << endl;
// +1 contribution comes from int/remd fusion adder
		schedule_file << "outdel =insizes(1,:)+insizes(1,:)+3+1; " << endl;
                schedule_file << "vl1=veclengs(1); " << endl;
		schedule_file << "racts=cell(1,size(insizes,2));" << endl;
		schedule_file << "for k=1:size(insizes,2)" << endl;
                schedule_file << "  racts1=[0 1 vl1-1 ;0 1 vl1-1; outdel(k) 1 vl1-1+outdel(k)];" << endl;
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
		// Calculate BW
		int dividend_majorbit=pins->query_majorbit(0);
		int dividend_bitlen=pins->query_bitlen(0);

		// Set
		if (pins->query_preclock(2)==UNLOCKED)
		    pins->set_precision(2,dividend_majorbit,dividend_bitlen,lock_mode);

		// For now, match the fractional precision the intended integer precision of the output
		if (pins->query_preclock(3)==UNLOCKED)
		    pins->set_precision(3,dividend_majorbit,dividend_bitlen,lock_mode);

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
		// Revise precisions
		sg_bitwidths(UNLOCKED);

		// Calculate pipe delay
//		acs_delay=pins->query_bitlen(0)+3+3;
// +1 contribution comes from int/remd fusion adder
		acs_delay=pins->query_bitlen(0)+pins->query_bitlen(3)+3+1;
		printf("Divider:pin0_len=%d, pin3_len=%d, acs_delay=%d\n",
		       pins->query_bitlen(0),
		       pins->query_bitlen(3),
		       acs_delay);

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
		pins->add_pin("dividend","dividend",INPUT_PIN);
		pins->add_pin("divisor","divisor",INPUT_PIN);
		pins->set_max_vlength(0,24);
		pins->set_min_vlength(1,3);
		pins->set_max_vlength(1,24);

		// Output port definitions
		pins->add_pin("quot","output",OUTPUT_PIN);
		pins->add_pin("remd",OUTPUT_PIN_NOEXT);

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
		    pins->set_datatype(0,STD_LOGIC);  // dividend port
		    pins->set_datatype(1,STD_LOGIC);  // divisor port
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);  // quot port
		    pins->set_datatype(3,STD_LOGIC);  // remd port
//		    pins->set_precision(3,0,3,LOCKED); // remainder not used, but still listed

		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(4,STD_LOGIC);  // c pin
		    pins->set_precision(4,0,1,LOCKED);
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

		    Pin* new_pins=NULL;
		    ostrstream core_entity;
		    new_pins=dup_pins();

		    VHDL_LANG* lang=new VHDL_LANG;
		    ostrstream poutput_filename;
		    poutput_filename << dest_dir << output_filename.str() << ".vhd" << ends;
		    ofstream out_fstr(poutput_filename.str());

		    // Build instructions
		    child_filenames->add("divider.vhd");
		    child_filenames->add("add16.vhd");
		    out_fstr << "library IEEE;" << endl;
		    out_fstr << "use IEEE.std_logic_1164.all;" << endl << endl;
		    out_fstr << lang->gen_entity(output_filename.str(),pins) 
			       << endl;
		    out_fstr << lang->gen_architecture(name(),
						       NULL,
						       STRUCTURAL,
						       pins,
						       data_signals,
						       ctrl_signals,
						       constant_signals);
		    out_fstr << lang->start_component("divider");
		    out_fstr << lang->start_port() << endl;
		    out_fstr << lang->set_port(new_pins) << endl;
		    out_fstr << lang->end_port() << endl;
		    out_fstr << lang->end_component() << endl;
			
		    out_fstr << lang->begin_scope << endl;
		    out_fstr << "U_divider:divider port map(" << endl;
		    out_fstr << "dividend=>dividend," << endl;
		    out_fstr << "divisor=>divisor," << endl;
		    out_fstr << "quot=>quot," << endl;
		    out_fstr << "c=>c);" << endl;

		    out_fstr << lang->end_architecture(STRUCTURAL) << endl;
		    delete lang;
/*
		    black_box=1;

		    out_fstr << "SET SelectedProducts = ImpNetlist VHDLSym VHDLSim" 
			     << endl;
		    out_fstr << "SET XilinxFamily = All_XC4000_Families" 
			     << endl;
		    out_fstr << "SET BusFormat = BusFormatAngleBracket" 
			     << endl;
		    out_fstr << "SET TargetSymbolLibrary = primary" << endl;
		    out_fstr << "SET OVERWRITEFILES=true" << endl;
		    out_fstr << "GSET ModuleTaxonomy = CORE Generator Library/LogiCORE/Math/Dividers/Pipelined Divider" 
			     << endl;
		    out_fstr << "GSET signed_b = true" << endl;
		    out_fstr << "GSET divclk_sel = 1" << endl;
		    out_fstr << "GSET fractional_width = " pins->query_bitlen(3) << endl;
		    out_fstr << "GSET fractional_b = true" << endl;
		    out_fstr << "GSET divisor_width = " << pins->query_bitlen(1) << endl;
		    out_fstr << "GSET dividend_width = " << pins->query_bitlen(0) << endl;
		    out_fstr << "GSET Component_Name = " << tolowercase(output_filename.str()) << endl;
		    out_fstr << "GENERATE" << endl;
*/

		    out_fstr.close();

		    if (fork()==0)
		    {
			chdir(dest_dir);
			if (execlp("coregen",
				   "coregen",
				   poutput_filename.str(),
				   (char*) 0)==-1)
			    printf("ACSivGFPGA:Error, unable to spawn a coregen session\n");		
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

