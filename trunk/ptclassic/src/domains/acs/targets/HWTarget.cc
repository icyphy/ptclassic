static const char file_id[] = "HWTarget.cc";

/**********************************************************************
Copyright (c) 1999-2001 Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE SANDERS, A LOCKHEED MARTIN COMPANY BE LIABLE TO ANY 
PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

SANDERS, A LOCKHEED MARTIN COMPANY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SANDERS, A LOCKHEED MARTIN
COMPANY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
                                                        COPYRIGHTENDKEY


 Programmers:  Ken Smith
 Date of creation: 3/23/98
 Version: @(#)HWTarget.cc	1.5 08/02/01
***********************************************************************/
#include "HWTarget.h"

#ifdef __GNUG__
#pragma implementation
#endif


//////////////////////////////////////////////////////////////////////
// Parse the smart generator list and save scheduling info to a file.
// Invoke the tcl plotting routine to display the scheduling info as a
// separate process.
//////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWplot_schedule()
{
  ostrstream plot_file;
  plot_file << design_directory->retrieve_extended() << "schedule_graph.txt" << ends;

  FILE *fp;
  fp=fopen(plot_file.str(),"w");

  fprintf(fp,"%d\n",seqlen);

  char* star_name=new char[MAX_STR];
  char* lc_starname=new char[MAX_STR];

  // Loop over all the smart generators
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
  
      strcpy(star_name,fpga_core->comment_name());
      
      // Convert starname to lower case for Tcl usage
      strcpy(lc_starname,star_name);
      char *cptr=lc_starname;
      for (unsigned int loop=0;loop<strlen(lc_starname);loop++)
	{
	  *cptr=tolower(*cptr);
	  cptr++;
	}
	  
      fprintf(fp,"%s%d\n",star_name,fpga_core->acs_id);
      fprintf(fp,"%s%d\n",lc_starname,fpga_core->acs_id);
      fprintf(fp,"%d\n",fpga_core->act_input);
      fprintf(fp,"%d\n",fpga_core->act_output);
    }

  // Cleanup
  delete []star_name;
  delete []lc_starname;

  fclose(fp);

  if (fork()==0)
    {
      chdir(design_directory->retrieve());
      if (execlp("itkwish","itkwish","~jmsmith/plot_schedule.tk",(char*) 0)==-1)
	printf("HWplot_schedule:Unable to spawn a wish interpreter\n");
    }
  else
    wait ((int*) 0);
}


//////////////////////////////////////////////////////
// Establish the hardware resources that are available
//////////////////////////////////////////////////////
void ACSCGFPGATarget::HWinitialize()
{
  // Determine path to VHDL for Annapolis Wildforce libraries
  StringList current_hwstate=hw_state.currentValue();
  const char* set_hwpath=(const char *)current_hwstate;
  hw_path=new Directory;
  hw_path->set(set_hwpath);

  // Determine path for simulator
  StringList current_simstate=sim_state.currentValue();
  const char* set_simpath=(const char *)current_simstate;
  sim_path=new Directory;
  sim_path->set(set_simpath);

  // Determine path for Xilinx coregen
  StringList current_corestate=core_state.currentValue();
  const char* set_corepath=(const char *)current_corestate;
  core_path=new Directory;
  core_path->set(set_corepath);

  // Determine execution profile
  StringList current_buildstate=build_state.currentValue();
  const char* current_build=(const char *)current_buildstate;
  if ((strcmp(current_build,"Yes")==0) ||
      (strcmp(current_build,"YES")==0) ||
      (strcmp(current_build,"yes")==0) ||
      (strcmp(current_build,"y")==0))
    do_vhdlbuild=TRUE;
  else
    do_vhdlbuild=FALSE;

  // Determine path for resultant VHDL code
  StringList current_ps=project_state.currentValue();
  const char* ps_str=(const char *)current_ps;
  design_directory=new Directory;
  if (strcmp(ps_str,"")==0)
    {
      printf("Warning: No path set, will use current directory for output\n");
      design_directory->set("./");
    }
  else
    {
      design_directory->set(ps_str);
      design_directory->make_directory();
    }

  // Determine path for building resultant VHDL code, if blank, use the design_directory
  StringList compile_strlist=compile_state.currentValue();
  const char* compile_str=(const char *)compile_strlist;
  compile_directory=new Directory;
  if (strcmp(compile_str,"")==0)
    compile_directory->set(design_directory->retrieve_extended());
  else
    {
      compile_directory->set(compile_str);
      compile_directory->make_directory();
    }

  printf("All output will be sent to %s\n",design_directory->retrieve());

  // Determine if wordlength analysis should be conducted
  StringList current_wordstate=word_state.currentValue();
  const char* current_word=(const char *)current_wordstate;
  if ((strcmp(current_word,"Yes")==0) ||
      (strcmp(current_word,"YES")==0) ||
      (strcmp(current_word,"yes")==0) ||
      (strcmp(current_word,"y")==0))
    do_wordlength=TRUE;
  else
    do_wordlength=FALSE;

  
  // Determine the vector size for processing
  IntState* int_state=&vlen_state;
  vector_length=(int) (*int_state);
  automatic_wordlength=0;
/*
  if (vector_length<0)
    automatic_wordlength=1;
  else
    {
      automatic_wordlength=0;
      multirate=0;
    }
    */

  // Make sure the coregen is installed! If VHDL is to be built,
  // otherwise the "Hydra" will be unleashed
  if (do_vhdlbuild==TRUE)
    {
      char* environ_path=getenv("COREGEN");
      if (environ_path==NULL)
	{
	  fprintf(stderr, "WARNING:COREGEN environment variable not set, is\n");
	  fprintf(stderr, " Xilinx Coregen installed? vhdlbuild cancelled.\n");
	  do_vhdlbuild=FALSE;
	}
    }

  // Assist the memory manager
  memory_monitor();

  // Set error string
  err_message=new char[MAX_STR];

  // Smart generator list initializations
  smart_generators=new CoreList;

  // Identifier initialization
  free_netlist_id=1;
  free_acsid=0;
  free_act=0;
  free_signal=0;

  // Accumulated list initializations
  all_libraries=new StringArray;
  all_includes=new StringArray;

  // Determine path and filename for the target architecture descriptor
//  StringList arch_strlist=arch_state.currentValue();
//  char* arch_str=(const char *)arch_strlist;
  char* arch_name="wildforce.arch";
//  char* arch_name="wildstar.arch";
  char* arch_path=new char[MAX_STR];
  strcpy(arch_path,getenv("PTOLEMY"));
//  strcpy(arch_path,"/common/projects/ptolemy/aptix/jmsmith/pt_r3");
  arch=new Arch(arch_path,arch_name);
  delete []arch_path;
}


int ACSCGFPGATarget::HWalg_query(CoreList* sg_list)
{
  // FIRST PASS:
  // Collect all stars into the smart generator list.
  // All fork stars are ignored.
  Galaxy& gal = *galaxy();
  Star* star;
  GalStarIter starIter(gal);
  char* name=new char[MAX_STR];
  while ((star=starIter++)!=NULL)
    {
      ACSCGFPGACore* fpga_core=fetch_fpgacore(star);
      strcpy(name,fpga_core->className());
      if (fpga_core!=NULL)
	if (strcmp(name,"ACSForkCGFPGA")!=0)
	  sg_list->append(fpga_core);
	else
	  {
	    // Forks should be initialized anyways AND
	    // they are considered to be in the hardware domain since
	    // they drive components which could be hardware
	    fpga_core->sg_initialize();
	    fpga_core->acs_domain=HW;
	  }
    }

  // SECOND PASS:
  // Assign unique identifiers to each smart generator
  // determine initial pipeline delays consumned, and
  // assign io to memory ports

  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);

      // FIX: This shouldn't be needed
      // Hunt-down all UNDEFINED smart generators and make preliminary 
      // assignments
      // For now just fill in some defaults
      if (fpga_core->acs_domain==UNDEFINED)
	fpga_core->acs_domain=HW;
      if (fpga_core->acs_existence==UNDEFINED)
	fpga_core->acs_existence=HARD;
      if (fpga_core->sg_language==UNDEFINED)
	fpga_core->sg_language=VHDL_BEHAVIORAL;

      // Apply unique identifier to stars
      HWstar_assignids(fpga_core);

      // Since these come from the Ptolemy graph, they are user generated
      fpga_core->acs_origin=USER_GENERATED;

      // If the user intentionally added a delay element then it needs to be 
      // classified as a save state device;
      // NOTE: Modify as other algorithmic save state cores are added to the user palette
      // FIX: Or this could be left to the core to evaluate its origin with new method:P
      if (strncmp(fpga_core->name(),"ACSDelay",8)==0)
	fpga_core->acs_state=SAVE_STATE;

      // Determine technology specifications given by user
      // Alloc smart generator memories
      // Query and set pipe states
      // Query memory assignment info
      int rc_sginit=fpga_core->sg_initialize(design_directory->retrieve_extended(),
					     &free_acsid);
      if (rc_sginit==0)
	return(0);

      if (fpga_core->unique_component)
	{
	  printf("fpga_core %s is unique\n",fpga_core->comment_name());
	  HWdup_component(fpga_core);
	}

      // Translate star parameters
      HWsg_query_params(fpga_core);
    }


  // THIRD PASS:
  // Also determine technology specifications (i.e., domain, technology, 
  // language).  Iterate over the smart generator ports to determine 
  // connectivity and output BW.
  // NOTE: The remoteport_id concept only works if MultiOut ports are
  //       indeed flattened into Output ports!
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      
      // ASSUMPTION:There should only be one(?) connected graph
      // Only connect relevant stars
      if (fpga_core->acs_domain==HW)
	{
	  int rc=HWsg_assign_pins(fpga_core);
	  if (rc==0)
	    return(0);
	}

    } //  for (int loop=0;loop<sg_list->size();loop++)


  // FOURTH PASS:
  // Remove all SW domain cores, since all HW connections have been established
  CoreList* tmp_gens=new CoreList;
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      if (fpga_core->acs_domain==HW)
	tmp_gens->append(fpga_core);
    }
//  delete sg_list;
  sg_list=tmp_gens;

  // Cleanup
  delete []name;

  // Return happy condition
  return(1);
}


/////////////////////////////////////////////////////////////////////////
// Since a unique component smart generator has been identified, we
// need to verify if there are any duplicates in the smart generator list
// and ensure that duplicate components won't be generated
/////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWdup_component(ACSCGFPGACore* dup_core)
{
  // It is unique until proven otherwise
  dup_core->declaration_flag=1;

  printf("core %s is unique and will be test\n",dup_core->comment_name());

  int sg_count=smart_generators->size();
  ACSCGFPGACore* cand_core=NULL;
  for (int sg_loop=0;sg_loop<sg_count;sg_loop++)
    {
      cand_core=(ACSCGFPGACore*) smart_generators->elem(sg_loop);
      if ((cand_core->unique_component) && (cand_core->declaration_flag))
	if ((strcmp(cand_core->unique_name.str(),dup_core->unique_name.str())==0)
	    && (cand_core->acs_id != dup_core->acs_id))
	  {
	    printf("core %s is not unique\n",dup_core->comment_name());
	    dup_core->declaration_flag=0;
	    break;
	  }
    }
}


//////////////////////////////////////////////////////////////////////////
// Set pin precisions based on either ACS precisions or Ptolemy precisions
// ASSUMPTION: Pin #s are implicitly linked to Ptolemy parameters.
//             input pins are first, starting at zero!
//////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWsg_query_params(ACSCGFPGACore* fpga_core)
{
  // Allocate local storage
  StringArray* input_list=new StringArray;
  StringArray* output_list=new StringArray;
  int pin_count=0;
  
  // Obtain parameter name info from each smart generator
  fpga_core->sg_param_query(input_list,output_list);

  // Retrieve precision from Ptolemy states and assign precisions
  HWsg_updateprec(fpga_core,input_list,pin_count,"InputPrecision","LockInput");
  pin_count+=(input_list->population())/2;
  HWsg_updateprec(fpga_core,output_list,pin_count,"OutputPrecision","LockOutput");

  // Retrieve user delay information
  fpga_core->sg_delay_query();

  // Update precisions, source precisions are locked, all designs can float
  if (fpga_core->acs_type==SOURCE)
    fpga_core->update_sg(LOCKED,UNLOCKED);
  else
    fpga_core->update_sg(UNLOCKED,UNLOCKED);

  // Cleanup
  delete input_list;
  delete output_list;
}


/////////////////////////////////////////////////////////////////////////////
// Query the ACS precision field and set the respective pin's precision
// if defaults, then retrieve precisions from fixed precision parameter field
/////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWsg_updateprec(ACSCGFPGACore* fpga_core,
				      StringArray* pin_list,
				      int pin_count,
				      const char* fixed_statename,
				      const char* lock_statename)
{
  Pin* pins=fpga_core->pins;
  char* lock_name=new char[MAX_STR];

  if (DEBUG_PTPRECISION)
    printf("Interpreting precisions for sg %s\n",fpga_core->comment_name());

  // ASSUMPTION:ACS precisions have two entries => (loop+=2)
  for (int loop=0;loop<pin_list->population();loop+=2)
    {
      // Obtain major bit
      char* mbit_field=pin_list->get(loop);
      int majorbit=fpga_core->intparam_query(mbit_field);

      // Obtain bit length
      char* bitlen_field=pin_list->get(loop+1);
      int bitlen=fpga_core->intparam_query(bitlen_field);

      if (DEBUG_PTPRECISION)
	printf("ACS Ptolemy precision fields are (%d,%d)\n",majorbit,bitlen);

      // Obtain user lock directive
      State* lock_state=fpga_core->stateWithName(lock_statename);
      strcpy(lock_name,lock_state->currentValue());
      if (DEBUG_PTPRECISION)
	printf("Lock state for field %s is %s\n",lock_statename,lock_name);
      int lock_status=0;
      if (strcmp(lock_name,"1")==0)
	lock_status=1;

      // FIX: FixSim core use of YES/NO implies 1/0 interpretation.
      if ((majorbit==0) && (bitlen==0))
	{
	  // ACS precisions not set, retrieve from Ptolemy precisions
	  State* precision_state=fpga_core->stateWithName(fixed_statename);
	  if (precision_state!=NULL)
	    {
	      StringList current_string=precision_state->currentValue();
	      const char* current_char=(const char *)current_string;
	  
	      Fix infix=Fix((const char*) current_char);
	      majorbit=infix.intb();
	      bitlen=infix.len()-majorbit;
	    }
	  else
	    {
	      // If there is no Ptolemy specified precision then zero it out
	      majorbit=0;
	      bitlen=0;
	    }
	}

      // Set precision
      if (lock_status)
	pins->set_wordlock(pin_count,LOCKED);
      else
	pins->set_wordlock(pin_count,UNLOCKED);
	
      if (DEBUG_PTPRECISION)
	printf("pin %s, precision set to (%d,%d)\n",
	       pins->query_pinname(pin_count),
	       majorbit,
	       bitlen);
	       
      if (lock_status)
	pins->set_precision(pin_count++,majorbit,bitlen,LOCKED);
      else
	pins->set_precision(pin_count++,majorbit,bitlen,UNLOCKED);
    }

  // Cleanup
  delete []lock_name;
}

////////////////////////////////////////////////////////////////////
// Identify ports on a star and assign to the portholes connectivity
// info.  This is only performed on user generated stars
// ASSUMPTION: Bit widths should already be propagated.
// FIX: This should be a two pass process.  Once for simple
//      connectivity, the other for BW information.
// FIX: Should trap for a mismatch between smart generator ports and
//      Ptolemy portholes
////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWsg_assign_pins(ACSCGFPGACore*& src_core)
{
  if (DEBUG_PTCONNECT)
    printf("HWstar_assign_ports invoked on smart generator %s\n",
	   src_core->comment_name());
  
  //
  // Determine port information and connectivity
  //
  BlockPortIter portIter(src_core->getCorona());
  ACSPortHole* phole;
  while ((phole=(ACSPortHole*)(portIter++)) != NULL)
    if (phole->isItOutput())
      {
	// Setup local star info
	Star* snk_star=(Star *) phole->far()->parent();
	
	if (DEBUG_PTCONNECT)
	  printf("phole %s is the driver, phole %s is the receiver\n",
		 phole->name(),
		 phole->far()->name());
	
	ACSCGFPGACore* snk_core=fetch_fpgacore(snk_star);
	if (snk_core != NULL)
	  if (snk_core->acs_domain==HW)
	    {
	      Pin* src_pins=src_core->pins;
	      Pin* snk_pins=snk_core->pins;
	      
	      int input_pin=UNASSIGNED;
	      if (DEBUG_PTCONNECT)
		printf("Searching for output pin\n");
	      int output_pin=src_pins->pin_withname(phole->name());
	      
	      if (snk_core->acs_type!=FORK) 
		{
		  if (src_core->acs_type!=FORK)
		    {
		      if (DEBUG_PTCONNECT)
			printf("Searching for input pin\n");
		      input_pin=snk_pins->pin_withname(phole->far()->name());
		      if (DEBUG_PTCONNECT)
			{
			  printf("sg %s, pin=%d, drives sg %s, pin=%d named=%s\n",
				 src_core->comment_name(),output_pin,
				 snk_core->comment_name(),input_pin,
				 phole->far()->name());
			}
		      
		      if (input_pin<0)
			{
			  sprintf(err_message,
				  "HWsg_assign_pins:Unable to find input pin on sg %s\n",
				  snk_core->comment_name());
			  Error::error(*this,err_message);
			  return(0);
			}
		      
		      
		      src_pins->connect_pin(output_pin,
					    snk_core->acs_id,
					    input_pin,
					    DATA_NODE);
		      snk_pins->connect_pin(input_pin,
					    src_core->acs_id,
					    output_pin,
					    DATA_NODE);
		      // Assign netlist info
		      HWassign_netlist(src_pins,output_pin,
				       snk_pins,input_pin);
		    }
		}
	      else  // Forks are bad, forward all information
		{
		  BlockPortIter forkIter(snk_core->getCorona());
		  ACSPortHole* fork_hole;
		  while ((fork_hole=(ACSPortHole*)(forkIter++)) != NULL)
		    if (fork_hole->isItOutput())
		      {
			// Setup local star info
			Star* forked_star=(Star *) fork_hole->far()->parent();
			ACSCGFPGACore* forked_core=fetch_fpgacore(forked_star);
			snk_pins=forked_core->pins;
			
			if (forked_core->acs_domain==HW)
			  {
			    if (DEBUG_PTCONNECT)
			      printf("Searching for input pin\n");
			    input_pin=snk_pins->pin_withname(fork_hole->far()->name());
//			    input_pin=snk_pins->free_pintype(INPUT_PIN);
			    
			    if (DEBUG_PTCONNECT)
			      printf("sg %s, pin=%d, drives sg %s, pin=%d named %s via fork\n",
				     src_core->comment_name(),output_pin,
				     forked_core->comment_name(),input_pin,
				     fork_hole->far()->name());
			    
			    src_pins->connect_pin(output_pin,
						  forked_core->acs_id,
						  input_pin,
						  DATA_NODE);
			    snk_pins->connect_pin(input_pin,
						  src_core->acs_id,
						  output_pin,
						  DATA_NODE);
			    
			    // Assign netlist info
			    HWassign_netlist(src_pins,output_pin,
					     snk_pins,input_pin);
			  }
		      }
		} // if (snk_core->acs_type==FORK) 
	      
	    } // 	if (snk_core != NULL)
	  else
	    printf("ACSCGFPGATarget::HWsg_assign_pins: Driven star %s is not an FPGACore! (domain=%d)\n",snk_core->comment_name(),snk_core->acs_domain);
      }

  // Return happy condition
  return(1);
}

void ACSCGFPGATarget::HWassign_netlist(Pin* src_pins,
				       int output_pin,
				       Pin* snk_pins,
				       int input_pin)
{
  /////////////////////////////
  // Assign netlist information
  /////////////////////////////
  int netlist_id=src_pins->query_netlistid(output_pin);
  if (netlist_id==UNASSIGNED)
    {
      int id=free_netlist_id;
      netlist_id=id;
      free_netlist_id++;
      src_pins->set_netlistid(output_pin,netlist_id);
    }
  snk_pins->set_netlistid(input_pin,netlist_id);
}







/////////////////////////////////////////////////////////
// This method assigns acs_ids to stars and port ids
// (as determined by an individual star) to the ports
// Should this be a CGStar method?
/////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWstar_assignids(ACSCGFPGACore*& fpga_core)
{
  // Assign a star id
  if ((fpga_core->acs_id==-1) && (fpga_core->acs_domain==HW))
    fpga_core->acs_id=free_acsid;

  return(free_acsid++);
}


/////////////////////////////////////////////////////
// Allocate cores to resources, if not user-specified
/////////////////////////////////////////////////////
void ACSCGFPGATarget::HWassign_devices(CoreList* sg_list)
{
  arch->unassign_all();

  printf("Assigning memories....");
  HWassign_memory(sg_list);
  printf("Done.\n");

  printf("Assigning FPGAs....");
  HWassign_fpga(sg_list);
  printf("Done.\n");

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after HWassign_devices");
}

/////////////////////////////////////////////////////////////////////////////
// Assign Source and Sink stars to memory ports based on specified addresses.
// Issue a firing schedule to Source stars on a first-come-first-serve basis.
// Allocate star pointers to each memory port for later access.
/////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWassign_memory(CoreList* sg_list)  
{
  ///////////////////////////////////////////////////////////////////
  // Assign memory as dictated by source addresses and port addresses
  // Set initial node activations on a first-come-first-served basis
  ///////////////////////////////////////////////////////////////////
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);

      if (DEBUG_ASSIGNMENTS)
	printf("checking core %s\n",fpga_core->name());

      // For now only schedule sources 
      if ((fpga_core->acs_type==SOURCE) || (fpga_core->acs_type==SINK) || (fpga_core->acs_type==SOURCE_LUT))
	{
	  if (DEBUG_ASSIGNMENTS)
	    printf("core %s is a %d\n",fpga_core->name(),fpga_core->acs_type);
	  int ok=arch->evaluate_memory_assignment(fpga_core->acs_device);
	  if (!ok)
	    fprintf(stderr,"HWassign_memory:Caution bad assignment for core %s, reassigned to %d\n",
		    fpga_core->comment_name(),
		    fpga_core->acs_device);
	  else
	    fpga_core->memory_device=fpga_core->acs_device;
		
	  //////////////////////////////////////
	  // Inform the memory port of this core
	  // FIX:Reassign to iocores member
	  //////////////////////////////////////
	  Port* mem_port=arch->get_memory_handle(fpga_core->memory_device);
	  if (fpga_core->acs_type==SOURCE)
	    mem_port->assign_srccore(fpga_core);
	  else if (fpga_core->acs_type==SINK)
	    mem_port->assign_snkcore(fpga_core);
	  else if (fpga_core->acs_type==SOURCE_LUT)
	    {
	      mem_port->assign_lutcore(fpga_core);
	      
	      // LUT has been trapped as a source, maintain revert to an algorithm star
//	      fpga_core->acs_type=BOTH;
	    }
	  else
	    fprintf(stderr,"ERROR:Unknown core type for %s when assigning to port\n",fpga_core->comment_name());

	  ///////////////////////////////////////////////////////////////////////////////////////
	  // If unirate, then word counts for all smart generators are derived from the global
	  // If wordlengths are set at the smart generator level, then this is the launch count
	  ///////////////////////////////////////////////////////////////////////////////////////
	  multirate=1;
	  mem_port->set_launchrate(vector_length);

	  ////////////////////////////////////////////////////
	  // Now determine through which fpga this is going
	  // FIX: For now, the memory device is the acs_device
	  //      this only works for the Annapolis Wildforce!
	  ////////////////////////////////////////////////////
	  fpga_core->acs_device=fpga_core->memory_device;

	  ////////////////////////////////////////////////////////////
	  // Since this memory will be used, its controlling FPGA will
	  // contain (of course) controlling circuitry.
	  ////////////////////////////////////////////////////////////
	  arch->activate_fpga(fpga_core->acs_device);
	  
	  ///////////////////////////////////////////////////////////////////
	  // To ease searching, assign the core handle to its controller FPGA
	  ///////////////////////////////////////////////////////////////////
	  Fpga* its_fpga=arch->get_fpga_handle(fpga_core->acs_device);
	  its_fpga->set_child(fpga_core);

	} // if ((fpga_core->acs_type==SOURCE) || (fpga_core->acs_type==SINK))
    } // for (int loop=0;loop<smart_generators->size();loop++)
}


/////////////////////////////////////////////////////////
// Watch for unassigned smart generators and default them
// FIX:A rudimentary algorithm should be added?
/////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWassign_fpga(CoreList* sg_list)
{
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      if (DEBUG_ASSIGNMENTS)
	printf("Core %s is assigned to acs_device %d\n",
	       fpga_core->comment_name(),
	       fpga_core->acs_device);
      
      if (fpga_core->acs_type==BOTH)
	{
	  int ok=arch->evaluate_fpga_assignment(fpga_core->acs_device);
	  if (!ok)
	    fprintf(stderr,"HWassign_fpga:Caution bad assignment for core %s, reassigned to %d\n",
		    fpga_core->comment_name(),
		    fpga_core->acs_device);
      

	  ////////////////////////////////////////////////////////////////
	  // To ease searching, assign the core handle to its respectively
	  // assigned FPGA
	  ////////////////////////////////////////////////////////////////
	  int local_device=fpga_core->acs_device;
	  Fpga* its_fpga=arch->get_fpga_handle(local_device);
	  its_fpga->set_child(fpga_core);


	  // Notify the core of technology type
	  fpga_core->target_type=its_fpga->part_type;
	}
    }
}



///////////////////////////////////////////////////////////////////
// Determine the major bit value parameter for a given ptolemy star
///////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::Ptquery_majorbit(const int io_type,
				      ACSCGFPGACore*& fpga_core)
{
  // Determine port size of smart generator
  State* bitm_state=NULL;
  if (io_type==SOURCE)
    bitm_state=fpga_core->stateWithName("Output_Major_Bit");
  if (io_type==SINK)
    bitm_state=fpga_core->stateWithName("Input_Major_Bit");
  
  int majorbit_val;
  if (bitm_state!=NULL)
    {
      IntState* bitm_intstate=(IntState *) bitm_state;
      majorbit_val=(int)(*bitm_intstate);
    }
  else
    {
      majorbit_val=0;
      
      if (io_type==SOURCE)
	printf("Ptquery_majorbit: Unable to find state:Output_Major_Bit\n");
      else // (io_type==SINK)
	printf("Ptquery_majorbit: Unable to find state:Input_Major_Bit\n");
    }
  
  if (DEBUG_PORTSIZE)
    printf("Ptquery_majorbit for %s is %d\n",
	   fpga_core->comment_name(),
	   majorbit_val);

  return(majorbit_val);
}

//////////////////////////////////////////////////////////////
// Determine the bit length parameter for a given ptolemy star
//////////////////////////////////////////////////////////////
int ACSCGFPGATarget::Ptquery_bitlen(const int io_type,
				    ACSCGFPGACore*& fpga_core)
{
  // Determine port size of smart generator
  State* bitlen_state=NULL;
  if (io_type==SOURCE)
    bitlen_state=fpga_core->stateWithName("Output_Bit_Length");
  if (io_type==SINK)
    bitlen_state=fpga_core->stateWithName("Input_Bit_Length");
  
  int bitlen;
  if (bitlen_state!=NULL)
    {
      IntState* bitlen_intstate=(IntState *) bitlen_state;
      bitlen=(int)(*bitlen_intstate);
    }
  else
    {
      bitlen=0;
      
      if (io_type==SOURCE)
	printf("Ptquery_bitlen: Unable to find state:Output_Bit_Len\n");
      else // (io_type==SINK)
	printf("Ptquery_bitlen: Unable to find state:Input_Bit_Len\n");
    }
  
  if (DEBUG_PORTSIZE)
    printf("Ptquery_bitlen for %s is %d\n",
	   fpga_core->comment_name(),
	   bitlen);

  return(bitlen);
}


///////////////////////////////////////////////////////////////////////////////
// This algorithm will trigger the architecture to examine its I/O utility and
// check for any opportunities to reduce total latency, by compressing multiple
// I/O channels into one via bit packing.  Then adds packing cores to support.
///////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWpack_ports(CoreList* sg_list)
{
  int ok=arch->assess_io();
  if (!ok)
    return(0);

  ACSIntArray* packed_list=NULL;

  // Check the memories
  packed_list=arch->compressed_io(MEMORY);
  if (packed_list!=NULL)
    {
      ACSIntArray* unneeded=HWpack_bits(packed_list,MEMORY,sg_list);
      for (int loop=0;loop<unneeded->population();loop++)
	sg_list->remove(unneeded->query(loop));

      delete unneeded;
    }

  // Check the systolics
  packed_list=arch->compressed_io(SYSTOLIC);
  if (packed_list!=NULL)
    {
//      ACSIntArray* unneeded=HWpack_bits(packed_list,MEMORY,sg_list);
      ACSIntArray* unneeded=HWpack_bits(packed_list,SYSTOLIC,sg_list);
      for (int loop=0;loop<unneeded->population();loop++)
	sg_list->remove(unneeded->query(loop));

      delete unneeded;
    }

  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////////////////
// Generates a single rate schedule for the algorithm given a fixed set of 
// input times.
// TODO: (1) Need to preclude constants or other non-timed functions from
//       affecting the scheduling.  Trap on size?
///////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWunirate_sched()
{
  ////////////////////////
  // Determine launch rate
  ////////////////////////
  seqlen=arch->calc_launchrate();
  if (DEBUG_SCHEDULER)
    printf("launch rate calculated to be %d\n",seqlen);

  ///////////////////////////////////////////////////////////////////////////////
  // Determine if addressing has been fixed or needs to be determined by stitcher
  ///////////////////////////////////////////////////////////////////////////////
  automatic_addressing=arch->query_addressing();
  if (automatic_wordlength)
    {
      vector_length=arch->mem_slice;
      if (DEBUG_SCHEDULER)
	printf("automatic wordlength calculation results in a maximum wordlength of %d\n",
	       vector_length);
    }

  ///////////////////////////////////////////////////
  // Recalculate pipe delay for phase_dependent cores
  // FIX:Revist this once multirate sequencer in place!!!
  //////////////////////////////////////////////////
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (fpga_core->phase_dependent)
	fpga_core->pipe_delay*=seqlen;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Determine if the algorithm has been scheduled already, if so then we are done.
  /////////////////////////////////////////////////////////////////////////////////
  if (HWall_scheduled())
    return;

  ////////////////////////////////////////////////////////////////////////////////////////////
  // Start by scheduling the sources
  // FIX:Should this be delegated to one of the subclasses?  The thought is to keep scheduling
  //     outside the domain of the architecture.
  ////////////////////////////////////////////////////////////////////////////////////////////
  for (int loop=0;loop<arch->mem_count;loop++)
    {
      Port* mem_port=arch->get_memory_handle(loop);
      if (mem_port->port_active())
	{
	  CoreList* source_list=mem_port->source_cores;
	  int free_time=0;
	  for (int core_loop=0;core_loop<source_list->size();core_loop++)
	    {
	      if (mem_port->sources_packed)
		mem_port->schedule_src(core_loop,0);
	      else
		mem_port->schedule_src(core_loop,free_time++);
	      ACSCGFPGACore* src_core=source_list->elem(core_loop);
	      src_core->act_launch=seqlen;
	      src_core->act_complete=(seqlen*vector_length)+src_core->act_output;
	    }
	}
    }


  ////////////////////////////////////////////////
  // Now iteratively schedule all downstream cores
  ////////////////////////////////////////////////
  int runaway=0;
  int done=0;
  while (!done)
    if (HWall_scheduled())
      {
	if (DEBUG_SCHEDULER)
	  printf("HWunirate_sched: Scheduling complete\n");
	done=1;
      }
    else
      {
	// Find an unscheduled node
	for (int loop=0;loop<sg_count;loop++)
	  {
	    ACSCGFPGACore* unsched_core=(ACSCGFPGACore*) smart_generators->elem(loop);
	    Pin* unsched_pins=unsched_core->pins;
	    Pipealign* unsched_pipe=unsched_core->pipe_alignment;
	    
	    if (DEBUG_SCHEDULER)
	      printf("Examining smart generator %s\n",unsched_core->comment_name());
	    
	    if (unsched_core->act_output==UNSCHEDULED)
	      {
		// Find source nodes
		ACSIntArray* unsched_inode_pins=unsched_pins->query_inodes();
		int snkpin_count=unsched_inode_pins->population();
		if (DEBUG_SCHEDULER)
		  printf("Star %s (with %d inputs) is unscheduled\n",
			 unsched_core->comment_name(),
			 snkpin_count);
		
		if (snkpin_count==0)
		  {
		    if (DEBUG_SCHEDULER)
		      printf("Non-io sg has no inputs, it will be NONSCHEDULED\n");
		    unsched_core->act_output=NONSCHEDULED;
		    break;
		  }
		
		int failed=0;
		for (int ipin_loop=0;ipin_loop<snkpin_count;ipin_loop++)
		  {
		    if (DEBUG_SCHEDULER)
		      printf("Testing input pin %d\n",ipin_loop);
		    int unsched_pin=unsched_inode_pins->query(ipin_loop);
		    Connectivity* icons=unsched_pins->query_connection(unsched_pin);
		    
		    // Sinks and Ioports may have been disconnected during the bit packing
		    // process.  These need to be removed from the scheduler's prying eye
		    // FIX: For now no bidirectionals drive this
		    // ASSUMPTION: Only one input node!!
		    if ((!icons->connected()) && 
			((unsched_core->acs_type==SINK) || (unsched_core->acs_type==IOPORT)))
		      {
			unsched_core->act_output=NONSCHEDULED;
			failed=1;
			unsched_pipe->start_over();
			break;  // ipin iterator
		      }
		    
		    int acs_id=icons->query_acsid(0);
		    ACSCGFPGACore* src_core=HWfind_star(acs_id);
		    if (src_core==NULL)
		      {
			printf("HWmono_sched: Unable to find source sg!!\n");
			failed=1;
			unsched_pipe->start_over();
			break;  // ipin iterator
		      }
		    if (src_core->act_output==UNSCHEDULED)
		      {
			if (DEBUG_SCHEDULER)
			  printf("HWmono_sched: Source sg %s is also unscheduled\n",
			       src_core->comment_name());
			failed=1;
			unsched_pipe->start_over();
			break;  // ipin iterator
		      }
		    
		    if (src_core->act_output >= 0)
		      unsched_pipe->add_src(unsched_pin,src_core->act_output,src_core->acs_id);
		    
		    if (DEBUG_SCHEDULER)
		      printf("Source star %s is scheduled at %d\n",
			     src_core->comment_name(),
			     src_core->act_output);
		  } 
		
		if (!failed)
		  {
		    // Determine amounts of delay needed, including zero
		    unsched_pipe->calc_netdelays();
		    
		    // Set the stars new activation time
		    if (unsched_core->pipe_delay < 0)
		      {
			sprintf(err_message,"Unirate_scheduler:Error, smart generator %s, bit size error.\n",unsched_core->comment_name());
			Error::error(*this,err_message);
			sprintf(err_message,"Hardware generated is therefore unreliable\n");
			Error::error(*this,err_message);
		      }
		    unsched_core->act_input=unsched_pipe->max_nodeact;
		    unsched_core->act_output=unsched_pipe->max_nodeact + unsched_core->pipe_delay;
		    unsched_core->act_launch=seqlen;
		    unsched_core->act_complete=unsched_core->act_input+(vector_length*seqlen);

		    if (unsched_core->delay_offset)
		      unsched_core->act_output+=unsched_core->delay_offset;
		    
		    if (unsched_core->acs_type==SINK)
		      {
			Port* mem_port=arch->get_memory_handle(unsched_core->acs_device);
			mem_port->schedule_snk(unsched_core,unsched_core->act_output);
			unsched_core->act_complete=unsched_core->act_input+(vector_length*seqlen);
		      }
		    if (DEBUG_SCHEDULER)
		      {
			printf("Sg %s now scheduled to fire at %d\n",
			       unsched_core->comment_name(),
			       unsched_core->act_output);
			printf("Src ready at %d, sg %s has a latency of %d\n",
			       unsched_pipe->max_nodeact,
			       unsched_core->comment_name(),
			       unsched_core->pipe_delay);
		      }
		  }
	      } // if (unsched_star->act_output==UNSCHEDULED)
	  } // for (int loop=0;loop<smart_generators->size();loop++)
	// DEBUG
	runaway++;
	if (runaway>RUNAWAY_SCHEDULER)
	  {
	    done=1;
	    if (DEBUG_SCHEDULER)
	      printf("HWunirate_sched:Runaway scheduler!\n");
	  }
      } // if (!HWall_scheduled())
}


/////////////////////////////////////////////////////////////////////////////////////////
// Memory-based smart generators need to have phase-dependent control signals associated
// with them in order to ensure that it\'s contents are valid and not corrupted with 
// constant clock cycle execution.  Execution is only warranted on the appropriate phase,
// identify this control signal for proper binding
/////////////////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWmultirate_sg(CoreList* sg_list)
{
  // For algorithmic smart generators, their effective delay
  // is based on the sequence length rather than the system clock.
  // Tag their CEs for sequencer phase hookups
  // ASSUMPTION: Algorithm delays imply a different rate.
  //             Shared memories are always used.
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* alg_core=(ACSCGFPGACore*) sg_list->elem(loop);
      if (DEBUG_SCHEDULER)
	{
	  printf("HWmultirate_sg:Examining smart generator %s\n",alg_core->comment_name());
	  Pipealign* core_pipe=alg_core->pipe_alignment;
	  core_pipe->print_delays();
	}
	    
      if (alg_core->phase_dependent)
//      if ((alg_core->alg_delay > 0) || (alg_core->phase_dependent))
	{
	  Pin* alg_pins=alg_core->pins;

	  // Test for any input control pins
	  ACSIntArray* input_controls=alg_pins->query_icnodes();
	  HWmultirate_controlpins(alg_core,input_controls);

	  // Test for any output control pins
	  ACSIntArray* output_controls=alg_pins->query_ocnodes();
	  HWmultirate_controlpins(alg_core,output_controls);

	}
    }
}


void ACSCGFPGATarget::HWmultirate_controlpins(ACSCGFPGACore* core,
					      ACSIntArray* control_pins)
{
  Pin* core_pins=core->pins;
  for (int loop=0;loop<control_pins->population();loop++)
    {
      int pin_no=control_pins->query(loop);
      int phase_dependency=core_pins->query_phasedependency(pin_no);
      if (phase_dependency!=UNASSIGNED)
	{
	  // Convert the pintypes for the resolver
	  core_pins->set_pintype(pin_no,INPUT_PIN_PHASE_START);
	  int phase=-1;
	  if (phase_dependency==INPUT_PIN)
	    phase=core->act_input;
	  else
	    phase=core->act_output;
	  core_pins->set_pinpriority(pin_no,phase);

	  if (DEBUG_SCHEDULER)
	    printf("Smart generator %s pin %d of type %d is phase-dependent on phase %d\n",
		   core->comment_name(),
		   pin_no,
		   core_pins->query_pintype(pin_no),
		   phase);
	}
    }
}


///////////////////////////////////////////////////////////////////////////////
// This method will add delays of delay_amt to the signals driven by the star 
// src_star before the reach the star with an acs_id of dest_id.
///////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWadd_delayto(ACSCGFPGACore* src_core,
				   ACSCGFPGACore* dest_core,
				   int src_pin,
				   int src_node,
				   int dest_pin,
				   int dest_node,
				   const int delay_amt)
{
  if (src_pin==UNKNOWN)
    src_pin=(src_core->pins)->match_acstype(dest_core->acs_id,OUTPUT_PIN);
  if (src_node==UNKNOWN)
    src_node=(src_core->pins)->match_acsnode(dest_core->acs_id,src_pin);
  if (dest_pin==UNKNOWN)
    dest_pin=(dest_core->pins)->match_acstype(src_core->acs_id,INPUT_PIN);
  if (dest_node==UNKNOWN)
    dest_node=(dest_core->pins)->match_acsnode(src_core->acs_id,dest_pin);
  
  Pin* src_pins=src_core->pins;
  int src_mbit=src_pins->query_majorbit(src_pin);
  int src_bitlen=src_pins->query_bitlen(src_pin);

  if (DEBUG_DELAYS)
    {
      printf("Adding delays to sg %s (before sg %s)\n",
	     src_core->comment_name(),
	     dest_core->comment_name());
      printf("Source pin=%d, source node=%d, dest pin=%d, dest node=%d\n",
	     src_pin,src_node,dest_pin,dest_node);
    }

	  
  // Is there already a delayed version of this star?
  if (src_core->acsdelay_count>0)
    {
      // Tap into existing delay
      if (DEBUG_DELAYS)
	printf("A delay of %d (max) already exists for this star\n",
	       src_core->acsdelay_count);
      if (delay_amt<=src_core->acsdelay_count)
	{
	  // Sufficient delays already exist tap into them
	  if (DEBUG_DELAYS)
	    printf("All the delays are available, tapping off of one\n");
	  
	  ACSIntArray* delayid_array=src_core->acsdelay_ids;
	  ACSCGFPGACore* delayed_core=HWfind_star(delayid_array->query(delay_amt-1));
	  if (DEBUG_DELAYS)
	    printf("Looking for link between src:%s and dest:%s\n",
		   src_core->comment_name(),
		   dest_core->comment_name());
	  
	  // Add new connection to delay
	  Pin* delayed_pins=delayed_core->pins;
	  int delayed_pin=delayed_pins->retrieve_type(OUTPUT_PIN);
	  delayed_pins->connect_pin(delayed_pin,
				    dest_core->acs_id,
				    dest_pin,
				    DATA_NODE);
	  
	  // Replace old connection on destination
	  Pin* dest_pins=dest_core->pins;
	  Connectivity* dest_con=dest_pins->query_connection(dest_pin);
	  dest_con->assign_node(dest_node,
				delayed_core->acs_id,
				delayed_pin,
				DATA_NODE);

	  // Remove old srcing node
	  Pin* src_pins=src_core->pins;
	  Connectivity* src_con=src_pins->query_connection(src_pin);
	  src_con->remove_node(src_node);
	   
	  if (DEBUG_DELAYS)
	    printf("delayed star %d is now connected to star %d\n",
		   delayed_core->acs_id,
		   dest_core->acs_id);
	}
      else
	{
	  ////////////////////////////////////////////////////////////
	  // Some delay already exists, but more needs to be tacked-on
	  ////////////////////////////////////////////////////////////
	  int add_delays=delay_amt-(src_core->acsdelay_count);
	  if (DEBUG_DELAYS)
	    printf("Adding %d more delays\n",add_delays);

	  // Find last delay in chain
	  int delay_index=(src_core->acsdelay_count)-1;
	  ACSIntArray* delayid_array=src_core->acsdelay_ids;
	  ACSCGFPGACore* delayed_core=HWfind_star(delayid_array->
						  query(delay_index));
	  if (DEBUG_DELAYS)
	    {
	      printf("Looking for link between src:%s\n",
		     src_core->comment_name());
	      printf("and dest:%s\n",dest_core->comment_name());
	    }

	  ACSIntArray* delay_ids=new ACSIntArray;
	  HWadd_delaychain(delayed_core,
			   UNASSIGNED,
			   UNASSIGNED,
			   dest_core,
			   dest_pin,
			   dest_node,
			   add_delays,
			   delay_ids,
			   src_mbit,
			   src_bitlen);

	  // Remove old srcing node
	  Pin* src_pins=src_core->pins;
	  Connectivity* src_con=src_pins->query_connection(src_pin);
	  src_con->remove_node(src_node);


	  // Update internal delay list (of the src_star)
	  ACSIntArray* delayid_array2=src_core->acsdelay_ids;
	  ACSIntArray* tmp_array=new ACSIntArray;
	  for (int loop=0;loop<delay_amt-1;loop++)
	    tmp_array->add(delayid_array2->query(loop));
	  delete src_core->acsdelay_ids;
	  src_core->acsdelay_ids=tmp_array;
	  for (int uloop=0;uloop<add_delays;uloop++)
	    delayid_array2->add(delay_ids->query(uloop));
	      
	  // Also increment free pointer
	  src_core->acsdelay_count+=delay_amt;
	}
    }
  else // if (src_core->acsdelay_count>0)
    {
      //////////////////////////////////////////
      // Generate new delayed versions of source
      //////////////////////////////////////////
      src_core->acsdelay_ids=new ACSIntArray;

      HWadd_delaychain(src_core,
		       src_pin,
		       src_node,
		       dest_core,
		       dest_pin,
		       dest_node,
		       delay_amt,
		       src_core->acsdelay_ids,
		       src_mbit,
		       src_bitlen);

      // Update internal delay list (of the src_star)
      // Also increment free pointer
      src_core->acsdelay_count+=delay_amt;

    } // if (src_core->acsdelay_count>0)
   
  if (dest_core != NULL)
    return(0);
  else
    {
      // Return happy condition
      return(1);
    }
}


int ACSCGFPGATarget::HWadd_delaychain(ACSCGFPGACore* src_core,
				      int src_pin,
				      int src_node, 
				      ACSCGFPGACore* dest_core,
				      int dest_pin,
				      int dest_node,
				      int add_delays,
				      ACSIntArray* delay_ids,
				      int major_bit,
				      int bit_length)
{
  // Resolve where the delays should reside all algorithmic smart generators
  // will base of the src_core which will be the most frequent option
  // Howver, IOPorts have two sides to them with the driving side being
  // allocated to the 2ndary acs device
  int target_device=src_core->acs_device;
  if (src_core->acs_type==IOPORT)
    {
      ACSIntArray* device_nums=src_core->sg_get_privaledge();
      int acs_outdevice=device_nums->query(1);
      target_device=acs_outdevice;
      if (DEBUG_DELAYS)
	printf("Adding delays to an IoPort, will target for device %d\n",
	       target_device);
    }

  construct->set_targetdevice(arch->get_fpga_handle(target_device));

  ACSCGFPGACore* prev_core=NULL;
  int prev_opin=UNASSIGNED;
//  int sign_convention=src_core->sign_convention;
  for (int gen_delay=1;gen_delay<=add_delays;gen_delay++)
    {
      if (DEBUG_DELAYS)
	{
	  printf("Generating delay #%d for star %s\n",gen_delay,
		 src_core->comment_name());
	  printf("tapping off pin %d, node %d\n",src_pin,src_node);
	}
      
      // Update internal delay list
      ACSCGFPGACore* delay_core=NULL;
      delay_core=construct->add_delay(1,smart_generators);
      delay_ids->add(delay_core->acs_id);
      delay_core->act_output=src_core->act_output+gen_delay;
      delay_core->add_comment((const char*) src_core->comment_name(),
			      "Pipealignment",
			      gen_delay);
      
      // Get connection information to this delay instance
      int delay_ipin=(delay_core->pins)->free_pintype(INPUT_PIN);
      int delay_opin=(delay_core->pins)->free_pintype(OUTPUT_PIN);
      
      // Set new BW
      (delay_core->pins)->set_precision(delay_ipin,
					major_bit,
					bit_length,
					UNLOCKED);
      (delay_core->pins)->set_precision(delay_opin,
					major_bit,
					bit_length,
					UNLOCKED);
      
      if (gen_delay==1)
	{
	  // Connect the originating smart generator
	  construct->connect_sg(src_core,src_pin,src_node,
				delay_core,delay_ipin,UNASSIGNED,
				DATA_NODE);
	  
	  
	  // Setup for next pass which could be next if statement!
	  prev_core=delay_core;
	}
      if (gen_delay==add_delays)
	{
	  // Make connection on delay instance
	  construct->connect_sg(delay_core,delay_opin,UNASSIGNED,
				dest_core,dest_pin,dest_node,
				DATA_NODE);
	}
      if (gen_delay != 1)
	{
	  // Connect intermediary delay smart generators
	  construct->connect_sg(prev_core,prev_opin,UNASSIGNED,
				delay_core,delay_ipin,UNASSIGNED,
				DATA_NODE);
	}
      prev_core=delay_core;
      prev_opin=delay_opin;
    }

  // Cleanup
//  delete construct;

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////
// Test to see if all nodes have been scheduled.
// This is the exit criteria for schedulers
////////////////////////////////////////////////
int ACSCGFPGATarget::HWall_scheduled(void)
{
  // Loop over all the smart generators
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (fpga_core->act_output==UNSCHEDULED)
	return(0);
    }

  // All nodes have been scheduled
  return(1);
}

////////////////////////////////////////////////////////////////////
// Examine each memory port, shuffle the sinks in case of collisions
////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_sched()
{
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      Port* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->port_active()) 
	{
	  if (DEBUG_IOSCHEDULER)
	    printf("HWio_sched::Memory %d is being used as a source/sink\n",
		   mem_loop);

	  /////////////////////////////////////
	  // Determine source and sink pointers
	  /////////////////////////////////////
	  CoreList* source_stars=mem_port->source_cores;
	  CoreList* sink_stars=mem_port->sink_cores;
	  int src_count=source_stars->size();
	  int snk_count=sink_stars->size();
	  
	  if (snk_count > 0)
	    {
	      ///////////////////////////////////////////////////
	      // Reschedule sinks should there be any read delays
	      ///////////////////////////////////////////////////
	      ACSIntArray* read_block=new ACSIntArray(seqlen,FALSE);
	      for (int source_loop=0;source_loop<src_count;source_loop++)
		{
		  ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) source_stars->elem(source_loop);
		  int addr_time=fpga_core->act_input;
		  read_block->set(addr_time,TRUE);
		  if (mem_port->read_skew != 0)
		    read_block->set(addr_time+mem_port->read_skew,TRUE);
		  if (DEBUG_IOSCHEDULER)
		    printf("Reads (%d of %d) block times %d and %d\n",
			   source_loop,src_count,
			   addr_time,
			   addr_time+mem_port->read_skew);
		}
	      
	      if (DEBUG_IOSCHEDULER)
		read_block->print("read_block contents after source formulation");

	      // FIX: Assuming that (for writes) address and data are in sync
	      for (int sink_loop=0;sink_loop<snk_count;sink_loop++)
		{
		  ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) sink_stars->elem(sink_loop);
		  int addr_time=fpga_core->act_input;
		  int seq_time=addr_time;
		  while (seq_time >= seqlen)
		    seq_time-=seqlen;
		  
		  if (read_block->query(seq_time)==FALSE)
		    {
		      read_block->set(seq_time,TRUE);
		      if (DEBUG_IOSCHEDULER)
			printf("Write (%d of %d) is not blocked at time %d\n",
			       sink_loop+1,snk_count,
			       seq_time);
		    }
		  else
		    {
		      if (DEBUG_IOSCHEDULER)
			printf("Write (%d of %d) at time %d is currently blocked\n",
			       sink_loop+1,snk_count,
			       seq_time);
		      int slot_found=0;
		      int offset=1;
		      while (!slot_found)
			{
			  seq_time++;
			  if (seq_time==seqlen)
			    seq_time=0;
			  if (read_block->query(seq_time)==FALSE)
			    {
			      mem_port->schedule_snk(sink_loop,addr_time+offset);
			      Pipealign* sink_pipe=fpga_core->pipe_alignment;
			      sink_pipe->calc_netdelays(fpga_core->act_output);
			      fpga_core->act_complete=fpga_core->act_input+(vector_length*seqlen);
			    
			      if (DEBUG_IOSCHEDULER)
				printf("Adjusting sink core firing time to %d\n",fpga_core->act_output);
			      read_block->set(seq_time,TRUE);
			      slot_found=1;
			    }
			  else
			    offset++;
			}
		    }
		} // if (snk_count > 0)
	      
	      // Cleanup
	      delete read_block;


	      /////////////////////////////////////////////////////
	      // Reschedule sources should there be any read delays
	      /////////////////////////////////////////////////////
	      if (src_count>0)
		if (mem_port->write_skew != 0)
		  {
		    printf("HWio_sched:Currently unable to schedule memories that\n");
		    printf("           have write delays\n");
		  }
	    }
	  
	  //////////////////////////////
	  // Add port timing information
	  //////////////////////////////
	  // REMOVE:This code was useful for the release 1 unirate sequencer
	  mem_port->init_pt(seqlen);
/*
	  for (int src_loop=0;src_loop<src_count;src_loop++)
	    {
	      ACSCGFPGACore* src_sg=(ACSCGFPGACore*) source_stars->elem(src_loop);
	      int addr_act=src_sg->act_input;

	      // Was if, converted to while (of hand)
	      while (addr_act >= seqlen)
		addr_act-=seqlen;
	      mem_port->add_pt(addr_act,SOURCE);
	      if (DEBUG_IOSCHEDULER)
		printf("Source address set to activate at %d in sequence\n",addr_act);
	    }
	  for (int snk_loop=0;snk_loop<snk_count;snk_loop++)
	    {
	      ACSCGFPGACore* snk_sg=(ACSCGFPGACore*) sink_stars->elem(snk_loop);
	      int addr_act=snk_sg->act_input;
	      while (addr_act >= seqlen)
		addr_act-=seqlen;
	      mem_port->add_pt(addr_act,SINK);
	      if (DEBUG_IOSCHEDULER)
		printf("Sink address set to activate at %d in sequence\n",addr_act);
	    }
	    */
	} // if ((mem_port->port_active()) && ...
    } //  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
}




int ACSCGFPGATarget::HWsynth_pipealigns(void)
{
  if (DEBUG_PIPESYNTH)
    printf("\n\nHWsynth_pipealigns engaged\n");
  
  // Now that the smart generators have been rescheduled, then have each examine
  // their effective pipe alignment requirements
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* unsched_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (!unsched_core->phase_dependent)
	{
	  Pin* unsched_pins=unsched_core->pins;
	  Pipealign* unsched_pipe=unsched_core->pipe_alignment;
	  ACSIntArray* unsched_inode_pins=unsched_pins->query_inodes();
	  int snkpin_count=unsched_inode_pins->population();
	  
	  for (int ipin_loop=0;ipin_loop<snkpin_count;ipin_loop++)
	    {
	      int unsched_pin=unsched_inode_pins->query(ipin_loop);
	      Connectivity* icons=unsched_pins->query_connection(unsched_pin);
	      int src_acsid=icons->query_acsid(0);
	      ACSCGFPGACore* src_core=HWfind_star(src_acsid);

	      // FIX: Scheduling done by wordlength analysis currently will 
	      // schedule constants to fire at time zero, irregardless of when
	      // the data is needed.  Hence pipedelays may be unnecessarily 
	      // generated.  Avoid this by rescheduling the constant core here.
	      
              // Crude trap for constant star
	      if (src_core->sg_constants != NULL)
		unsched_pipe->add_src(unsched_pin,unsched_core->act_input,
				      src_core->acs_id);	
	      else if (src_core->act_output >= 0)
		unsched_pipe->add_src(unsched_pin,src_core->act_output,src_core->acs_id);	
	      if (DEBUG_PIPESYNTH)
		printf("core %s, input time %d, is driven by core %s with an output time of %d\n",
		       unsched_core->comment_name(),
		       unsched_core->act_input,
		       src_core->comment_name(),
		       src_core->act_output);
	    }
	  unsched_pipe->calc_netdelays(unsched_core->act_input);
	}
    }

  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* src_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      Pin* src_pins=src_core->pins;
      ACSIntArray* opins=src_pins->query_onodes();
      if (DEBUG_PIPESYNTH)
	{
	  printf("\nresolving delays for sg %s with %d opins\n",
		 src_core->comment_name(),
		 opins->population());
	  src_pins->print_pinclasses();
	}
      
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int opin=opins->query(opin_loop);
	  Connectivity* onodes=src_pins->query_connection(opin);
	  Delay* srcpin_delays=src_pins->query_delay(opin);
	  for (int onode_loop=0;onode_loop<onodes->node_count;onode_loop++)
	    {
	      if (onodes->query_pintype(onode_loop)==DATA_NODE)
		{
		  int acs_id=onodes->query_acsid(onode_loop);
		  ACSCGFPGACore* dest_core=HWfind_star(acs_id);
		  Pipealign* snk_pipe=dest_core->pipe_alignment;
		  int snk_pin=onodes->query_pinid(onode_loop);
		  if (DEBUG_PIPESYNTH)
		    printf("opin %d, onode %d, connects to sg %s on pin %d\n",
			   opin,
			   onode_loop,
			   dest_core->comment_name(),
			   snk_pin);
		  if (snk_pipe->delays_needed)
		    {
		      int delay_id=snk_pipe->find_pin(snk_pin);
		      int delay_req=(snk_pipe->new_delays)->query(delay_id);
		      if (delay_req > 0)
			srcpin_delays->add_dest(acs_id,snk_pin,delay_req);
		      if (DEBUG_PIPESYNTH)
			printf("this connection requires %d delays\n",
			       delay_req);
		    }
		  else
		    if (DEBUG_PIPESYNTH)
		      printf("this connection requires no delays\n");
		}
	    }
	}
      
    } // LOOP: smart_generators

  if (DEBUG_PIPESYNTH)
    printf("\n\nNow checking for adding delays\n");
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* src_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      Pin* src_pins=src_core->pins;
      ACSIntArray* opins=src_pins->query_onodes();

      if (DEBUG_PIPESYNTH)
	printf("Evaluating source %s (acsid=%d) for adding delays\n",
	       src_core->comment_name(),
	       src_core->acs_id);
      
      // Add delays if necessary
      // FIX: Delegate more to the Delay class:P
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int opin=opins->query(opin_loop);
	  Delay* srcpin_delays=src_pins->query_delay(opin);
	  if (srcpin_delays->delays_needed)
	    {
	      if (DEBUG_PIPESYNTH)
		printf("Adding necessary delays:\n");

	      ACSIntArray* src_dest_ids=srcpin_delays->dest_ids;
	      int delay_count=src_dest_ids->population();
	      srcpin_delays->sort_delays();

	      // Incremental delay info
	      int delays_added=0;
	      int src_pin=opin;
	      ACSCGFPGACore* sourcing_core=src_core;

	      for (int dloop=0;dloop<delay_count;dloop++)
		{
		  int dest_id=src_dest_ids->query(dloop);
		  ACSCGFPGACore* dest_core=HWfind_star(dest_id);
		  
                  // Break original links
		  src_pins->disconnect_all_pinsto(dest_id);

		  ACSIntArray* srcpin_delay_reqs=srcpin_delays->delay_requirements;
		  ACSIntArray* srcpin_delay_pins=srcpin_delays->dest_pins;
		  int delay_amt=srcpin_delay_reqs->query(dloop);
		  if (DEBUG_PIPESYNTH)
		    printf("%d delays to %s (acsid=%d) on pin %d\n",
			   delay_amt,
			   dest_core->comment_name(),
			   dest_core->acs_id,
			   srcpin_delay_pins->query(dloop));

		  if (src_core->acs_type==BOTH)
		    construct->set_targetdevice(arch->get_fpga_handle(src_core->acs_device));
		  else
		    construct->set_targetdevice(arch->get_fpga_handle(dest_core->acs_device));

		  int net_delay=delay_amt-delays_added;

		  if (net_delay > 0)
		    {
		      if (DEBUG_PIPESYNTH)
			printf("Adding delay of %d between %s and %s\n",
			       net_delay,
			       sourcing_core->comment_name(),
			       dest_core->comment_name());

		      ACSCGFPGACore* delay_core=construct->add_delay(net_delay,
								     smart_generators);

		      construct->connect_sg(sourcing_core,src_pin,UNASSIGNED,
					    delay_core,UNASSIGNED,UNASSIGNED,DATA_NODE);
		      construct->connect_sg(delay_core,UNASSIGNED,UNASSIGNED,
					    dest_core,srcpin_delay_pins->query(dloop),UNASSIGNED,DATA_NODE);

/*
		      construct->insert_sg(sourcing_core,
					   delay_core,
					   dest_core,
					   UNASSIGNED,
					   UNASSIGNED,
					   DATA_NODE,
					   srcpin_delay_pins->query(dloop),
					   UNASSIGNED,
					   DATA_NODE);
					   */
		      sourcing_core=delay_core;
		      src_pin=UNASSIGNED;
		      delays_added=delay_amt;
		    }
		  else
		    {
		      construct->connect_sg(sourcing_core,src_pin,UNASSIGNED,
					    dest_core,srcpin_delay_pins->query(dloop),UNASSIGNED,DATA_NODE);

		      if (DEBUG_PIPESYNTH)
			printf("Sufficient delay exists, connecting %s to %s\n",
			       sourcing_core->comment_name(),
			       dest_core->comment_name());
		    }
/*
		  HWadd_delayto(src_core,
				dest_sg,
				opin,
				UNKNOWN,
				srcpin_delay_pins->query(dloop),
				UNKNOWN,
				srcpin_delay_reqs->query(dloop));
				*/
		}
	    }
	}
    }

  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////////////////////
// Generate a sequencer to control the algorithm for this architecture
//////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWsequencer(void)
{
  //////////////////////////////////////////////////////
  // Generate the multirate sequencer for each used fpga
  //////////////////////////////////////////////////////
  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
      if (fpga_elem->usage_state==FPGA_USED)
	HWmultirate_sequencer(fpga_elem,fpga_no);
    }

  ////////////////////////////////////////////////////
  // Construct architecture-specific kickoff sequencer
  ////////////////////////////////////////////////////
  arch->generate_sequencer();
}

///////////////////////////////////////////////
// Generate the generalized multirate sequencer
///////////////////////////////////////////////
void ACSCGFPGATarget::HWmultirate_sequencer(Fpga* fpga_elem, const int fpga_no)
{  
  ////////////////////////////
  // Generate the master timer
  ////////////////////////////
  ACSIntArray* master_timing=new ACSIntArray;
  ACSIntArray* master_rate=new ACSIntArray;
  ACSIntArray* master_totalacts=new ACSIntArray;

  // First determine the needed timing signals, then make them dependent
  // FIX: Scheduling similarites could be identified in order to reduce 
  //      the complexity of the sequencer here
  // NOTE: IOPORTs times are split between fpgas, only one time is relevant.  Unneeded hw will be
  //       instantiated here, but trimmed during synthesis.
  CoreList* sg_list=fpga_elem->get_childcores_handle();
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      if (fpga_core->acs_type != BOTH)
	{
	  if (DEBUG_SEQUENCER)
	    printf("Examining core %s for the master timer in=%d, out=%d, complete=%d, launch=%d, totalacts=%d\n",
		   fpga_core->comment_name(),
		   fpga_core->act_input,
		   fpga_core->act_output,
		   fpga_core->act_complete,
		   fpga_core->act_launch,
		   fpga_core->word_count);
//		 (fpga_core->act_complete-fpga_core->act_input)/fpga_core->act_launch);
	  
	  // Trap for scheduling an input
	  if (fpga_core->act_input != UNASSIGNED)
	    {
	      // FIX:Uniqueness must span ALL three categories!
	      // FIX:Shouldnt need to trap for Source LUT, need to improve ctrl signal traps
	      //     past the input and output only stages!
	      int unique=0;
	      if (fpga_core->acs_type == SOURCE_LUT)
		unique=master_timing->add_unique(fpga_core->act_input+2);
	      else
		unique=master_timing->add_unique(fpga_core->act_input);

	      if (unique)
		{
		  master_rate->add(fpga_core->act_launch);
		  master_totalacts->add(fpga_core->word_count);

/*
		  if (fpga_core->acs_type == SOURCE)
		    master_totalacts->add((fpga_core->act_complete-fpga_core->act_input)
					  / fpga_core->act_launch);
		  else if (fpga_core->acs_type == SINK)
		    master_totalacts->add(((fpga_core->act_complete-fpga_core->act_input)+1)
					  / fpga_core->act_launch);
		  else
		    master_totalacts->add((fpga_core->act_complete-fpga_core->act_output)
					  / fpga_core->act_launch);
					  */
		}
	    }
	  
	  // Trap for scheduling a unique output
	  if ((fpga_core->act_input) != (fpga_core->act_output))
	    {
	      // FIX:Uniqueness must span ALL three categories!
	      int unique=master_timing->add_unique(fpga_core->act_output);
	      if (unique)
		{
		  master_rate->add(fpga_core->act_launch);
		  if ((fpga_core->acs_type == SOURCE) || (fpga_core->acs_type == SINK))
		    master_totalacts->add((fpga_core->act_complete-fpga_core->act_input)
					  / fpga_core->act_launch);
		  else
		    master_totalacts->add((fpga_core->act_complete-fpga_core->act_output)
					  / fpga_core->act_launch);
		}
	    }
	}
      if ((fpga_core->act_output != UNASSIGNED) && (fpga_core->acs_state == SAVE_STATE))
	{
	  // FIX?:Is this truely asking for a oneshot activation or multiple?
	  // FIX:Uniqueness must span ALL three categories!
	  Pin* fpga_pins=fpga_core->pins;
	  for (int pin_loop=0;pin_loop<fpga_pins->query_pincount();pin_loop++)
	    {
	      int depend_assignment=fpga_pins->query_phasedependency(pin_loop);
	      if (depend_assignment != UNASSIGNED)
		{
		  int phase=UNASSIGNED;
		  if (depend_assignment==INPUT_PIN)
		    phase=fpga_core->act_input;
		  else
		    phase=fpga_core->act_output;
		  
		  int unique=master_timing->add_unique(phase);
		  if (unique)
		    {
		      master_rate->add(fpga_core->act_launch);
		      master_totalacts->add((fpga_core->act_complete-fpga_core->act_output)/fpga_core->act_launch);
		    }
		}
	    }
	}
    }

  ACSIntArray* master_order=master_timing->sort_lh();
  master_timing->reorder(master_order);
  master_rate->reorder(master_order);
  master_totalacts->reorder(master_order);
  delete master_order;
  
  if (DEBUG_SEQUENCER)
    {
      master_timing->print("scheduled times for master timer");
      master_rate->print("scheduled rates for master timer");
      master_totalacts->print("scheduled word counts for master timer");
    }

  // Test if sequencer isnt needed for this fpga
  if (master_timing->population()==0)
    {
      delete master_timing;
      delete master_rate;
      delete master_totalacts;
      return;
    }

  if (DEBUG_SEQUENCER)
    printf("Building multirate sequencer for fpga %d\n",fpga_no);

  construct->set_targetdevice(fpga_elem);
  ACSCGFPGACore* master_timer=construct->add_timer(master_timing,smart_generators);
  master_timer->alter_pintype(INPUT_PIN_START,INPUT_PIN_ADDRCLR);
  master_timer->alter_pintype(INPUT_PIN_EXTCTRL,INPUT_PIN_START);
  master_timer->alter_pintype(INPUT_PIN_RESET,INPUT_PIN_SEQRESET);

  // Force implementation to use a delay-based counter
  master_timer->target_implementation=1;


  ////////////////////////////////////////////////////
  // Notify fpga of the counter to indicate completion
  ////////////////////////////////////////////////////
  int completion_time=master_timing->tail();
  fpga_elem->set_completiontime(completion_time);

  //////////////////
  // Add the phasers
  //////////////////
  char* comment=new char[MAX_STR];
  for (int loop=0;loop<master_rate->population();loop++)
    {
      int phase_count=master_rate->query(loop);
      ACSCGFPGACore* phase_core=construct->add_phaser(phase_count,smart_generators);
      sprintf(comment,"Phaser for time %d",master_timing->query(loop));
      phase_core->add_comment(comment);

      // Annotate phase information for the I/O pins
      int start_time=master_timing->query(loop);
      int phasein_pin=phase_core->find_hardpin(INPUT_PIN_EXTCTRL);
      phase_core->set_pinpriority(phasein_pin,start_time);
      phase_core->replace_pintype(phasein_pin,INPUT_PIN_PHASE_START);
      if (DEBUG_SEQUENCER)
	printf("set phase_core pin %d, priority=%d\n",phasein_pin,start_time);
      int phaseout_pin=phase_core->find_hardpin(OUTPUT_PIN_PHASE);
      phase_core->set_pinpriority(phaseout_pin,start_time);
      int prephaseout_pin=phase_core->find_hardpin(OUTPUT_PIN_PREPHASE);
      phase_core->set_pinpriority(prephaseout_pin,start_time);
      int pstop_pin=phase_core->find_hardpin(INPUT_PIN_STOP);
      phase_core->set_pinpriority(pstop_pin,start_time); 

      int master_pin=master_timer->find_hardpin(OUTPUT_PIN,start_time);
      master_timer->replace_pintype(master_pin,OUTPUT_PIN_PHASE_START);

      // Add the word counters
      int count_duration=master_totalacts->query(loop);
//      int count_duration=master_totalacts->query(loop)+1;
      if (DEBUG_SEQUENCER)
	printf("count_duration=%d\n",count_duration);
      if (count_duration > 0)
	{
	  ACSCGFPGACore* counter_core=construct->add_timer(count_duration,smart_generators);

	  // Connect the phaser to the word counter and visa-versa
	  int counter_pin=counter_core->find_hardpin(INPUT_PIN_EXTCTRL);
	  counter_core->set_pinpriority(counter_pin,start_time);
	  counter_core->replace_pintype(counter_pin,INPUT_PIN_PHASE_START);
	  int countout_pin=counter_core->find_hardpin(OUTPUT_PIN);
	  counter_core->set_pinpriority(countout_pin,start_time);
	  counter_core->replace_pintype(countout_pin,OUTPUT_PIN_STOP);
	  int countclr_pin=counter_core->find_hardpin(INPUT_PIN_RESET);
	  counter_core->replace_pintype(countclr_pin,INPUT_PIN_SEQRESET);
	  
	  // Add word counter CE controller
	  // ASSUMPTION:OR pin numbers wont change
	  ACSCGFPGACore* orce_core=construct->add_sg("ACS","OR","CGFPGA",
						     BOTH,UNSIGNED,smart_generators);
	  construct->connect_sg(orce_core,
				counter_core,INPUT_PIN_CE);
	  Pin* orce_pins=orce_core->pins;
	  
	  orce_pins->set_pintype(0,INPUT_PIN_PHASE);
	  orce_pins->set_pinpriority(0,start_time);
	  orce_pins->set_precision(0,0,1,LOCKED);
	  orce_pins->set_pintype(1,INPUT_PIN_PHASE_START);
	  orce_pins->set_pinpriority(1,start_time);
	  orce_pins->set_precision(1,0,1,LOCKED);
	  orce_pins->set_precision(2,0,1,LOCKED);
	}
    }

  // Cleanup
  delete []comment;
  delete master_timing;
  delete master_rate;
  delete master_totalacts;
}



////////////////////////////////////////////////////////
// Determine bitwidth mismatches and correct with buffers
////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWbw_resolver()
{
  if (DEBUG_BWRESOLVER)
    printf("\n\nBitwidth Resolver\n");

  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
      Pin* data_signals=fpga_elem->data_signals;
      Pin* ext_signals=fpga_elem->ext_signals;
      Pin* constant_signals=fpga_elem->constant_signals;
      Pin* ctrl_signals=fpga_elem->ctrl_signals;
      
      // Evaluate external driving signals
      // FIX: Why only INPUT_PIN_MEMORY???
      for (int loop=0;loop<ext_signals->query_pincount();loop++)
	{
	  int type=ext_signals->pin_classtype(loop);
//	  printf("HWlink_resolver to examine external signal %s\n",ext_signals->query_pinname(loop));
	  if (type==INPUT_PIN)
	    {
	      Connectivity* output_nodes=ext_signals->query_connection(loop);
	      for (int onode_loop=0;onode_loop<output_nodes->node_count;onode_loop++)
		HWlink_resolver(data_signals,ext_signals,
				constant_signals,ctrl_signals,
				NULL,ext_signals,
				loop,output_nodes,onode_loop);
	    }
	}
    }
      
  // Evaluate smart generator driving links
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      Pin* fpga_pins=fpga_core->pins;
      
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_core->acs_device);
      Pin* data_signals=fpga_elem->data_signals;
      Pin* ext_signals=fpga_elem->ext_signals;
      Pin* constant_signals=fpga_elem->constant_signals;
      Pin* ctrl_signals=fpga_elem->ctrl_signals;

      if (DEBUG_BWRESOLVER)
	{
	  printf("associated fpga is %d\n",fpga_core->acs_device);
	  fpga_elem->print_fpga();
	}

      ACSIntArray* opins=fpga_pins->query_onodes();
      if (DEBUG_BWRESOLVER)
	{
	  printf("resolving bw links for star %s with %d opins\n",
		 fpga_core->comment_name(),
		 opins->population());
	  fpga_pins->print_pins("bw resolving these candidate pins");
	}
      
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int output_pin=(int) opins->query(opin_loop);
	  Connectivity* output_nodes=fpga_pins->query_connection(output_pin);
	  
	  if (DEBUG_BWRESOLVER)
	    printf("pin %d has %d nodes\n",opin_loop,output_nodes->node_count);
	  for (int onode_loop=0;onode_loop<output_nodes->node_count;onode_loop++)
	    HWlink_resolver(data_signals,ext_signals,
			    constant_signals,ctrl_signals,
			    fpga_core,fpga_pins,
			    output_pin,output_nodes,onode_loop);
	}
    }
}


void ACSCGFPGATarget::HWlink_resolver(Pin* data_signals,Pin* ext_signals,
				      Pin* constant_signals,Pin* ctrl_signals,
				      ACSCGFPGACore* output_core,
				      Pin* output_pins,
				      int output_pin,
				      Connectivity* output_nodes,
				      int output_node)
{
  ACSCGFPGACore* dest_core=NULL;
  Pin* dest_pins=NULL;

  // For smart generators find out the class of destination
  if (output_core != NULL)
    {
      Fpga* fpga_elem=arch->get_fpga_handle(output_core->acs_device);
      construct->set_targetdevice(fpga_elem);

      switch (output_nodes->query_pintype(output_node))
	{
	case DATA_NODE:
	  dest_core=HWfind_star(output_nodes->query_acsid(output_node));
	  dest_pins=dest_core->pins;
	  break;
	case EXT_NODE:
	  dest_pins=ext_signals;
	  break;
	case CONSTANT_NODE:
	  dest_pins=constant_signals;
	  break;
	case CTRL_NODE:
	  dest_core=HWfind_star(output_nodes->query_acsid(output_node));
	  if (dest_core==NULL)
	    dest_pins=ctrl_signals;
	  else
	    dest_pins=dest_core->pins;
	  break;
	default:
	  dest_pins=NULL;
	}
    }
  else
    {
      // ASSUMPTION: This should be relevant only for bw resolving
      // This is connected to a smart generator
      dest_core=HWfind_star(output_nodes->query_acsid(output_node));
      dest_pins=dest_core->pins;

      Fpga* dest_fpga=arch->get_fpga_handle(dest_core->acs_device);
      construct->set_targetdevice(dest_fpga);
    }

  int input_pin=output_nodes->query_pinid(output_node);

  // Since there can only be one input node....
  //  Connectivity* input_nodes=dest_pins->connect[input_pin];
  //  int input_node=input_nodes->find_acsnode(fpga_core->acs_id);
  int input_node=0;
  
  // Check for BW errors
  int src_majorbit=output_pins->query_majorbit(output_pin);
  int src_bitlen=output_pins->query_bitlen(output_pin);
  int dest_majorbit=dest_pins->query_majorbit(input_pin);
  int dest_bitlen=dest_pins->query_bitlen(input_pin);

/*
  if ((src_majorbit != dest_majorbit) ||
      (src_bitlen != dest_bitlen))
      */
  if (src_bitlen != dest_bitlen)
    {
      if (DEBUG_BWRESOLVER)
	{
	  printf("BW error:driving bw(%d,%d), sink bw(%d,%d)\n",
		 src_majorbit,src_bitlen,
		 dest_majorbit,dest_bitlen);
	  printf("Adding buffer to remedy mismatch\n");
	}
      
      int sign_convention=SIGNED;
      int bitslice_strategy=PRESERVE_LSB;
      if (output_core!=NULL)
	{
	  sign_convention=output_core->sign_convention;
	  bitslice_strategy=output_core->bitslice_strategy;
	}
      if (dest_core==NULL)
	bitslice_strategy=PRESERVE_LSB;

      ACSCGFPGACore* buffer_core=construct->add_buffer(sign_convention,smart_generators);
      buffer_core->bitslice_strategy=bitslice_strategy;
      Pin* buffer_pins=buffer_core->pins;
      
      if (dest_core!=NULL)
	{
	  if (output_core != NULL)
	    construct->insert_sg(output_core,
				 buffer_core,
				 dest_core,
				 output_pin,
				 output_node,
				 DATA_NODE,
				 input_pin,
				 input_node,
				 output_nodes->query_pintype(output_node));
	  else
	    {
	      // Sense of direction is confusing at this point since external signals
	      // can be both input and output pins
	      int ptype=output_pins->pin_classtype(output_pin);

	      if (DEBUG_BWRESOLVER)
		{
		  printf("Inserting a buffer core %s between the extpin (type %d) and %s\n",
			 buffer_core->comment_name(),
			 ptype,
			 dest_core->comment_name());
		  printf("output_pin=%d, output_node=%d, input_pin=%d, input_node=%d\n",
			 output_pin,
			 output_node,
			 input_pin,
			 input_node);
		}

	      construct->insert_sg(output_pins,
				   buffer_core,
				   dest_core,
				   output_pin,
				   output_node,
				   EXT_NODE,
				   input_pin,
				   input_node,
				   output_nodes->query_pintype(output_node));
	    }
	}
      else
	{
	  if (output_core != NULL)
	    // Must be an external node
	    construct->insert_sg(output_core,
				 buffer_core,
				 dest_pins,
				 output_pin,
				 output_node,
				 DATA_NODE,
				 input_pin,
				 input_node,
				 output_nodes->query_pintype(output_node));
	}
	  
      // WARNING: A little liberty is taken on the pin values
      //          for the buffer smart generator
      buffer_pins->set_precision(0,
				 src_majorbit,
				 src_bitlen,
				 UNLOCKED);
      buffer_pins->set_precision(1,
				 dest_majorbit,
				 dest_bitlen,
				 UNLOCKED);

      if (DEBUG_BWRESOLVER)
	printf("BWerror resolved\n\n");
    }

  // Cleanup
}


////////////////////////////////////////////////////
// Identify sg links which cross domains and resolve
////////////////////////////////////////////////////
void ACSCGFPGATarget::HWalg_resolver(CoreList* sg_list)
{
  if (DEBUG_RESOLVER)
    printf("\n\nResolver\n");

  // Determine data signal links between smart generators
  if (DEBUG_RESOLVER)
    printf("Resolving data driving signals\n");
  HWresolve_drivingsignals(sg_list,DATA_NODE);

  // Determine ctrl signal links between smart generators
  if (DEBUG_RESOLVER)
    printf("Resolving ctrl driving signals\n");
  HWresolve_drivingsignals(sg_list,CTRL_NODE);

  if (DEBUG_RESOLVER)
    printf("Resolving unconnected ctrl driven signals\n");
  HWresolve_ictrlsignals(sg_list);

  if (DEBUG_RESOLVER)
    printf("Resolving unconnected external driven signals\n");
  HWresolve_ectrlsignals();
}

void ACSCGFPGATarget::HWresolve_drivingsignals(CoreList* sg_list,
					       const int mode)
{
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_core->acs_device);
      Pin* fpga_pins=fpga_core->pins;

      ACSIntArray* opins=NULL;
      if (mode==DATA_NODE)
	opins=fpga_pins->query_onodes();
      else
	opins=fpga_pins->query_ocnodes();

      if (DEBUG_RESOLVER)
	if (mode==DATA_NODE)
	  printf("resolving driving links for sg %s with %d opins\n",
		 fpga_core->comment_name(),
		 opins->population());
	else
	  printf("resolving driving links for sg %s with %d copins\n",
		 fpga_core->comment_name(),
		 opins->population());
      
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int output_pin=opins->query(opin_loop);
	  Connectivity* output_nodes=fpga_pins->query_connection(output_pin);

	  if (DEBUG_RESOLVER)
	    printf("pin %d has %d nodes\n",output_pin,output_nodes->node_count);
	  for (int onode_loop=0;onode_loop<output_nodes->node_count;onode_loop++)
	    {
	      Pin* dest_pins=NULL;
	      char* name=new char[MAX_STR];
	      ACSCGFPGACore* dest_core=NULL;
	      switch (output_nodes->query_pintype(onode_loop))
		{
		case DATA_NODE:
		  dest_core=HWfind_star(output_nodes->query_acsid(onode_loop));
		  dest_pins=dest_core->pins;
		  strcpy(name,dest_core->comment_name());
		  break;
		case EXT_NODE:
		  dest_pins=fpga_elem->ext_signals;
		  strcpy(name,"ext_signals");
		  break;
		case CONSTANT_NODE:
		  dest_pins=fpga_elem->constant_signals;
		  strcpy(name,"constant_signals");
		  break;
		case CTRL_NODE:
		  dest_core=HWfind_star(output_nodes->query_acsid(onode_loop));
		  if (dest_core==NULL)
		    {
		      dest_pins=fpga_elem->ctrl_signals;
		      strcpy(name,"ctrl_signals");
		    }
		  else
		    {
		      dest_pins=dest_core->pins;
		      strcpy(name,dest_core->comment_name());
		    }
		  break;
		default:
		  dest_pins=NULL;
		  strcpy(name,"ERROR");
		}

	      int input_pin=output_nodes->query_pinid(onode_loop);
	      Connectivity* input_nodes=dest_pins->query_connection(input_pin);
	      int input_node=input_nodes->find_acsnode(fpga_core->acs_id);
	      
	      if (DEBUG_RESOLVER)
		printf("input_pin=%d, input_node=%d\n",input_pin,input_node);
	      
	      int data_signal=-1;
	      if (output_nodes->query_pinsignal(onode_loop)==UNASSIGNED)
		{
		  if (onode_loop!=0) // Bind all additonal node signals to the primary
		    {
		      data_signal=output_nodes->query_pinsignal(0);
		      output_nodes->set_pinsignal(onode_loop,data_signal);
		    }
		  else
		    {
		      // Need to assign an intermediate signal name for these
		      // Add a new data signal name
		      char* tmp_name=new char[MAX_STR];

		      if (output_nodes->query_pintype(onode_loop)==DATA_NODE)
			{
			  Pin* local_dsignals=fpga_elem->data_signals;
			  sprintf(tmp_name,"data_signal%d",local_dsignals->query_pincount());
			  data_signal=local_dsignals->add_pin(tmp_name,
							      fpga_pins->query_majorbit(output_pin),
							      fpga_pins->query_bitlen(output_pin),
							      STD_LOGIC,
							      INTERNAL_PIN);
			}
		      else
			{
			  Pin* local_dsignals=fpga_elem->ctrl_signals;
			  sprintf(tmp_name,"ctrl_signal%d",local_dsignals->query_pincount());
			  data_signal=local_dsignals->add_pin(tmp_name,
							      fpga_pins->query_majorbit(output_pin),
							      fpga_pins->query_bitlen(output_pin),
							      STD_LOGIC,
							      INTERNAL_PIN);
			}
		      // Cleanup
		      delete []tmp_name;
		      
		      output_nodes->set_pinsignal(onode_loop,data_signal);
		    }
		}
	      else
		data_signal=output_nodes->query_pinsignal(onode_loop);
	      
	      // Notify the driven node of its data signal identifier
	      input_nodes->set_pinsignal(input_node,data_signal);

	      if (DEBUG_RESOLVER)
		{
		  printf("it is connected to star %s via input_node %d\n",name,input_node);
		  printf("Assigning signal %d of type %s to this link\n",data_signal,name);
		}
	      delete []name;
	      
	    } // for (int onode_loop=0;onode_loop<output_nodes->node_count; ...
	} // for (int opin_loop=0;opin_loop<opins->size();opin_loop++)
    } // for (int loop=0;loop<sg_list->size();loop++)
}


// Check for dangling internal control signals
void ACSCGFPGATarget::HWresolve_ictrlsignals(CoreList* sg_list)
{
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_core->acs_device);
      Pin* fpga_pins=fpga_core->pins;
      if (DEBUG_RESOLVER)
	printf("\nResolving control links for star %s\n",
	       fpga_core->comment_name());

      HWresolve_ctrlpins(fpga_elem,fpga_core,fpga_pins,fpga_pins->query_icnodes());
    }
}

// Check for dangling internal control signals
void ACSCGFPGATarget::HWresolve_ectrlsignals(void)
{
  for (int loop=0;loop<arch->fpga_count;loop++)
    {
      Fpga* fpga_elem=arch->get_fpga_handle(loop);
      if (fpga_elem->usage_state==FPGA_USED)
	{
	  Pin* cpins=fpga_elem->ext_signals;
	  HWresolve_ctrlpins(fpga_elem,NULL,fpga_elem->ext_signals,cpins->query_icnodes());
	  HWresolve_ctrlpins(fpga_elem,NULL,fpga_elem->ext_signals,cpins->query_ocnodes());
	}
    }
}


void ACSCGFPGATarget::HWresolve_ctrlpins(Fpga* fpga_elem, 
					 ACSCGFPGACore* fpga_core,
					 Pin* fpga_pins,
					 ACSIntArray* cpins)
{      
  for (int cpin_loop=0;cpin_loop<cpins->population();cpin_loop++)
    {
      int control_pin=cpins->query(cpin_loop);
      if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
	{
	  // Port was not assigned previously, need to resolve with externals
	  if (DEBUG_RESOLVER)
	    printf("Driving control pin %s, of type %d, priority %d is unresolved\n",
		   fpga_pins->query_pinname(control_pin),
		   fpga_pins->query_pintype(control_pin),
		   fpga_pins->query_pinpriority(control_pin));
	  
	  // Trap for external signals
	  if (DEBUG_RESOLVER)
	    printf("Comparing to external signals\n");
	  HWsig_compare(fpga_pins->query_pintype(control_pin),
			fpga_pins,
			control_pin,
			fpga_elem->ext_signals,
			EXT_NODE,
			NULL);

	  // Still unresolved?
	  // Then check against internal control signals
	  if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
	    {
	      if (DEBUG_RESOLVER)
		printf("Comparing to internal signals\n");
	      
	      // Only local control signals are allowed!
	      CoreList* fpga_children=fpga_elem->get_childcores_handle();
	      
	      for (int iloop=0;iloop<fpga_children->size();iloop++)
		{
		  ACSCGFPGACore* candidate_core=(ACSCGFPGACore*) fpga_children->elem(iloop);
		  
		  // No inbreeding:)
		  int acs_id=UNASSIGNED;
		  if (fpga_core!=NULL)
		    acs_id=fpga_core->acs_id;
		  if (acs_id != candidate_core->acs_id)
		    {
		      if (DEBUG_RESOLVER)
			printf("Checking sg %s\n",candidate_core->comment_name());
		      
		      // Check smart generators 
		      // fpga_elem->ctrl_signals or candidate_core????
		      if (fpga_core!=NULL)
			{
			  if (DEBUG_RESOLVER)
			    printf("checking for binds to control signals\n");
			  HWsig_compare(fpga_pins->query_pintype(control_pin),
					fpga_pins,
					control_pin,
					candidate_core->pins,
					CTRL_NODE,
					fpga_elem->ctrl_signals);
			}
		      else
			{
			  if (DEBUG_RESOLVER)
			    printf("checking for binds to external signals\n");
			  HWsig_compare_ext(fpga_pins->query_pintype(control_pin),
					    fpga_pins,
					    control_pin,
					    candidate_core->pins,
					    EXT_NODE,
					    NULL);
			}
		    }
		}
	    }
	      
	  // Still unresolved?
	  // Then check against constant signals using the pin's default pin assertion
	  if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
	    {
	      if (DEBUG_RESOLVER)
		printf("Comparing to constant signals\n");
	      
	      HWsig_compare(fpga_pins->query_pinassertion(control_pin),
			    fpga_pins,
			    control_pin,
			    fpga_elem->constant_signals,
			    CONSTANT_NODE,
			    NULL);
	    }
	} // if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
	else
	  {
	    if (DEBUG_RESOLVER)
	      {
		Connectivity* control_nodes=fpga_pins->query_connection(control_pin);
		printf("Pin %s, of type %d is already resolved, pin_signal=%d\n",
		       fpga_pins->query_pinname(control_pin),
		       fpga_pins->query_pintype(control_pin),
		       control_nodes->query_pinsignal(0));
	      }
	  }
    } // for (int cpin_loop=0;cpin_loop<cpins->size();cpin_loop++)
}
  
void ACSCGFPGATarget::HWsig_compare(const int refpin_type,
				    Pin* ref_pins,
				    const int pin_no,
				    Pin* candidate_pins,
				    const int rule_type,
				    Pin* ctrl_pins)
{
  for (int pin_loop=0;pin_loop<candidate_pins->query_pincount();pin_loop++)
    {
      Connectivity* candidate_connects=candidate_pins->query_connection(pin_loop);

      int match=0;
      int assertion_type=0;
      switch (rule_type)
	{
	case EXT_NODE:
	  match=HWresolver_extrules_driving(refpin_type,
					    candidate_pins->query_pintype(pin_loop));
	  break;
	case CTRL_NODE:
	  match=HWresolver_ctrlrules(refpin_type,
				     ref_pins->query_pinpriority(pin_no),
				     candidate_pins->query_pintype(pin_loop),
				     candidate_pins->query_pinpriority(pin_loop));
	  break;
	case CONSTANT_NODE:
	  assertion_type=ref_pins->query_pinassertion(pin_no);

	  if (DEBUG_RESOLVER)
	    printf("constant rules, comparing pin %s, assertion_type %d with %s\n",
		   ref_pins->query_pinname(pin_no),
		   ref_pins->query_pinassertion(pin_no),
		   candidate_pins->query_pinname(pin_loop));
	  match=HWresolver_construles(assertion_type,
				      candidate_pins->query_pintype(pin_loop));
	  break;
	default:
	  match=0;
	}

      if (DEBUG_RESOLVER)
	if (match)
	  printf("Match found for pin %s, its match is %s\n",
		 ref_pins->query_pinname(pin_no),candidate_pins->query_pinname(pin_loop));

      if (match)
	if ((rule_type==CTRL_NODE) && (candidate_pins->query_pinassigned(pin_loop)==UNASSIGNED))
	  {
	    // Need to add a new internal control signal
	    // Add a new ctrl signal name
	    char* tmp_name=new char[MAX_STR];
	    sprintf(tmp_name,"ctrl_signal%d",ctrl_pins->query_pincount());
	    int ctrl_pin=ctrl_pins->add_pin(tmp_name,
					    candidate_pins->query_majorbit(pin_loop),
					    candidate_pins->query_bitlen(pin_loop),
					    STD_LOGIC,
					    INTERNAL_PIN);

	    // Connect control signal on referring smart generator
	    // ASSUMPTION: acs ids are uninteresting at this point
	    if (DEBUG_CONNECT)
	      printf("Assigning ref_pins\n");
	    ref_pins->connect_pin(pin_no,
				  UNASSIGNED,
				  ctrl_pin,
				  rule_type,
				  ctrl_pin);
	    
	    // ASSUMPTION: acs ids are uninteresting at this point
	    if (DEBUG_CONNECT)
	      printf("Assigning ctrl_pins\n");
	    ctrl_pins->connect_pin(ctrl_pin,
				   UNASSIGNED,
				   ctrl_pin,
				   rule_type,
				   ctrl_pin);

	    if (DEBUG_CONNECT)
	      printf("Assigning candidate_pins\n");
	    candidate_pins->connect_pin(pin_loop,
					UNASSIGNED,
					ctrl_pin,
					rule_type,
					ctrl_pin);
	    
	    if (DEBUG_RESOLVER)
	      printf("it binds to signal %s (%d)\n",
		     ctrl_pins->query_pinname(ctrl_pin),
		     ctrl_pin);

	    // Cleanup
	    delete []tmp_name;
	    
	    break;
	  }
	else
	  {
	    // ASSUMPTION: acs ids are uninteresting at this point
	    int sig_id;
	    int type;
	    type=rule_type;
	    int skip_connect=0;
	    if (rule_type==CTRL_NODE)
	      {
		// ASSUMPTION:Only one (unique) signal allowed per pin
		// FIX:Then why is it an array:(
		
		// Check to see if the limit for this control pin has been reached
		if (candidate_connects->node_count < candidate_pins->query_pinlimit(pin_loop)*2)
		  if ((candidate_pins->query_pinpriority(pin_loop)!=NO_PRIORITY)  &&
		      (ref_pins->query_pinpriority(pin_no)!=NO_PRIORITY))
		    if (candidate_pins->query_pinpriority(pin_loop) == 
			ref_pins->query_pinpriority(pin_no))
		      {
			if (DEBUG_RESOLVER)
			  printf("match found here\n");
			sig_id=candidate_connects->query_pinsignal(0);
			type=candidate_connects->query_pintype(0);
		      }
		    else
		      {
			sig_id=-1;
			skip_connect=1;
			if (DEBUG_RESOLVER)
			  printf("candidate pin priority %d does not match ref priority %d\n",
				 candidate_pins->query_pinpriority(pin_loop),
				 ref_pins->query_pinpriority(pin_no));
		      }
		  else
		    {
		      if (DEBUG_RESOLVER)
			printf("match found here2\n");
		      sig_id=candidate_connects->query_pinsignal(0);
		      type=candidate_connects->query_pintype(0);
		    }
		else
		  {
		    sig_id=-1;
		    skip_connect=1;
		    if (DEBUG_RESOLVER)
		      {
			printf("pin limit %d (node_count=%d) has been reached, skip\n",
			       candidate_pins->query_pinlimit(pin_loop),
			       candidate_connects->node_count);
		      }
		  }
	      }
	    else
	      // FIX:Problem here!!!
	      if (candidate_connects->node_count < candidate_pins->query_pinlimit(pin_loop)*2)
		if ((candidate_pins->query_pinpriority(pin_loop)!=NO_PRIORITY)  &&
		    (ref_pins->query_pinpriority(pin_no)!=NO_PRIORITY))
		  if (candidate_pins->query_pinpriority(pin_loop)==(ref_pins->query_pinpriority(pin_no)))
		    sig_id=pin_loop;
		  else
		    {
		      sig_id=-1;
		      skip_connect=1;
		    }
		else
		  sig_id=pin_loop;
	      else
		{
		  sig_id=-1;
		  skip_connect=1;
		}
	    
	    if (!skip_connect)
	      {
		ref_pins->connect_pin(pin_no,
				      UNASSIGNED,
				      sig_id,
				      type,
				      sig_id);
		if (DEBUG_RESOLVER)
		  printf("match found srctype=%d, cand_type=%d pre-existing signal #%d\n",
			 refpin_type,sig_id,type);
		break;
	      }
	    else
	      if (DEBUG_RESOLVER)
		printf("It has been skipped: srctype=%d, cand_type=%d\n",
		       refpin_type,
		       candidate_pins->query_pintype(pin_loop));
	  }
      else
	if (DEBUG_RESOLVER)
	  printf(" does not match, srctype=%d, cand_type=%d, cand_priority=%d\n",
		 refpin_type,
		 candidate_pins->query_pintype(pin_loop),
		 candidate_pins->query_pinpriority(pin_loop));
    }
}
void ACSCGFPGATarget::HWsig_compare_ext(const int refpin_type,
					Pin* ref_pins,
					const int pin_no,
					Pin* candidate_pins,
					const int rule_type,
					Pin* ctrl_pins)
{
  int src_priority=ref_pins->query_pinpriority(pin_no);

  for (int pin_loop=0;pin_loop<candidate_pins->query_pincount();pin_loop++)
    {
      int match=HWresolver_extrules_driven(refpin_type,
					   candidate_pins->query_pintype(pin_loop));

      // Check priorities (if applicable)
      if (match)
	{
	  if (DEBUG_RESOLVER)
	    printf("Match found for pin %s, its match is %s\n",
		   ref_pins->query_pinname(pin_no),candidate_pins->query_pinname(pin_loop));

	  if (src_priority != UNASSIGNED)
	    {
	      if (DEBUG_RESOLVER)
		printf("there is a source priority of %d\n",src_priority);
	      if (candidate_pins->query_pinpriority(pin_loop) == src_priority)
		{
		  // ASSUMPTION: acs ids are uninteresting at this point
		  candidate_pins->connect_pin(pin_loop,
					      UNASSIGNED,
					      pin_no,
					      EXT_NODE,
					      pin_no);
		  if (DEBUG_RESOLVER)
		    printf("match found srctype=%d, binds to ext signal #%d\n",
			   refpin_type,pin_no);
		  return;
		}
	      else
		if (DEBUG_RESOLVER)
		  printf("match found srctype=%d, to ext signal #%d, but priorities mismatch (%d vs. %d)\n",
			 refpin_type,pin_no,
			 src_priority,candidate_pins->query_pinpriority(pin_loop));
	    }
	  else
	    {
	      if (DEBUG_RESOLVER)
		printf("there is no source priority, making connection\n");

	      // ASSUMPTION: acs ids are uninteresting at this point
	      candidate_pins->connect_pin(pin_loop,
					  UNASSIGNED,
					  pin_no,
					  EXT_NODE,
					  pin_no);
	      if (DEBUG_RESOLVER)
		printf("match found srctype=%d, binds to ext signal #%d\n",
		       refpin_type,pin_no);
	      return;
	    }
	}
      else
	if (DEBUG_RESOLVER)
	  printf(" does not match, srctype=%d, cand_type=%d, cand_priority=%d\n",
		 refpin_type,
		 candidate_pins->query_pintype(pin_loop),
		 candidate_pins->query_pinpriority(pin_loop));
    }
}
	    


////////////////////////////////////////////////////////////////////////////////
// These rules define if a control signal should be associated given a reference
// pin on a smart generator (sg_sig) and a likely candidate signal (pool_sig)
// These rules are only to be used for driving external signals!
////////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWresolver_extrules_driving(int sg_sig,int pool_sig)
{
  switch (sg_sig)
    {
      // Clocking pins
    case INPUT_PIN_CLK:
      switch (pool_sig)
	{
	case INPUT_PIN_CLK:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_RESET:
      switch (pool_sig)
	{
	case INPUT_PIN_RESET:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_CLR:
      switch (pool_sig)
	{
	case OUTPUT_PIN_CLR:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_EXTCTRL:
      switch (pool_sig)
	{
	case INPUT_PIN_EXTCTRL:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_MEMOK:
      switch (pool_sig)
	{
	case INPUT_PIN_MEMOK:
	  return(1);
	default:
	  return(0);
	}
    }

  // Return unhappy condition
  return(0);
}
////////////////////////////////////////////////////////////////////////////////
// These rules define if a control signal should be associated given a reference
// pin on a smart generator (sg_sig) and a likely candidate signal (pool_sig)
// These rules are only to be used for driven external signals!
////////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWresolver_extrules_driven(int sg_sig,int pool_sig)
{
  switch (sg_sig)
    {
    case OUTPUT_PIN_INTERCONNECT_ENABLE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_INTERCONNECT_ENABLE:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_MEMREQ:
      switch (pool_sig)
	{
	case OUTPUT_PIN_MEMREQ:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_DONE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_DONE:
	  return(1);
	default:
	  return(0);
	}
    }

  // Return unhappy condition
  return(0);
}
// These rules define if a control signal should be associated given a reference
// pin on a smart generator (sg_sig) and a likely candidate signal (pool_sig)
int ACSCGFPGATarget::HWresolver_ctrlrules(const int sg_sig,
					  const int sg_priority,
					  const int pool_sig,
					  const int pool_priority)
{
  // Trivial test for mismatch on priorities would preclude any match
  if (sg_priority!=pool_priority)
    return(0);

  switch (sg_sig)
    {
    case INPUT_PIN_RAMG:
      switch (pool_sig)
	{
	case INPUT_PIN_RAMG:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_RAMW:
      switch (pool_sig)
	{
	case INPUT_PIN_RAMW:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_RAME:
      switch (pool_sig)
	{
	case INPUT_PIN_RAME:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_ADDRCE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_ADDRCE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_ADDRCLR:
      switch (pool_sig)
	{
	case OUTPUT_PIN_ADDRCLR:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_PHASE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_PHASE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_PREPHASE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_PREPHASE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_START:
      switch (pool_sig)
	{
	case OUTPUT_PIN_START:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_PHASE_START:
      switch (pool_sig)
	{
	case OUTPUT_PIN_PHASE_START:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_STOP:
      switch (pool_sig)
	{
	case OUTPUT_PIN_STOP:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SEQRESET:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SEQRESET:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_LUTINDEX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_LUTINDEX:
	  return(1);
	default:
	  return(0);
	}
    }

  // Return unhappy condition
  return(0);
}
int ACSCGFPGATarget::HWresolver_construles(int sg_sig,int pool_sig)
{
  switch (sg_sig)
    {
    case AH:
      switch (pool_sig)
	{
	case OUTPUT_PIN_VCC:
	  return(1);
	default:
	  return(0);
	}
    case AL:
      switch (pool_sig)
	{
	case OUTPUT_PIN_GND:
	  return(1);
	default:
	  return(0);
	}
    }

  // Return unhappy condition
  return(0);
}

void ACSCGFPGATarget::HWmacro(CoreList* sg_list)
{
  int all_queried=0;
  while (!all_queried)
    {
      all_queried=1;
      int sg=0;
      while ((sg<sg_list->size()) && (all_queried==1))
	{
	  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(sg);
	  if (DEBUG_WHITESTAR)
	    printf("macro_queried for %s is %d\n",fpga_core->comment_name(),
		   fpga_core->macro_queried);
	  
	  // If this is a white star, then let's build it's interior
	  if (!fpga_core->macro_queried)
	    {
	      all_queried=0;
	      fpga_core->macro_queried=1;

	      if (DEBUG_WHITESTAR)
		printf("evaluating core %d %s for macros\n",
		       sg,
		       fpga_core->comment_name());
	      if (fpga_core->macro_query()==WHITE_STAR)
		{
		  // White star investigator
		  HWwhite_star(fpga_core,sg_list);
		}
	      else if (fpga_core->macro_query()==DARK_STAR)
		{
		  // Dark star investigator
		  HWdark_star(fpga_core,sg_list);
		}
	    }
	  sg++;
	}
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This method should be invoked after any cores have been added to the algorithm.
// White stars and dark stars are examined, core requirements are determined and
// BWs are refined.
//////////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWalg_setup(CoreList* sg_list)
{
  // Identify any libraries needed
  HWlib_needs(sg_list);

  // Ensure that precision requirements are met before proceeding to scheduling
  HWset_bw(sg_list);
}


void ACSCGFPGATarget::HWwhite_star(ACSCGFPGACore* fpga_core, CoreList* sg_list)
{
  fpga_core->macro_queried=1;

  CoreList* white_stars=fpga_core->macro_build(&free_acsid,&free_netlist_id);
  Connection_List* white_cons=fpga_core->macro_connections();
  
  if (white_stars!=NULL)
    {
      if (DEBUG_WHITESTAR)
	printf("sg %s is a white star with %d children\n",fpga_core->comment_name(),
	       white_stars->size());
      for (int white_loop=0;white_loop<white_stars->size();white_loop++)
	{
	  ACSCGFPGACore* white_star=(ACSCGFPGACore*) white_stars->elem(white_loop);
	  sg_list->append(white_star);
	  if (DEBUG_WHITESTAR)
	    printf("\tchild:%s\n",white_star->comment_name());
	}
      
      // FIX: If parent core has a resource assignment then follow up the assignment
      //      with the children
      
      // Propagate device ownership of parent to children as well as origin
      Fpga* target_fpga=arch->get_fpga_handle(fpga_core->acs_device);
      for (int white_loop=0;white_loop<white_stars->size();white_loop++)
	{
	  ACSCGFPGACore* white_star=(ACSCGFPGACore*) white_stars->elem(white_loop);
	  white_star->acs_device=target_fpga->retrieve_acsdevice();
	  white_star->target_type=target_fpga->part_type;
	  white_star->acs_origin=fpga_core->acs_origin;
	  target_fpga->set_child(white_star);
	}

      // Remap the io of the parent white star to the child cores
      Pin* fpga_pins=fpga_core->pins;
      for (int con_loop=0;con_loop<white_cons->count();con_loop++)
	{
	  int ext_src_pin, src_id, src_pin;
	  int ext_dest_pin, dest_id, dest_pin;
	  white_cons->get(con_loop,
			  ext_src_pin,src_id,src_pin,
			  ext_dest_pin,dest_id,dest_pin);
	  if (DEBUG_WHITESTAR)
	    printf("con(%d)=%d, %d, %d and %d, %d, %d\n",
		   con_loop,
		   ext_src_pin,src_id,src_pin,
		   ext_dest_pin,dest_id,dest_pin);

	  // Reconnect source pin
	  if (src_id==UNASSIGNED)
	    {
	      // Recast the pin type, priority and defaults
	      // FIX:Probably a buncha others to:(
	      if (dest_pin!=UNASSIGNED)
		{
		  int pin_type=fpga_pins->query_pintype(ext_dest_pin);
		  int pin_priority=fpga_pins->query_pinpriority(ext_dest_pin);
		  int pin_assertion=fpga_pins->query_pinassertion(ext_dest_pin);
		  int pin_netlistid=fpga_pins->query_netlistid(ext_dest_pin);
		  if (DEBUG_WHITESTAR)
		    printf("white_star:dest_pin netlist id=%d\n",pin_netlistid);

		  ACSCGFPGACore* dest_core=HWfind_star(dest_id);
		  Pin* dest_pins=dest_core->pins;
		  dest_core->replace_pintype(dest_pin,pin_type);
		  dest_core->set_pinpriority(dest_pin,pin_priority);
		  dest_pins->set_pinassertion(dest_pin,pin_assertion);
		  dest_pins->set_netlistid(dest_pin,pin_netlistid);
		}

	      // First remove the old
	      Connectivity* inodes=fpga_pins->query_connection(ext_dest_pin);
	      if (inodes != NULL)
		if (inodes->node_count != 0)
		  {
		    src_id=inodes->query_acsid(0);
		    src_pin=inodes->query_pinid(0);
		    if (src_id != UNASSIGNED)
		      {
			ACSCGFPGACore* src_core=HWfind_star(src_id);
			Pin* src_pins=src_core->pins;
			src_pins->disconnect_pin_to(fpga_core->acs_id,src_pin);

			// Now connect the new
			ACSCGFPGACore* dest_core=HWfind_star(dest_id);

			// Ensure that, indeed, there was a connection to begin with
			// FIX:Are both connection constructs needed?
			if (dest_core != NULL)
			  if (dest_pin == UNASSIGNED)
			    construct->connect_sg(src_core,
						  src_pin,
						  UNASSIGNED,
						  dest_core,
						  UNASSIGNED,
						  UNASSIGNED,
						  DATA_NODE);
			  else
			    {
			      if (DEBUG_WHITESTAR)
				printf("white_star:connection %s pin %d to %s pin %d\n",
				       src_core->comment_name(),src_pin,
				       dest_core->comment_name(),dest_pin);
			      construct->connect_sg(src_core,
						    src_pin,
						    UNASSIGNED,
						    dest_core,
						    dest_pin,
						    UNASSIGNED,
						    DATA_NODE);
			    }
		      }
		  }
	      
	    }
	  else // dest_id==UNASSIGNED
	    {
	      ACSCGFPGACore* src_core=HWfind_star(src_id);

	      // Recast the pin type and priority
	      if (src_pin!=UNASSIGNED)
		{
		  int pin_type=fpga_pins->query_pintype(ext_src_pin);
		  int pin_priority=fpga_pins->query_pinpriority(ext_src_pin);
		  int pin_netlistid=fpga_pins->query_netlistid(ext_src_pin);
		  if (DEBUG_WHITESTAR)
		    printf("white_star:src_pin netlist id=%d\n",pin_netlistid);

		  src_core->replace_pintype(src_pin,pin_type);
		  src_core->set_pinpriority(src_pin,pin_priority);
		  Pin* src_pins=src_core->pins;
		  src_pins->set_netlistid(src_pin,pin_netlistid);
		}

	      // First remove the old
	      Connectivity* onodes=fpga_pins->query_connection(ext_src_pin);
	      if (onodes != NULL)
		for (int onode_loop=0;onode_loop<onodes->node_count;onode_loop++)
		  {
		    dest_id=onodes->query_acsid(onode_loop);
		    if (dest_id != UNASSIGNED)
		      {
			// Now connect the new
			ACSCGFPGACore* dest_core=HWfind_star(dest_id);

			// Ensure that, indeed, there was a connection to begin with
			if (dest_core != NULL)
			  {
			    Pin* dest_pins=dest_core->pins;
			    dest_pin=onodes->query_pinid(onode_loop);
			    dest_pins->disconnect_pin_from(fpga_core->acs_id,
							    dest_pin);
			    
			    if (src_pin == UNASSIGNED)
			      construct->connect_sg(src_core,dest_core);
			    else
			      construct->connect_sg(src_core,
						    src_pin,
						    UNASSIGNED,
						    dest_core,
						    dest_pin,
						    UNASSIGNED,
						    DATA_NODE);
			  }
			else
			  {
			    Fpga* fpga_elem=arch->get_fpga_handle(src_core->acs_device);
			    Pin* dest_pins=fpga_elem->ext_signals;
			    dest_pin=onodes->query_pinid(onode_loop);
			    dest_pins->disconnect_pin(src_pin,onode_loop);
			    
			    construct->connect_sg(src_core,
						  src_pin,
						  onode_loop,
						  dest_pin,
						  dest_pins,
						  dest_pin,
						  UNASSIGNED,
						  dest_pin,
						  EXT_NODE);
			  }
		      }
		  }

	    }
	}
      // White core no longer needed
      fpga_pins->disconnect_allpins();
      sg_list->remove(fpga_core->acs_id);
      Fpga* white_fpga=arch->get_fpga_handle(fpga_core->acs_device);
      CoreList* fpga_list=white_fpga->get_childcores_handle();
      fpga_list->remove(fpga_core->acs_id);
      
      // Cleanup
      delete white_stars;
      delete white_cons;
    }
  else
    fprintf(stderr,"Sg %s claims to be a WHITE_STAR yet returns no children...ingoring\n",
	    fpga_core->comment_name());
}

void ACSCGFPGATarget::HWdark_star(ACSCGFPGACore* fpga_core, CoreList* sg_list)
{
  fpga_core->macro_queried=1;
  
//  CoreList* dark_stars=fpga_core->macro_build(&free_acsid);
  
  // Trap the dark star list for any white stars
  // any child white stars would be a violation
}

// FIX: Should the libraries also be parameterized since the origin is as well?
void ACSCGFPGATarget::HWlib_needs(CoreList* sg_list)
{
  int sg_count=sg_list->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);
      StringArray* star_libraries=fpga_core->libs;
      StringArray* star_includes=fpga_core->incs;
      if (!fpga_core->lib_queried)
	{
	  // Bundle up library and include dependencies
	  for (int lib_loop=0;lib_loop<star_libraries->population();lib_loop++)
	    all_libraries->add(star_libraries->get(lib_loop));
	  for (int inc_loop=0;inc_loop<star_includes->population();inc_loop++)
	    all_includes->add(star_includes->get(inc_loop));
	  fpga_core->lib_queried=1;
	}
      else
	if (DEBUG_ALGSETUP)
	  printf("sg_setup already invoked for sg %s\n",fpga_core->comment_name());
    }
}


void ACSCGFPGATarget::HWalg_create(const int fpga_no)
{
  if (DEBUG_ALGCREATE)
    printf("\n\nHWalg_create invoked for galaxy %s\n\n",galaxy()->name());

  // Only generate information for the smart generator at-hand
  Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
  CoreList* sg_subset=fpga_elem->get_childcores_handle();

  // Results
  ostrstream expression;
  
  //FIX: For now assuming only VHDL as language
  VHDL_LANG* lang=new VHDL_LANG;

  // Establish filenames and open file stream
  char* root_filename=new char[MAX_STR];
  sprintf(root_filename,"%s_fpga%d",galaxy()->name(),fpga_no);
  fpga_elem->set_rootfilename(root_filename);
  
  ostrstream of_filename;
  of_filename << design_directory->retrieve_extended() 
	      << fpga_elem->retrieve_rootfilename()
	      << ".vhd" << ends;
  ofstream open_file(of_filename.str());
  
  // Add libraries/includes
  open_file << lang->gen_libraries(all_libraries,all_includes);
//  open_file << lang->gen_entity(fpga_elem->retrieve_rootfilename(),
//				fpga_elem->ext_signals);
  char* entity_name=new char[MAX_STR];
  sprintf(entity_name,"fpga%d_entity",fpga_no);
  open_file << lang->gen_entity(entity_name,
				fpga_elem->ext_signals);
//  open_file << lang->gen_architecture(fpga_elem->retrieve_rootfilename(),
  open_file << lang->gen_architecture(entity_name,
				      sg_subset,
				      STRUCTURAL,
				      fpga_elem->ext_signals,
				      fpga_elem->data_signals,
				      fpga_elem->ctrl_signals,
				      fpga_elem->constant_signals);
				     
  // Close-out any opened files
  if (open_file != NULL)
    open_file.close();

  // Task each smart generator to produce "code" for this fpga
  for (int loop=0;loop<sg_subset->size();loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_subset->elem(loop);
      if (DEBUG_ALGCREATE)
	printf("HWalg_create:Initiated VHDL construction for star %s\n",
	       fpga_core->comment_name());
      
      fpga_core->acs_build();
    }

  // Cleanup
  delete lang;
  delete []root_filename;
  delete []entity_name;
}

// FIX: A bit hard coded to the Annapolis Wildforce...
void ACSCGFPGATarget::HWalg_integrate(const int fpga_no)
{
  if (DEBUG_ALGINTEGRATE)
    printf("\n\nHWalg_create invoked for galaxy %s\n\n",galaxy()->name());

  Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);

  // Results
  ostrstream expression;

  //FIX: For now assuming only VHDL as language
  VHDL_LANG* lang=new VHDL_LANG;

  // Establish filenames and open file stream
  char* root_filename=new char[MAX_STR];
  sprintf(root_filename,"pe%dlca",fpga_no);

  ostrstream of_filename;
  of_filename << design_directory->retrieve_extended() << root_filename 
	      << ".vhd" << ends;
  ofstream open_file(of_filename.str());

  ostrstream hierarchy_name;
  hierarchy_name << "PE" << fpga_no << "_Logic_Core" << ends;

  // Generate bindings
  // FIX:Should be generalized
  ostrstream statements;
  statements << endl << "-- Bindings" << endl
	     << "U" << fpga_no << ":" << fpga_elem->retrieve_rootfilename() 
	     << " port map (" << endl
	     << "Clock => PE_Pclk," << endl
	     << "Reset => PE_RESET," << endl
//	     << "XbarData_InReg => PE_XbarData_InReg," << endl
	     << "Go => PE_CPE_Bus_In(0)," << endl
	     << "MemBusGrant_n => PE_MemBusGrant_n," << endl
	     << "MemBusReq_n => PE_MemBusReq_n," << endl
	     << "Done => PE_CPE_Bus_Out(1)," << endl;


  // Add interconnects
  // FIX: Annapolis specific switches
  if (fpga_no != 4)
    statements << "Right_InReg => PE_Right_InReg," << endl
	       << "Right_Out => PE_Right_Out," << endl
	       << "Right_OE => PE_Right_OE," << endl;
  if (fpga_no != 1)
    statements << "Left_InReg => PE_Left_InReg," << endl
	       << "Left_Out => PE_Left_Out," << endl
	       << "Left_OE => PE_Left_OE," << endl;
  
    
  // Memory assignments
  statements << "MemData_InReg => PE_MemData_InReg," << endl
	     << "MemData_OutReg => PE_MemData_OutReg," << endl
	     << "MemAddr_OutReg => PE_MemAddr_OutReg," << endl
	     << "MemWriteSel_n => PE_MemWriteSel_n," << endl
	     << "MemStrobe_n => PE_MemStrobe_n" << endl
	     << ");" << endl << endl;

  // Generate assignments
  statements << "-- Enable handshake lines" << endl
	     << lang->equals("PE_CPE_Bus_OE","\"10\"") 
	     << lang->end_statement << endl;

  // Disable leftover controls
  statements << lang->equals("PE_InterruptReq_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_MemHoldReq_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspInterruptReq_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspSemaphore_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspMemBusReq_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspMemStrobe_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspMemWriteSel_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspHandShake_OE","( others => '0')")
	     << lang->end_statement << endl
	     << lang->equals("PE_DspHoldReq_n","'1'")
	     << lang->end_statement << endl
	     << lang->equals("PE_XbarData_OE","( others => '0')")
	     << lang->end_statement << endl
	     << lang->equals("PE_XbarData_WE","( others => '0')")
	     << lang->end_statement << endl;
  
  // Disable fifo control signals for FPGAs 1 and 4
  if ((fpga_no==1) || (fpga_no==4))
    statements << lang->equals("PE_FifoSelect","\"00\"") 
	       << lang->end_statement << endl
	       << lang->equals("PE_Fifo_WE_n","'1'")
	       << lang->end_statement << endl
	       << lang->equals("PE_FifoPtrIncr_EN","'0'")
	       << lang->end_statement << endl;

  statements << ends;

  // Generate VHDL file
  open_file << "library IEEE;" << endl;
  open_file << "use IEEE.std_logic_1164.all;" << endl << endl;
  open_file << lang->gen_architecture(hierarchy_name.str(),
				      NULL,
				      BLANK,
				      fpga_elem->ext_signals,
				      NULL,
				      NULL,
				      NULL);
  open_file << lang->start_component(fpga_elem->retrieve_rootfilename());
  open_file << lang->start_port() << endl;
  open_file << lang->set_port(fpga_elem->ext_signals) << endl;
  open_file << lang->end_port() << endl;
  open_file << lang->end_component() << endl;
  open_file << lang->begin_scope << endl;
  open_file << statements.str();
  open_file << lang->end_architecture(BLANK) << endl;

  // Close-out any opened files
  if (open_file != NULL)
    open_file.close();

  // Cleanup
  delete lang;
  delete []root_filename;
}

//////////////////////////////////////////////////////////////////
// Export the necessary scripts to allow the developer to simulate 
// the results
// FIX: Still some dependencies based on the Annoplis VHDL
//      VHDL hierarchy:(
//////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWexport_simulation(CoreList* smart_generators)
{
  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  ostrstream compile_filename;
  ostrstream simulate_filename;
  compile_filename << design_directory->retrieve_extended() << root_filename 
		   << "_mti_vcom.do" << ends;
  simulate_filename << design_directory->retrieve_extended() << root_filename 
		    << "_mti_vsim.do" << ends;
  ofstream compile_file(compile_filename.str());
  ofstream simulate_file(simulate_filename.str());

  compile_file << "-----------------------------------------------------------------" 
	       << endl 
	       << "--" << endl
	       << "--  Copyright (C) 1996-1997 Annapolis Micro Systems, Inc." << endl 
	       << "--" << endl
	       << "-----------------------------------------------------------------" << endl
	       << "--  Description   : MTI V-System macro file used to analyze VHDL" << endl 
	       << "--                  files for the WF4 VHDL Simulator.  This macro" << endl
	       << "--                  file is to be used in conjunction with the" << endl
	       << "--                  Windows NT MTI V-System VHDL simulator," << endl
	       << "--                  version 4.6 or above." << endl
	       << "--" << endl
	       << "--  Date          : 11/12/96" << endl
	       << "--" << endl
	       << "--  Author        : Chris Esser, AMS" << endl
	       << "--" << endl;
  compile_file << endl << endl
	       << "--" << endl
	       << "-- Automated by Sanders, a Lockheed Martin Company" << endl
	       << "-- Ken Smith" << endl
	       << "-- 2/1/99" << endl
	       << "--" << endl
	       << "---------------------------------------------------------"
	       << endl;

  compile_file << "set ARCH_BASE " << hw_path->retrieve() << endl;
  compile_file << "set MODEL_TECH " << sim_path->retrieve() << endl;
  compile_file << "set COREGEN " << core_path->retrieve() << endl;
  compile_file << "set PROJECT_BASE " << compile_directory->retrieve() << endl;
  compile_file << "--" << endl
	       << "--  Create the MTI libraries for VHDL simulation" << endl
	       << "--" << endl;
  compile_file << "cd \"$PROJECT_BASE\"" << endl
	       << "vlib ./mti_" << root_filename << endl
	       << "vlib ./mti_" << root_filename << "/mti_fpga0" << endl;

  compile_file << "--" << endl
	       << "--  Map the MTI libraries for VHDL simulation" << endl
	       << "--" << endl
	       << "vmap std $MODEL_TECH/std" << endl
	       << "vmap ieee $MODEL_TECH/ieee" << endl
	       << "vmap xul $COREGEN/xul" << endl
//	       << "vmap unisim $COREGEN/unisim" << endl
	       << "vmap ARCH $ARCH_BASE/lib/mti" << endl
	       << "vmap work ./mti_" << root_filename << endl
	       << "vmap system ./mti_" << root_filename << endl
//	       << "vmap cpe0 ./mti_" << root_filename << "/mti_fpga0" 
	       << endl;

  
  char* compile=new char[MAX_STR];
  char* compile_fpga0=new char[MAX_STR];
  strcpy(compile,"vcom -93 -explicit -work work ");
  strcpy(compile_fpga0,"vcom -93 -explicit -work cpe0 ");  

  compile_file << "--" << endl
	       << "--  Top level architecture files" << endl
	       << "--" << endl;
  StringArray* support_files=arch->archsupport_files;
  StringArray* support_path=arch->archsupport_path;
  for (int loop=0;loop<support_files->population();loop++)
    compile_file << compile
		 << hw_path->retrieve_extended()
		 << support_path->get(loop) 
		 << support_files->get(loop) << endl;

  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      compile_file << "--" << endl;
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
      StringArray* preamble_path=fpga_elem->preamble_path;
      StringArray* preamble_files=fpga_elem->preamble_files;
      ACSIntArray* preamble_origin=fpga_elem->preamble_origin;
      StringArray* postamble_files=fpga_elem->postamble_files;
      StringArray* postamble_path=fpga_elem->postamble_path;
      ACSIntArray* postamble_origin=fpga_elem->postamble_origin;
	  
      if ((fpga_elem->usage_state==FPGA_USED) || (fpga_elem->usage_state==FPGA_RESERVED))
	{
	  compile_file << "-- ACS-generated Algorithm for FPGA " << fpga_no << endl
		       << "--" << endl;

	  // Start the preamble
	  for (int loop=0;loop<preamble_files->population();loop++)
	    {
	      if (DEBUG_ALGSUPPORT)
		printf("%s has origin %d\n",preamble_files->get(loop),
		       preamble_origin->query(loop));
	      compile_file << compile;
	      if (preamble_origin->query(loop)==0)
		compile_file << hw_path->retrieve_extended() << preamble_path->get(loop);
	      compile_file << preamble_files->get(loop) << endl;
	    }

	  // Ellicit algorithm files
	  if (fpga_elem->usage_state==FPGA_USED)
	    {
	      CoreList* sg_subset=fpga_elem->get_childcores_handle();
	      for (int sg_loop=0;sg_loop<sg_subset->size();sg_loop++)
		{
		  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_subset->elem(sg_loop);
		  if (fpga_core->acs_type==BOTH)
		    {
		      // CAUTION: May not need to examine for children, since the FPGA is
		      //           aware of all it's children?
		      for (int child_loop=0;
			   child_loop < (fpga_core->child_filenames)->population();
			   child_loop++)
			compile_file << compile  
				     << (fpga_core->child_filenames)->get(child_loop) 
				     << endl;
		      
		      // FIX: Extension should be obtained from the core
		      // FIX: Prohibit reliance on black_box status requiring lower
		      //      case name! This is only to satisfy coregen->xnf naming
		      //      conventions
		      if ((fpga_core->unique_component) &&  (fpga_core->declaration_flag))
			compile_file << compile << fpga_core->unique_name.str() << ".vhd" 
				     << endl;
		      else
			if (fpga_core->black_box==0)
			  compile_file << compile  
				       << fpga_core->name() << ".vhd" << endl;
			else
			  compile_file << compile 
				       << fpga_core->lc_name() << ".vhd" << endl;
		    }
		}
	      compile_file << compile << fpga_elem->retrieve_rootfilename() << ".vhd" << endl;
	    }

	  // Wrap-up with the postamble
	  for (int loop=0;loop<postamble_files->population();loop++)
	    {
	      if (DEBUG_ALGSUPPORT)
		printf("%s has origin %d\n",postamble_files->get(loop),
		       postamble_origin->query(loop));
	      compile_file << compile;
	      if (postamble_origin->query(loop)==0)
		compile_file << hw_path->retrieve_extended() << postamble_path->get(loop);
	      compile_file << postamble_files->get(loop) << endl;
	    }
	}
      else
	{
	compile_file << "-- Externally generated Algorithm for FPGA " << fpga_no << endl
		     << "--" << endl;
	  for (int loop=0;loop<preamble_path->population();loop++)
	    compile_file << compile 
			 << hw_path->retrieve_extended()
			 << preamble_path->get(loop) 
			 << preamble_files->get(loop) << endl;

	  for (int loop=0;loop<postamble_path->population();loop++)
	    compile_file << compile
			 << hw_path->retrieve_extended()
			 << postamble_path->get(loop) 
			 << postamble_files->get(loop) << endl;
      }

    }

  // Misc Wildforce files
  compile_file << "--" << endl
	       << "--  Additional architecture files" << endl
	       << "--" << endl;

  StringArray* misc_path=arch->misc_path;
  StringArray* misc_files=arch->misc_files;
  ACSIntArray* misc_origin=arch->misc_origin;
  for (int loop=0;loop<misc_files->population();loop++)
    {
      compile_file << compile;
      if (misc_origin->query(loop)==0)
	compile_file << hw_path->retrieve_extended() << misc_path->get(loop);
      compile_file << misc_files->get(loop) << endl;
    }
      
  ////////////////////
  simulate_file << "-----------------------------------------------------------------" 
		<< endl 
		<< "--" << endl
		<< "--  Copyright (C) 1996-1997 Annapolis Micro Systems, Inc." << endl 
		<< "--" << endl
		<< "-----------------------------------------------------------------" << endl
		<< "--  Description   : MTI V-System macro file used to analyze VHDL" << endl 
		<< "--                  files for the WF4 VHDL Simulator.  This macro" << endl
		<< "--                  file is to be used in conjunction with the" << endl
		<< "--                  Windows NT MTI V-System VHDL simulator," << endl
		<< "--                  version 4.6 or above." << endl
		<< "--" << endl
		<< "--  Date          : 11/12/96" << endl
		<< "--" << endl
		<< "--  Author        : Chris Esser, AMS" << endl
		<< "--" << endl;
  simulate_file << endl << endl
		<< "--" << endl
		<< "-- Automated by Sanders, a Lockheed Martin Company" << endl
		<< "-- Ken Smith" << endl
		<< "-- 2/1/99" << endl
		<< "--" << endl
		<< "---------------------------------------------------------"
		<< endl;
  simulate_file << "vsim systemconfig" << endl
		<< "force -freeze /wf4host/gen_boards__0/wf4board/u_pe0/u_core/u1/reset 1 0" 
		<< endl
		<< "force -freeze /wf4host/gen_boards__0/wf4board/u_pe0/u_core/u1/reset 0 100"
		<< endl;

  // Cleanup
  compile_file.close();
  simulate_file.close();
  delete []compile;
  delete []compile_fpga0;
  delete []root_filename;
}

////////////////////////////////////////////////////////////////////
// Export the necessary scripts to allow the developer to synthesize 
// the results
// FIX: For now this will be hardcoded for Synplicity's Synplify
//      and for the Annapolis Wildfire board and PE1
////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWexport_synthesis(Fpga* fpga_elem,
					 const int fpga_no)
{
  CoreList* sg_subset=fpga_elem->get_childcores_handle();

  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  ostrstream compile_filename;
  compile_filename << design_directory->retrieve_extended() 
		   << fpga_elem->retrieve_rootfilename() << ".prj" << ends;
  ofstream compile_file(compile_filename.str());

  compile_file << "# Synplify project for ACS generated graph " 
	       << root_filename << " for fpga number " << fpga_no << endl << endl;
  compile_file << "# Device options" << endl
	       << "set_option -technology " << fpga_elem->part_tech << endl
	       << "set_option -part " << fpga_elem->part_name << endl
	       << "set_option -package " << fpga_elem->part_pack << endl
	       << "set_option -speed_grade -" << fpga_elem->speed_grade << endl 
	       << endl;

  char* command=new char[MAX_STR];
  char* board_path=new char[MAX_STR];
  strcpy(command,"add_file -vhdl -lib ");
  compile_file << "# Architecture files" << endl;
  StringArray* archlib=arch->archsynthesis_libname;
  StringArray* archfiles=arch->archsynthesis_files;
  StringArray* archpath=arch->archsynthesis_path;
  for (int loop=0;loop<archlib->population();loop++)
    compile_file << command << archlib->get(loop) << " \"" << hw_path->retrieve_extended() 
		 << archpath->get(loop) << archfiles->get(loop) << "\"" << endl;

  StringArray* support_files=arch->archsupport_files;
  StringArray* support_path=arch->archsupport_path;
  for (int loop=0;loop<support_files->population();loop++)
    compile_file << command << "work \""
		 << hw_path->retrieve_extended() << support_path->get(loop) 
		 << support_files->get(loop) << "\"" << endl;

  StringArray* synthesis_libname=fpga_elem->synthesis_libname;
  StringArray* synthesis_path=fpga_elem->synthesis_path;
  StringArray* synthesis_files=fpga_elem->synthesis_files;
  for (int loop=0;loop<synthesis_libname->population();loop++)
    compile_file << command << synthesis_libname->get(loop) << " \""
		 << hw_path->retrieve_extended() << synthesis_path->get(loop)
		 << synthesis_files->get(loop) << "\"" << endl;

  StringArray* preamble_path=fpga_elem->preamble_path;
  StringArray* preamble_files=fpga_elem->preamble_files;
  ACSIntArray* preamble_origin=fpga_elem->preamble_origin;
  for (int loop=0;loop<preamble_files->population();loop++)
    {
      compile_file << command << "work \"";
      if (preamble_origin->query(loop)==0)
	compile_file << hw_path->retrieve_extended() << preamble_path->get(loop);
      compile_file << preamble_files->get(loop) << "\"" << endl;
    }

  compile_file << endl << "# ACS-generated files" << endl;
  for (int loop=0;loop<sg_subset->size();loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_subset->elem(loop);
      if ((fpga_core->acs_type==BOTH) && (fpga_core->black_box==0))
	if ((fpga_core->unique_component) &&  (fpga_core->declaration_flag))
	  compile_file << command << "work \"" << fpga_core->unique_name.str() << ".vhd" << endl;
	else
	  compile_file << command << "work \"" << fpga_core->name() << ".vhd\"" << endl;
    }
  compile_file << command << "work \"" << fpga_elem->retrieve_rootfilename() << ".vhd\"" << endl;


  StringArray* postamble_files=fpga_elem->postamble_files;
  StringArray* postamble_path=fpga_elem->postamble_path;
  ACSIntArray* postamble_origin=fpga_elem->postamble_origin;
  for (int loop=0;loop<postamble_files->population();loop++)
    {
      compile_file << command << "work \"";
      if (postamble_origin->query(loop)==0)
	compile_file << hw_path->retrieve_extended() << postamble_path->get(loop);
      compile_file << postamble_files->get(loop) << "\"" << endl;
    }
  

  compile_file << endl << "# Compilation/Mapping options" << endl
	       << "set_option -frequency 20.000" << endl
	       << "set_option -fanout_limit 100" << endl
	       << "set_option -maxfan_hard false" << endl
	       << "set_option -force_gsr true" << endl
	       << "set_option -disable_io_insertion false" << endl
	       << "set_option -xilinx_m1 true" << endl
	       << endl;

  compile_file << "#set result format/file last" << endl
	       << "project -result_format \"xnf\"" << endl
	       << "project -result_file \"" << fpga_elem->retrieve_rootfilename() << ".xnf\"" << endl
	       << endl;

  // Cleanup
  compile_file.close();
  delete []root_filename;
  delete []command;
  delete []board_path;
}


// Export relevant information to the Ptolemy star that will invoke
// the FPGAs
void ACSCGFPGATarget::HWexport_controlinfo(CoreList* smart_generators)
{
  // Allocate and assign standard filenames
  char* pe_filename=new char[MAX_STR];
  char* meminfo_filename=new char[MAX_STR];
  sprintf(pe_filename,"%sPeFileNames.txt",design_directory->retrieve_extended());
  sprintf(meminfo_filename,"%sMemInfo.txt",design_directory->retrieve_extended());

  // Open file streams
  ofstream pe_file(pe_filename);
  ofstream meminfo_file(meminfo_filename);

  // Fill static files
  // NOTE: Pe#'s are implicit
  pe_file << "1 fpga0.m68" << endl;
  for (int fpga_loop=1;fpga_loop<arch->fpga_count;fpga_loop++)
    {
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_loop);
      if (fpga_elem->usage_state==FPGA_USED)
	pe_file << 1 << " " << fpga_elem->retrieve_rootfilename() << ".m68" << endl;
      else
	pe_file << 0 << " blank.bin" << endl;
    }
  
  // Fill memory-related files
  int total=0;
  int worst_latency=0;
  int last_fpga=-1;
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      Port* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->port_active())
	{
	  Port_Timing* port_timing=mem_port->port_timing;
	  total+=port_timing->count;
	}
      
      // FIX: worst_latency is a factor related to an Fpga not memory,
      //      for now last_fpga will be the mem_loop (works for Annapolis model)
      if (mem_port->total_latency > worst_latency)
	{
	  worst_latency=mem_port->total_latency;
	  last_fpga=mem_loop;
	}
    }

  meminfo_file << last_fpga-1 << endl;
  meminfo_file << total << endl;
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      Port* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->port_active())
	{
	  Port_Timing* port_timing=mem_port->port_timing;
	  for (int loop=0;loop<port_timing->count;loop++)
	    if (port_timing->get_memtype(loop)==SOURCE)
	      {
		CoreList* src_list=mem_port->source_cores;
		ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) src_list->elem(port_timing->get_memid(loop));
		meminfo_file << mem_loop << " " 
			     << 1 << " "
			     << fpga_core->address_start << " " 
			     << port_timing->get_vectorlength(loop) << " "
			     << port_timing->get_majorbit(loop) << " "
			     << port_timing->get_bitlen(loop) << endl;
	      }
	    else if (port_timing->get_memtype(loop)==SINK)
	      {
		CoreList* snk_list=mem_port->sink_cores;
		ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) snk_list->elem(port_timing->get_memid(loop));
		meminfo_file << mem_loop << " "
			     << 0 << " "
			     << fpga_core->address_start << " " 
			     << port_timing->get_vectorlength(loop) << " "
			     << port_timing->get_majorbit(loop) << " "
			     << port_timing->get_bitlen(loop) << endl;
	      }
	}
    }

  // Close files
  pe_file.close();
  meminfo_file.close();

  // Cleanup
  delete []pe_filename;
  delete []meminfo_filename;
}

void ACSCGFPGATarget::HWannounce_vhdldone(void)
{
  int pid;
  int wait_stat=512;
  int* wait_ptr=&wait_stat;

  if ((pid=fork())==0)
    {
      char* vhdldone_command=new char[MAX_STR];
      char* environ_path=getenv("PTOLEMY");
      sprintf(vhdldone_command,"%s/src/domains/acs/utils/vhdl_done.tk",
	      environ_path);

      chdir(design_directory->retrieve());
      if (execlp("itkwish","itkwish",vhdldone_command,(char*) 0)==-1)
	printf("HWpause_for_vhdldone:Unable to spawn a wish interpreter\n");

      // Cleanup
      delete []vhdldone_command;
    }
  else
    {
      while (wait_stat != 0)
	wait (wait_ptr);
    }
}

void ACSCGFPGATarget::HWcleanup(void)
{
  delete arch;
  delete all_libraries;
  delete all_includes;
  delete hw_path;
  delete sim_path;
  delete core_path;
  delete design_directory;
  delete compile_directory;

  delete []err_message;

  // Purge smart generator lists
  delete smart_generators;
}




