defcore {
	name { 16FFT }
	domain { ACS }
	coreCategory { CGFPGA }
	corona { 16FFT }
	desc { Compute a 16-point complex FFT }
	version{ @(#)ACS16FFTCGFPGA.pl	1.2 08/02/01 }
	author { K. Smith }
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
        ccinclude { <sys/wait.h> }
	ccinclude { "acs_vhdl_lang.h" }
	ccinclude { "acs_starconsts.h" }
        hinclude { "Directory.h" }
	defstate {
	    name {Real_Input_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Real_Input_BitLength}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {16}
	    attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
	    name {Imag_Input_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Imag_Input_BitLength}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {16}
	    attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
	    name {Real_Output_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the output}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Real_Output_BitLength}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {16}
	    attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
	    name {Imag_Output_Major_Bit}
	    type {int}
	    desc {The highest order bit represented at the output}
	    default {0}
	    attributes {A_NONCONSTANT|A_SETTABLE}
	}
	defstate {
	    name {Imag_Output_BitLength}
	    type {int}
	    desc {The highest order bit represented at the input}
	    default {16}
	    attributes {A_NONCONSTANT|A_NONSETTABLE}
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
	    name {Comment}
	    type {string}
	    desc {A user-specified identifier}
	    default{""}
	}
        protected {
	    // Stitcher assignments
	    ostrstream output_filename;

	    // Directory containing pre-canned implementation files
	    Directory* vhdl_src;
	}
        constructor {
	    vhdl_src=new Directory;
	}
	destructor {
	    delete vhdl_src;
	}
	method {
	    name {sg_param_query}
	    access {public}
	    arglist { "(StringArray* input_list, StringArray* output_list)" }
	    type {int}
	    code {
		input_list->add("Real_Input_Major_Bit");
		input_list->add("Real_Input_BitLength");
		input_list->add("Imag_Input_Major_Bit");
		input_list->add("Imag_Input_BitLength");
		output_list->add("Real_Output_Major_Bit");
		output_list->add("Real_Output_BitLength");
		output_list->add("Imag_Output_Major_Bit");
		output_list->add("Imag_Output_BitLength");
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
		cost_file << "wl=max(msbranges(1:2)'*ones(1,size(insizes,2))-insizes+1);" << endl;
		cost_file << "wu=max(msbranges(1:2)');" << endl;
		cost_file << "cost=ceil((wu-wl+1)/2);" << endl;
		cost_file << " if sum(numforms)>0 " << endl;
		cost_file << "  disp('ERROR - use parallel numeric form only' )  " << endl;
		cost_file << " end " << endl;

		//  numsim_file << "y=sum(x);" << endl; 
		numsim_file <<  " y=cell(1,size(x,2));" << endl;
		numsim_file <<  " for k=1:size(x,2) " << endl;
		numsim_file <<  "   y{k}=x{1,k}+x{2,k}; " << endl;
		numsim_file <<  " end " << endl;
		numsim_file <<  " " << endl;

		rangecalc_file << "orr=sum(inputrange);" << endl;

		natcon_file << "wi=min(msbranges(1:2)'*ones(1,size(insizes,2)) -insizes+1);" << endl;
		natcon_file << "wo=msbranges(3)-outsizes+1;" << endl;
		natcon_file << "yesno=(wo>=wi);" << endl;
		natcon_file << "yesno=yesno & (max(insizes)<33) & (min(insizes)>1);" << endl;

		// this is ok because adder latency does not depend on wordlength
		schedule_file << " vl1=veclengs(1); " << endl;
		schedule_file << " racts1=[0 1 vl1-1 ;0 1 vl1-1; 1 1 vl1];" << endl;
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
		// Precisions are fixed for this implementation
		// No, bitwidth calculations needed

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
		acs_delay=76+12;
			    
		// Return happy condition
		return(1);
	    }
	}
        method {
	    name {sg_setup}
	    access {public}
	    type {int}
	    code {
		output_filename << name() << ".vhd" << ends;

		phase_dependent=1;
		delay_offset=-1;

		///////////////////////////////////
		// Language-independent assignments
		///////////////////////////////////
		// BEGIN-USER CODE
		// General defintions
		acs_type=BOTH;
		acs_existence=HARD;

		// Define as unique for declarative purposes
		// NOTE:This is due to the fixed representation of this
		//      Xilinx core
//		unique_component=1;
//		unique_name << "xfft16" << ends;
		    

	        // Input port definitions
		pins->add_pin("DR","real",INPUT_PIN);
		pins->add_pin("DI","imag",INPUT_PIN);

		// Output port definitions
		pins->add_pin("XK_R","real_output",OUTPUT_PIN);
		pins->add_pin("XK_I","imag_output",OUTPUT_PIN);

		// Bidir port definitions
		
		// Control port definitions
		pins->add_pin("CLK",0,1,INPUT_PIN_CLK);
		pins->add_pin("CE",0,1,INPUT_PIN_CE,AH);
		pins->add_pin("RS",0,1,INPUT_PIN_RESET,AH);
		pins->add_pin("START",0,1,INPUT_PIN_START,AH);

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
		    pins->set_datatype(0,STD_LOGIC);
		    pins->set_datatype(1,STD_LOGIC);
		    pins->set_min_vlength(0,16);
		    pins->set_max_vlength(0,16);
		    pins->set_min_vlength(1,16);
		    pins->set_max_vlength(1,16);
		    
		    // Output port definitions
		    pins->set_datatype(2,STD_LOGIC);
		    pins->set_datatype(3,STD_LOGIC);
		
		    // Bidir port definitions

   		    // Control port definitions
		    pins->set_datatype(4,STD_LOGIC);
		    pins->set_datatype(5,STD_LOGIC);
		    pins->set_datatype(6,STD_LOGIC);
		    pins->set_datatype(7,STD_LOGIC);
		    pins->set_precision(4,0,1,LOCKED);
		    pins->set_precision(5,0,1,LOCKED);
		    pins->set_precision(6,0,1,LOCKED);
		    pins->set_precision(7,0,1,LOCKED);
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
		    // FIX: This should use the directory structure vhdl_src...
		    char* src_dir=new char[PATH_MAX];
		    char* environ_path=getenv("PTOLEMY");
		    sprintf(src_dir,"%s%s",environ_path,"/src/domains/acs/vhdl_src/16fft/*");
//		    vhdl_src->set("$PTOLEMY/src/domains/acs/vhdl_src/16fft");

		    // Add info pertaining to child support files
		    child_filenames->add("radd16.vhd");
		    child_filenames->add("cmplxadd.vhd");
		    child_filenames->add("z5w16.vhd");
		    child_filenames->add("reg16.vhd");
		    child_filenames->add("sinn16.vhd");
		    child_filenames->add("fd.vhd");
		    child_filenames->add("fdre.vhd");
		    child_filenames->add("ftrse.vhd");
		    child_filenames->add("cb8re.vhd");
		    child_filenames->add("coss16.vhd");
		    child_filenames->add("triginv.vhd");
		    child_filenames->add("phases.vhd");
		    child_filenames->add("r4n256w.vhd");
		    child_filenames->add("dual_xlatch16.vhd");
		    child_filenames->add("fdre_locx.vhd");
		    child_filenames->add("fdre_locy.vhd");
		    child_filenames->add("cb2re.vhd");
		    child_filenames->add("xfft1024_cntrl.vhd");
		    child_filenames->add("z4_1_h.vhd");
		    child_filenames->add("mux4w16.vhd");
		    child_filenames->add("reg16_pitch2.vhd");
		    child_filenames->add("rmux4w16.vhd");
		    child_filenames->add("tcompw16.vhd");
		    child_filenames->add("rsub16.vhd");
		    child_filenames->add("mulxx.vhd");
		    child_filenames->add("cmplx_mul16.vhd");
		    child_filenames->add("cmplxsub16.vhd");
		    child_filenames->add("blyw0.vhd");
		    child_filenames->add("bflywj.vhd");
		    child_filenames->add("fft4.vhd");
		    child_filenames->add("r4butterfly.vhd");
		    child_filenames->add("fft256r4.vhd");
		    child_filenames->add("ramd1616.vhd");
		    child_filenames->add("mux2w4.vhd");
		    child_filenames->add("reg4.vhd");
		    child_filenames->add("mux2w4r.vhd");
		    child_filenames->add("mux21w16.vhd");
		    child_filenames->add("dfly_agen.vhd");
		    child_filenames->add("dblbufr.vhd");
		    child_filenames->add("z4_1.vhd");
		    child_filenames->add("bitrev_agen.vhd");
		    child_filenames->add("result.vhd");
		    child_filenames->add("bitrev_dbufr.vhd");
		    child_filenames->add("fft4r.vhd");
		    child_filenames->add("dblbufr1.vhd");
		    child_filenames->add("state_control.vhd");
		    child_filenames->add("fdc.vhd");
		    child_filenames->add("mux4w8.vhd");
		    child_filenames->add("reg8.vhd");
		    child_filenames->add("z17w8.vhd");
		    child_filenames->add("xfft16.vhd");
		    child_filenames->add("or2.vhd");
		    
		    // Copy all source files to the target directory
		    if (fork()==0)
		    {
			chdir(dest_dir);
/*
			if (execlp("cp",
				   "cp",
				   vhdl_src->retrieve(),
				   src_dir,
				   ".",
				   (char*) 0)==-1)
			    printf("ACS16FFTCGFPGA:Error, copy source files\n");
*/
		    }
	            else
			wait ((int*) 0);
			
		    // Now generate the wrapper
		    Pin* fft_pins=dup_pins();
		    Pin* or_pins=new Pin;
		    VHDL_LANG* lang=new VHDL_LANG;

		    or_pins->add_pin("kickoff",0,1,STD_LOGIC,INPUT_PIN);
		    or_pins->add_pin("chip_enable",0,1,STD_LOGIC,INPUT_PIN);
		    or_pins->add_pin("enabler",0,1,STD_LOGIC,OUTPUT_PIN);
		    
		    ofstream out_fstr(output_filename.str());
		    out_fstr << "library IEEE;" << endl;
		    out_fstr << "use IEEE.std_logic_1164.all;" << endl << endl;
		    out_fstr << lang->gen_entity(name(),pins) << endl;
		    out_fstr << lang->gen_architecture(name(),
						       NULL,
						       STRUCTURAL,
						       pins,
						       data_signals,
						       ctrl_signals,
						       constant_signals);

		    // FFT entity
		    out_fstr << lang->start_component("xfft16");
		    out_fstr << lang->start_port() << endl;
		    out_fstr << lang->set_port(fft_pins) << endl;
		    out_fstr << lang->end_port() << endl;
		    out_fstr << lang->end_component() << endl;

		    // OR2 entity
		    out_fstr << lang->start_component("or2");
		    out_fstr << lang->start_port() << endl;
		    out_fstr << lang->set_port(or_pins) << endl;
		    out_fstr << lang->end_port() << endl;
		    out_fstr << lang->end_component() << endl;

		    out_fstr << lang->signal("enable",STD_LOGIC,1);
			
		    // Should be reduced once hierarchies are functioning
		    out_fstr << lang->begin_scope << endl;
		    out_fstr << "U_or2:or2 port map (" << endl
			     << "\tkickoff=>START," << endl
			     << "\tchip_enable=>CE," << endl
			     << "\tenabler=>enable);" << endl << endl;
		    out_fstr << "U_xfft16:xfft16 port map (" << endl
			     << "\tDR=>DR," << endl
			     << "\tDI=>DI," << endl
			     << "\tXK_R=>XK_R," << endl
			     << "\tXK_I=>XK_I," << endl
			     << "\tCLK=>CLK," << endl
			     << "\tCE=>enable," << endl
			     << "\tRS=>RS," << endl
			     << "\tSTART=>START);" << endl;
		    out_fstr << lang->end_scope << " structural" 
			     << lang->end_statement << endl;
		    out_fstr.close();

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

