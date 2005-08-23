defcore {
	name { Lut }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { Lut }
	desc { Look-up Table }
        version { @(#)ACSLutCGFPGA.pl	1.2 11/22/00}
        author { J. Ramanathan }
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
	defstate {
	    name {Depth}
	    type {int}
	    desc {Depth of LUT (Must be a multiple of 16)}
	    default {16}
	}
	defstate {
	    name {Coeff_File}
	    type {string}
	    desc {Location of coeffients file}
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
		int imb=intparam_query("Input_Major_Bit");
		int ibl=intparam_query("Input_Bit_Length");
		int omb=intparam_query("Output_Major_Bit");
		int obl=intparam_query("Output_Bit_Length");
		int depth=intparam_query("Depth");
		
		if (depth==16)
		    cost_file << "cost=floor(" << ibl << "/2)*ones(1,size(insizes,2));" << endl;
                else if (depth==32)
		    cost_file << "cost=" << ibl << "*ones(1,size(insizes,2));" << endl;
		else if (depth==48)
		    cost_file << "cost=2*" << ibl << "*ones(1,size(insizes,2));" << endl;
		else
		    cost_file << "cost=" << depth << "/16*(floor(" << ibl << "/2)+1);" << endl;
			 
		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=2^(omb-obl+1)*floor(2^(obl-omb-1)*(rand(1)*2^(omb+1)-2^omb)); " << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;
		
		rangecalc_file << "orr=[" << -2^omb << " " << 2^omb-2^(omb-obl+1) << "];" << endl;
		
		natcon_file << "yesno=(insizes=" << ibl << " & outsizes=" << obl << ");" << endl;

		// this is ok because const latency does not depend on wordlength
		schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << " racts1=[0 1 vl1-1; 1 1 vl1];" << endl;
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
		acs_existence=SOFT;
		
		if (fpga_type()==XC4000)
		{
		    // Input port definitions
		    pins->add_pin("a","input",INPUT_PIN);
		    pins->set_min_vlength(0,4);
		    pins->set_max_vlength(0,8);
		    
		    // Output port definitions
		    pins->add_pin("q","output",OUTPUT_PIN);
		    pins->set_min_vlength(1,2);
		    pins->set_max_vlength(1,31);
		    
		    // Bidir port definitions
			
		    // Control port definitions
		    pins->add_pin("c",INPUT_PIN_CLK);
		    pins->add_pin("ce",INPUT_PIN_CE);
		    pins->add_pin("clr",INPUT_PIN_CLR);
		}
		else if (fpga_type()==VIRTEX)
		{
		    // Input port definitions
		    pins->add_pin("A","input",INPUT_PIN);
		    pins->set_min_vlength(0,4);
		    pins->set_max_vlength(0,8);

		    // Output port definitions
		    pins->add_pin("QSPO","output",OUTPUT_PIN);
		    pins->set_min_vlength(1,1);
		    pins->set_max_vlength(1,256);
		    
		    // Bidir port definitions
			
		    // Control port definitions
		    pins->add_pin("CLK",INPUT_PIN_CLK);
		    pins->add_pin("QSPO_CE",INPUT_PIN_CE);
		    pins->add_pin("QSPO_RST",INPUT_PIN_CLR);
		}
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
		    // BEGIN-USER CODE
		    // General defintions
		    libs->add("IEEE");
		    incs->add(".std_logic_1164.all");

		    // Input port definitions
		    pins->set_datatype(0,STD_LOGIC);  // a port

		    // Output port definitions
		    pins->set_datatype(1,STD_LOGIC);  // q port
		
		    // Bidir port definitions

		    // Control port definitions
		    pins->set_datatype(2,STD_LOGIC);  // c port
		    pins->set_precision(2,0,1,LOCKED);
		    pins->set_datatype(3,STD_LOGIC);  // ce port
		    pins->set_precision(3,0,1,LOCKED);
		    pins->set_datatype(4,STD_LOGIC);  // clr port
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

                    ostrstream poutput_filename;
                    poutput_filename << dest_dir << tolowercase(output_filename.str()) << ends;
                    ofstream out_fstr(poutput_filename.str());
		    
                    ostrstream corelib_filename;
                    corelib_filename << dest_dir << "coregen.prj" << ends;
                    ofstream corelib_fstr(corelib_filename.str());

		    // Read/shift data values
		    mb = intparam_query("Output_Major_Bit");
		    bl = intparam_query("Output_Bit_Length");

		    ifstream incoe(stringparam_query("Coeff_File"));
		    ostrstream coeffs;
		    float temp;
		    int fixed;
		    incoe >> temp;
		    char delim=' ';
		    while(!incoe.eof())
		    {
			coeffs << delim;
			delim = ',';
			fixed = (int)(temp * pow(2,bl-mb-1));
			if (fixed < -pow(2,bl-1))
			{
			    coeffs << pow(2,bl-1);
			}
			else if (fixed > (pow(2,bl-1)-1))
			{
			    coeffs << pow(2,bl-1)-1;
			}
			else if (fixed >= 0)
			{
			    coeffs << fixed;
			}
			else
			{
			    coeffs << fixed + pow(2,bl);
			}
			incoe >> temp;
		    }
		    coeffs << ends;

		    // Build instructions
                    black_box=1;

		    if (fpga_type()==XC4000)
		    {
			// Library info	    
			corelib_fstr << "Registered_ROM|xilinx|xc4000_all|1.0=active" <<endl;
			
			// Core generator info
			out_fstr << "SET BusFormat = BusFormatParen" << endl;
			out_fstr << "SET SimulationOutputProducts = VHDL" << endl;
			out_fstr << "SET ViewlogicLibraryAlias = primary" << endl;
			out_fstr << "SET XilinxFamily = XC4000" << endl;
			out_fstr << "SET DesignFlow = VHDL" << endl;
			out_fstr << "SET FlowVendor = Synplicity" << endl;
			out_fstr << "SELECT Registered_ROM  XC4000 Xilinx 1.0" << endl;
			out_fstr << "CSET signed = false" << endl;
			out_fstr << "CSET component_name = " << tolowercase(output_filename.str()) << endl;
			out_fstr << "CSET default = 0" << endl;
			out_fstr << "CSET data_width = " << bl << endl;
			out_fstr << "CSET radix = 10" << endl;
			out_fstr << "CSET depth = " << intparam_query("Depth") << endl;
			out_fstr << "CSET memdata =" << coeffs.str() << endl;
			out_fstr << "GENERATE" << endl;
		    }
		    else if (fpga_type()==VIRTEX)
		    {
			ostrstream coename;
			coename << dest_dir << tolowercase(output_filename.str()) << ".coe" << ends;
			ofstream coe(coename.str());
			coe << "memory_initialization_radix = 10;" << endl;
			coe << "memory_initialization_vector =" << coeffs.str() << ";" << endl;
			coe.close();

			// Library info	    
			corelib_fstr << "distributed_memory|xilinx|virtex_all+virtex2|3.0=active" <<endl;
			
			// Core generator info
			out_fstr << "SET BusFormat = BusFormatParen" << endl;
			out_fstr << "SET SimulationOutputProducts = VHDL" << endl;
			out_fstr << "SET ViewlogicLibraryAlias = primary" << endl;
			out_fstr << "SET XilinxFamily = Virtex" << endl;
			out_fstr << "SET DesignFlow = VHDL" << endl;
			out_fstr << "SET FlowVendor = Synplicity" << endl;
			out_fstr << "SELECT Distributed_Memory Virtex Xilinx,_Inc. 3.0" << endl;
			out_fstr << "CSET coefficient_file = " << coename.str() << endl;
			out_fstr << "CSET default_data_radix = 10" << endl;
			out_fstr << "CSET component_name = " << tolowercase(output_filename.str()) << endl;
			out_fstr << "CSET reset_qdpo = false" << endl;
			out_fstr << "CSET output_options = registered" << endl;
			out_fstr << "CSET multiplexer_construction = lut_based" << endl;
			out_fstr << "CSET data_width = " << bl << endl;
			out_fstr << "CSET dual_port_address = non_registered" << endl;
			out_fstr << "CSET pipeline_stages = 1" << endl;
			out_fstr << "CSET default_data = 0" << endl;
			out_fstr << "CSET depth = " << intparam_query("Depth") << endl;
			out_fstr << "CSET common_output_ce = false" << endl;
			out_fstr << "CSET memory_type = rom" << endl;
			out_fstr << "CSET input_options = non_registered" << endl;
			out_fstr << "CSET create_rpm = true" << endl;
			out_fstr << "CSET reset_qspo = true" << endl;
			out_fstr << "CSET qualify_we_with_i_ce = false" << endl;
			out_fstr << "CSET read_enable = false" << endl;
			out_fstr << "CSET common_output_clk = false" << endl;
			out_fstr << "GENERATE" << endl;
		    }
		    corelib_fstr.close();
		    out_fstr.close();

                    if (fork()==0)
                    {
                        chdir(dest_dir);
                        if (execlp("coregen",
                                   "coregen",
                                   "-b",
                                   poutput_filename.str(),
                                   "-p",
                                   ".",
                                   (char*) 0)==-1)
                            printf("ACSLutCGFPGA:Error, unable to spawn a coregen session\n");
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
		addCode(trueblock);
	}
}


