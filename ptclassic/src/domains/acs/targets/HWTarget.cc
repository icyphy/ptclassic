static const char file_id[] = "HWTarget.cc";

/**********************************************************************
Copyright (c) 1999 Sanders, a Lockheed Martin Company
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
 Version: @(#)HWTarget.cc      1.0     06/16/99
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

  // Determine path for building resultatn VHDL code, if blank, use the design_directory
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

  printf("All output sent to %s\n",design_directory->retrieve());

  // Make sure the coregen is installed if vhdl is to be built
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

  // Assist the memory manager
  memory_monitor();

  // Set error string
  err_message=new char[MAX_STR];

  // Smart generator list initializations
  smart_generators=new SequentialList;

  // Identifier initialization
  free_netlist_id=1;
  free_acsid=0;
  free_act=0;
  free_signal=0;

  // Accumulated list initializations
  all_libraries=new StringArray;
  all_includes=new StringArray;

  //
  // Hardcoded hardware definitions for now
  //
  arch=new Arch(NUM_FPGAS,NUM_MEMS);
  arch->set_fpga();
  arch->set_memory();
//  arch->print_arch("Allocated architecture:\n");
}


int ACSCGFPGATarget::HWalg_query()
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
	  smart_generators->append((Pointer) fpga_core);
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

  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);

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
      fpga_core->acs_origin=USER_GENERATED;

      // Determine technology specifications given by user
      // Alloc smart generator memories
      // Query and set pipe states
      // Query memory assignment info
      int rc_sginit=fpga_core->sg_initialize(design_directory->retrieve_extended(),
					     ALGORITHM,&free_acsid);
      if (rc_sginit==0)
	return(0);

      if (fpga_core->unique_component)
	{
	  printf("fpga_core %s is unique\n",fpga_core->comment_name());
	  HWdup_component(fpga_core);
	}

      // Translate star parameters
      HWsg_queryprec(fpga_core);
    }


  // THIRD PASS:
  // Also determine technology specifications (i.e., domain, technology, 
  // language).  Iterate over the smart generator ports to determine 
  // connectivity and output BW.
  // NOTE: The remoteport_id concept only works if MultiOut ports are
  //       indeed flattened into Output ports!
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      
      // ASSUMPTION:There should only be one(?) connected graph
      // Only connect relevant stars
      if (fpga_core->acs_domain==HW)
	{
	  int rc=HWsg_assign_pins(fpga_core);
	  if (rc==0)
	    return(0);
	}

    } //  for (int loop=1;loop<=smart_generators->size();loop++)


  // FOURTH PASS:
  // Remove all SW domain cores, since all HW connections have been established
  SequentialList* tmp_gens=new SequentialList;
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (fpga_core->acs_domain==HW)
	tmp_gens->append((Pointer) fpga_core);
    }
  delete smart_generators;
  smart_generators=tmp_gens;

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
void ACSCGFPGATarget::HWsg_queryprec(ACSCGFPGACore* fpga_core)
{
  // Allocate local storage
  SequentialList* input_list=new SequentialList;
  SequentialList* output_list=new SequentialList;
  int pin_count=0;
  
  // Obtain parameter name info from each smart generator
  fpga_core->sg_param_query(input_list,output_list);

  // Retrieve precision from Ptolemy states and assign precisions
  HWsg_updateprec(fpga_core,input_list,pin_count,"InputPrecision","LockInput");
  pin_count+=(input_list->size())/2;
  HWsg_updateprec(fpga_core,output_list,pin_count,"OutputPrecision","LockOutput");

  // Treat all precision information as being locked and update delays
  fpga_core->sg_resources(LOCKED);
  fpga_core->sg_delay_query();

  // Cleanup
  delete input_list;
  delete output_list;
}


/////////////////////////////////////////////////////////////////////////////
// Query the ACS precision field and set the respective pin's precision
// if defaults, then retrieve precisions from fixed precision parameter field
/////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWsg_updateprec(ACSCGFPGACore* fpga_core,
				      SequentialList* pin_list,
				      int pin_count,
				      const char* fixed_statename,
				      const char* lock_statename)
{
  Pin* pins=fpga_core->pins;

  if (DEBUG_PTPRECISION)
    printf("Interpreting precisions for sg %s\n",fpga_core->comment_name());

  // ASSUMPTION:ACS precisions have two entries => (loop+=2)
  for (int loop=0;loop<pin_list->size();loop+=2)
    {
      // Obtain major bit
      char* mbit_field=(char*) pin_list->elem(loop);
      int majorbit=fpga_core->intparam_query(mbit_field);

      // Obtain bit length
      char* bitlen_field=(char*) pin_list->elem(loop+1);
      int bitlen=fpga_core->intparam_query(bitlen_field);

      if (DEBUG_PTPRECISION)
	printf("ACS Ptolemy precision fields are (%d,%d)\n",majorbit,bitlen);

      // Obtain user lock directive
      int lock_state=fpga_core->intparam_query(lock_statename);

      // FIX: FixSim core use of YES/NO implies 1/0 interpretation.
      if (((majorbit==0) && (bitlen==0)) || (lock_state==1))
	{
	  // ACS precisions not set, retrieve from Ptolemy precisions
	  State* precision_state=fpga_core->stateWithName(fixed_statename);
	  if (precision_state!=NULL)
	    {
	      StringList current_string=precision_state->currentValue();
	      const char* current_char=(const char *)current_string;
	  
	      Fix infix=Fix((const char*) current_char);
	      majorbit=infix.intb();
	      bitlen=infix.len();
	    }
	  else
	    {
	      // If there is no Ptolemy specified precision then zero it out
	      majorbit=0;
	      bitlen=0;
	    }
	}

      // Set precision
      pins->set_wordlock(pin_count,lock_state);
      if (DEBUG_PTPRECISION)
	printf("pin %s, precision set to (%d,%d)\n",
	       pins->query_pinname(pin_count),
	       majorbit,
	       bitlen);
	       
      pins->set_precision(pin_count++,majorbit,bitlen,LOCKED);
    }
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
      netlist_id=free_netlist_id++;
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
void ACSCGFPGATarget::HWassign_devices()
{
  printf("Assigning memories....");
  HWassign_memory();
  printf("Done.\n");

  printf("Assign fpgas....");
  HWassign_fpga();
  printf("Done.\n");

  if (DEBUG_ASSIGNMENTS)
    HWtest_assignments();
}

void ACSCGFPGATarget::HWtest_assignments()
{
  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      Fpga* its_fpga=arch->get_fpga_handle(fpga_no);
      printf("Fpga %d has the following children:\n",fpga_no);
      its_fpga->print_children();
    }
}

/////////////////////////////////////////////////////////
// Watch for unassigned smart generators and default them
// FIX:A rudimentary algorithm should be added?
/////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWassign_fpga(void)
{
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      int local_device=-1;
      if (DEBUG_ASSIGNMENTS)
	printf("Core %s is assigned to acs_device %d\n",
	       fpga_core->comment_name(),
	       fpga_core->acs_device);
      
      if (fpga_core->acs_type==BOTH)
	{
	  /////////////////////////////////////////
	  // Evaluate core assignments to the fpgas
	  /////////////////////////////////////////
	  if (fpga_core->acs_device<=UNASSIGNED)
	    {
	      fpga_core->acs_device=1;
	      printf("HWassign_fpga:Warning, core %s, not assigned to an fpga, will assign to 1\n",
		     fpga_core->comment_name());
	    }
	  if (fpga_core->acs_device > NUM_FPGAS)
	    {
	      fpga_core->acs_device=1;
	      printf("HWassign_fpga:Warning, core %s, assigned to fictitious fpga, will assign to 1\n",
		     fpga_core->comment_name());
	      printf("Available Fpgas are %d to %d\n",0,arch->fpga_count-1);
	    }
      
	  //////////////////////////////////////////////////
	  // Notify fpga that it will be used in this design
	  // FIX: Should do a search for a non-reserved fpga
	  //////////////////////////////////////////////////
	  int rc=arch->activate_fpga(fpga_core->acs_device);
	  if (!rc)
	    {
	      printf("HWassign_fpga:Warning, core %s, assigned to reserved fpga, will assign to 1\n",
		     fpga_core->comment_name());
	      fpga_core->acs_device=1;
	      arch->activate_fpga(fpga_core->acs_device);
	    }
	  local_device=fpga_core->acs_device;

	  ////////////////////////////////////////////////////////////////
	  // To ease searching, assign the core handle to its respectively
	  // assigned FPGA
	  ////////////////////////////////////////////////////////////////
	  Fpga* its_fpga=arch->get_fpga_handle(local_device);
	  its_fpga->set_child(fpga_core);
	}
    }
}

/////////////////////////////////////////////////////////////////////////////
// Assign Source and Sink stars to memory ports based on specified addresses.
// Issue a firing schedule to Source stars on a first-come-first-serve basis.
// Allocate star pointers to each memory port for later access.
/////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWassign_memory()  
{
  // Assign an array to depict the total number of times a port is 
  // used for a given sequence
  seqlen=0;
  int* port_hits=new int[arch->mem_count];
  for (int loop=0;loop<arch->mem_count;loop++)
    port_hits[loop]=0;

  // Assign memory as dictated by source addresses and port addresses
  // Set initial node activations on a first-come-first-served basis
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);

      // For now only schedule sources 
      if ((fpga_core->acs_type==SOURCE) || (fpga_core->acs_type==SINK))
	{
	  int reserved_mem=0;
	  if (fpga_core->acs_device != UNASSIGNED)
	    {
	      MemPort* mem_port=arch->get_memory_handle(fpga_core->acs_device);
	      if (mem_port->portuse==MEMORY_RESERVED)
		reserved_mem=1;
	    }
	  if ((fpga_core->acs_device!=UNASSIGNED) && (!reserved_mem))
	    {
	      if (DEBUG_ARCH)
		printf("Assigning %s to :\n",fpga_core->comment_name());
	    }
	  else
	    {
	      // Automatically assign this component to a memory
	      // FIX: Only a first-available approach used

	      if (DEBUG_ASSIGNMENTS)
		{
		  if (fpga_core->acs_device == UNASSIGNED)
		    printf("Memory not assigned, will make an assignment\n");
		  if (reserved_mem)
		    printf("Memory assigned to a reserved memory, will make new assignment\n");
		}

	      // Identify memory ports in use as dictated by addresses
	      int mem_loop=0;
	      int assigned=0;
	      while ((mem_loop<arch->mem_count) && (!assigned))
		{
		  MemPort* mem_port=arch->get_memory_handle(mem_loop);
		  if (mem_port->portuse != MEMORY_RESERVED)
		    {
		      if ((fpga_core->address_start < mem_port->addr_hi) &&
			  (fpga_core->address_start >= mem_port->addr_lo))
			{
			  if (DEBUG_ASSIGNMENTS)
			    printf("addr=%d, fits under port %d of address range (%d,%d)\n",
				   fpga_core->address_start,
				   mem_loop,
				   mem_port->addr_lo,
				   mem_port->addr_hi);
			  
			  // Associate this memory with the star
			  fpga_core->acs_device=mem_loop;
			  assigned=1;
			}
		    }
		  mem_loop++;
		} //  while ((mem_loop<arch->mem_count) && (!assigned))
	    }
		
	  ///////////////////////////////////////////////////////
	  // Inform the memory port of this star and schedule it.
	  // SINK stars will be assigned during scheduling, but
	  // at least they are tracked here.
	  ///////////////////////////////////////////////////////
	  MemPort* mem_port=arch->get_memory_handle(fpga_core->acs_device);
	  if (fpga_core->acs_type==SOURCE)
	    mem_port->assign_srccore(fpga_core);
	  port_hits[fpga_core->acs_device]++;


	  ///////////////////////////////////////////////////////////////
	  // Since this memory will be used, its controlling FPGA will be 
	  // contain (of course) controlling circuitry.
	  ///////////////////////////////////////////////////////////////
	  arch->activate_fpga(fpga_core->acs_device);


	  //////////////////////////////////////////////////////////////
	  // To ease searching, assign the core handle to its controller
	  // FPGA
	  //////////////////////////////////////////////////////////////
	  Fpga* its_fpga=arch->get_fpga_handle(mem_port->controller_fpga);
	  its_fpga->set_child(fpga_core);

	} // if (fpga_core->acs_type==SOURCE) || (fpga_core->acs_type==SINK))
    } // for (int loop=1;loop<=smart_generators->size();loop++)

  // Determine sequence length
  for (int loop=0;loop<arch->mem_count;loop++)
    if (port_hits[loop]>seqlen)
      seqlen=port_hits[loop];

  // Adjust sequence length based on memory delays
  // ASSUMPTION:Delays are the same on all memory models
  // FIX:The sequence adjustment should be the difference of skews!!
  //     Long term solution will obviate this, so ok for now.
  MemPort* mem_port=arch->get_memory_handle(0);
  seqlen+=mem_port->read_skew;
  seqlen+=mem_port->write_skew;

  if (DEBUG_SCHEDULER)
    printf("seqlen=%d\n",seqlen);

  // Cleanup
  delete []port_hits;
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
// Generates a single rate schedule for the algorithm given a fixed set of 
// input times.
// TODO: (1) Need to preclude constants or other non-timed functions from
//       affecting the scheduling.  Trap on size?
///////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWunirate_sched()
{
  // Recalculate pipe delay for phase_dependent cores
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (fpga_core->phase_dependent)
	{
//	  printf("before, pipe_delay=%d\n",fpga_core->pipe_delay);
	  fpga_core->pipe_delay*=seqlen;
//	  printf("after, pipe_delay=%d\n",fpga_core->pipe_delay);
	}
    }

  // Now iteratively schedule
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
		    
		    // FIX: For now no bidirectionals drive this
		    // ASSUMPTION: Only one input node!!
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
		    if (unsched_core->delay_offset)
		      unsched_core->act_output+=unsched_core->delay_offset;
		    
		    if (unsched_core->acs_type==SINK)
		      {
			MemPort* mem_port=arch->get_memory_handle(unsched_core->acs_device);
			mem_port->assign_snkcore(unsched_core,unsched_core->act_output);
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
	  } // for (int loop=1;loop<=smart_generators->size();loop++)
	// DEBUG
	runaway++;
	if (runaway>RUNAWAY_SCHEDULER)
	  {
	    done=1;
	    if (DEBUG_SCHEDULER)
	      printf("HWunirate_sched:Runaway scheduler!\n");
	  }
      } // if (!HWall_scheduled())


  // For algorithmic smart generators, their effective delay
  // is based on the sequence length rather than the system clock.
  // Tag their CEs for sequencer phase hookups
  // ASSUMPTION: Algorithm delays imply a different rate.
  //             Shared memories are always used.
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* alg_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (DEBUG_SCHEDULER)
	{
	  printf("Examining smart generator %s\n",alg_core->comment_name());
	  Pipealign* core_pipe=alg_core->pipe_alignment;
	  core_pipe->print_delays();
	}
	    
      if ((alg_core->alg_delay > 0) || (alg_core->phase_dependent))
	{
	  Pin* alg_pins=alg_core->pins;

	  // Convert the pintypes for the resolver
	  int ce_pin=alg_pins->retrieve_type(INPUT_PIN_CE);

	  //FIX: Once pin type is split into class and defaults then
	  //     remove this kludgy trap
	  if (ce_pin==-1)
	    ce_pin=alg_pins->retrieve_type(INPUT_PIN_AH);
	    
	  alg_pins->set_pintype(ce_pin,INPUT_PIN_PHASE);

	  // Assign the appropriate phase activation
	  // As determined by the pipe-aligned source as a function of the ring counter
	  int phase=alg_core->act_input % seqlen;
/*
	  if (alg_core->delay_offset)
	    phase=(alg_core->act_output-alg_core->pipe_delay+alg_core->delay_offset) % seqlen;
	  else
	    phase=(alg_core->act_output-alg_core->pipe_delay) % seqlen;
	    */

//	  printf("dataact=%d, seqlen=%d, phase=%d\n",alg_core->act_output,seqlen,phase);
//	  phase--;
	  if (phase<0)
	    phase=seqlen;
	  alg_pins->set_pinpriority(ce_pin,phase);

	  if (DEBUG_SCHEDULER)
	    printf("Smart generator %s is phase-dependent and will activate on phase %d\n",
		   alg_core->comment_name(),
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
      target_device=src_core->acs_outdevice;
      if (DEBUG_DELAYS)
	printf("Adding delays to an IoPort, will target for device %d\n",
	       target_device);
    }

  // Add a constructor engine.  For now all delays will be targeted for the
  // same device as the source core
  /*
  construct=new Sg_Constructs(&free_acsid,
			      design_directory->retrieve_extended(),
			      arch->get_fpga_handle(target_device),
			      target_device);
			      */
  construct->set_targetdevice(arch->get_fpga_handle(target_device),target_device);

  ACSCGFPGACore* prev_core=NULL;
  int prev_opin=UNASSIGNED;
  int sign_convention=src_core->sign_convention;
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
      delay_core=construct->add_sg("ACS","Delay","CGFPGA",
				   BOTH,sign_convention,smart_generators);
      delay_ids->add(delay_core->acs_id);
      delay_core->act_output=src_core->act_output+gen_delay;
      delay_core->pipe_delay=1;
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
					LOCKED);
      (delay_core->pins)->set_precision(delay_opin,
					major_bit,
					bit_length,
					LOCKED);
      
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








void ACSCGFPGATarget::HWio_sched()
{
  /////////////////////////////////////////////////////////////////
  // Examine each memory port
  /////////////////////////////////////////////////////////////////
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      MemPort* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->portuse==MEMORY_USED) 
	{
	  if (DEBUG_IOSCHEDULER)
	    printf("HWio_sched::Memory %d is being used as a source/sink\n",
		   mem_loop);

	  /////////////////////////////////////
	  // Determine source and sink pointers
	  /////////////////////////////////////
	  SequentialList* source_stars=mem_port->source_stars;
	  SequentialList* sink_stars=mem_port->sink_stars;
	  int src_count=source_stars->size();
	  int snk_count=sink_stars->size();
	  
	  if (snk_count > 0)
	    {
	      ///////////////////////////////////////////////////
	      // Reschedule sinks should there be any read delays
	      ///////////////////////////////////////////////////
	      ACSIntArray* read_block=new ACSIntArray(seqlen,FALSE);
	      for (int source_loop=1;source_loop<=src_count;source_loop++)
		{
		  ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) source_stars->elem(source_loop);
		  int addr_time=fpga_core->act_input;
		  read_block->set(addr_time,TRUE);
		  if (mem_port->read_skew != 0)
		    read_block->set(addr_time+mem_port->read_skew,TRUE);
		  if (DEBUG_IOSCHEDULER)
		    printf("Reads block times %d and %d\n",
			   addr_time,
			   addr_time+mem_port->read_skew);
		}
	      
	      // FIX: Assuming that (for writes) address and data are in sync
	      for (int sink_loop=1;sink_loop<=snk_count;sink_loop++)
		{
		  ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) sink_stars->elem(sink_loop);
		  int addr_time=fpga_core->act_input;
		  int next_seqlen=FALSE;
		  if (addr_time >= seqlen)
		    next_seqlen=TRUE;
		  else
		    if (read_block->query(addr_time)==FALSE)
		      {
			read_block->set(addr_time,TRUE);
			if (DEBUG_IOSCHEDULER)
			  printf("Write is not blocked at time %d\n",
				 addr_time);
		      }
		    else
		      {
			if (DEBUG_IOSCHEDULER)
			  printf("Write at time %d is currently blocked\n",
				 addr_time);
			next_seqlen=TRUE;
			for (int srch_loop=addr_time+1;srch_loop<seqlen;srch_loop++)
			  if (read_block->query(srch_loop)==FALSE)
			    {
			      next_seqlen=FALSE;
			      fpga_core->act_input=srch_loop;
			      fpga_core->act_output=srch_loop;
			      Pipealign* sink_pipe=fpga_core->pipe_alignment;
			      sink_pipe->calc_netdelays(fpga_core->act_output);
			      
			      if (DEBUG_IOSCHEDULER)
				printf("Adjusting sink core firing time to %d\n",srch_loop);
			      read_block->set(srch_loop,TRUE);
			      break; // srch_loop
			    }
		      }
	      
		  if (next_seqlen)
		    {
		      if (DEBUG_IOSCHEDULER)
			printf("HWio_sched:Unable to fit in 1st seqlen\n");
		      
		      for (int srch_loop=0;srch_loop<seqlen;srch_loop++)
			if (read_block->query(srch_loop)==FALSE)
			  {
			    // Ensure that it occurs AFTER its current
			    // algorithmic schedule time!  Otherwise delays
			    // would not be added as the sourcer fires AFTER
			    // the seqlen+srch_loop time.
			    int new_time=seqlen+srch_loop;
			    while (new_time < fpga_core->act_input)
			      new_time+=seqlen;
			    
			    // FIX:Address and data may not always be unskewed
			    fpga_core->act_input=new_time;
			    fpga_core->act_output=new_time;
			    Pipealign* sink_pipe=fpga_core->pipe_alignment;
			    sink_pipe->calc_netdelays(fpga_core->act_output);
			    
			    if (DEBUG_IOSCHEDULER)
			      printf("Adjusting sink core firing time to %d\n",
				     fpga_core->act_output);
			    mem_port->total_latency=fpga_core->act_output;
			    read_block->set(srch_loop,TRUE);
			    
			    break; // srch_loop
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
	  mem_port->init_pt(seqlen);
	  for (int src_loop=1;src_loop<=src_count;src_loop++)
	    {
	      ACSCGFPGACore* src_sg=(ACSCGFPGACore*) 
		source_stars->elem(src_loop);
	      int addr_act=src_sg->act_input;

	      // Was if, converted to while (of hand)
	      while (addr_act >= seqlen)
		addr_act-=seqlen;
	      mem_port->add_pt(addr_act,SOURCE);
	      if (DEBUG_IOSCHEDULER)
		printf("Source address set to activate at %d in sequence\n",addr_act);
	    }
	  for (int snk_loop=1;snk_loop<=snk_count;snk_loop++)
	    {
	      ACSCGFPGACore* snk_sg=(ACSCGFPGACore*) sink_stars->elem(snk_loop);
	      int addr_act=snk_sg->act_input;
	      while (addr_act >= seqlen)
		addr_act-=seqlen;
	      mem_port->add_pt(addr_act,SINK);
	      if (DEBUG_IOSCHEDULER)
		printf("Sink address set to activate at %d in sequence\n",
		       addr_act);
	    }
	} // if ((mem_port->portuse==MEMORY_USED) && ...
    } //  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
}




int ACSCGFPGATarget::HWsynth_pipealigns(void)
{
  if (DEBUG_PIPESYNTH)
    printf("\n\nHWsynth_pipealigns engaged\n");
  
  int sg_count=smart_generators->size();
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
      Pin* src_pins=(src_core->pins);
      ACSIntArray* opins=src_pins->query_onodes();

      if (DEBUG_PIPESYNTH)
	printf("Evaluating source %s for adding delays\n",
	       src_core->comment_name());
      
      // Add delays if necessary
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
	      for (int dloop=0;dloop<delay_count;dloop++)
		{
		  int dest_id=src_dest_ids->query(dloop);
		  ACSCGFPGACore* dest_sg=HWfind_star(dest_id);

		  ACSIntArray* srcpin_delay_reqs=srcpin_delays->delay_requirements;
		  ACSIntArray* srcpin_delay_pins=srcpin_delays->dest_pins;
		  if (DEBUG_PIPESYNTH)
		    printf("%d delays to %s on pin %d\n",
			   srcpin_delay_reqs->query(dloop),
			   dest_sg->comment_name(),
			   srcpin_delay_pins->query(dloop));
		  HWadd_delayto(src_core,
				dest_sg,
				opin,
				UNKNOWN,
				srcpin_delay_pins->query(dloop),
				UNKNOWN,
				srcpin_delay_reqs->query(dloop));
		}
	    }
	}
    }

  /*
  for (int loop=0;loop<new_delays->population();loop++)
    if (new_delays->query(loop)>0)
      {
	// This star needs to be delayed
	ACSCGFPGACore* src_core=
	  (ACSCGFPGACore *)src_cores->elem(loop+1);
	HWadd_delayto(src_core,
		      unsched_core,
		      new_delays->query(loop));
      } // if (new_delays->query(loop) > 0)
      */
  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////////////
// Should become a star method
// FIX: Need to make architecture independent as well
// FIX: Sink and delay counters should be replicated on a per-sink basis!!!
///////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWunirate_sequencer(Fpga* fpga_elem, const int fpga_no)
{
  //
  // Generate some local handles to frequented structures
  //
  Sequencer* sequencer=fpga_elem->sequencer;
  ACSCGFPGACore* seq_core=sequencer->seq_sg;
  Pin* seq_pins=seq_core->pins;

  // Determine if this fpga will be sourcing and/or sinking
  int total_srcs=0;
  int total_snks=0;
  for (int mem_loop=0;mem_loop<fpga_elem->mem_count;mem_loop++)
    if ((fpga_elem->mem_connected)->query(mem_loop)==MEMORY_AVAILABLE)
      {
	MemPort* mem_port=arch->get_memory_handle(mem_loop);
	total_srcs+=(mem_port->source_stars)->size();
	total_snks+=(mem_port->sink_stars)->size();
      }

  // FIX:Need to switch amongst language models
  VHDL_LANG lang;

  //
  // Define common external signals
  //
  if (DEBUG_SEQUENCER)
    printf("Generating seq_ext_signals for fpga %d\n",fpga_no);
  Pin* seq_ext_signals=new Pin;
  seq_ext_signals->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  seq_ext_signals->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET);
  seq_ext_signals->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL);
  seq_ext_signals->add_pin("Alg_Start",0,1,STD_LOGIC,OUTPUT_PIN_START);
  seq_ext_signals->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK);
  seq_ext_signals->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  seq_ext_signals->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);
  seq_ext_signals->add_pin("ADDR_CE",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCE);
  seq_ext_signals->add_pin("ADDR_CLR",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCLR);
  seq_pins->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  seq_pins->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET);
  seq_pins->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL);
  seq_pins->add_pin("Alg_Start",0,1,STD_LOGIC,OUTPUT_PIN_START);
  seq_pins->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK);
  seq_pins->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  seq_pins->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);
  seq_pins->add_pin("ADDR_CE",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCE);
  seq_pins->add_pin("ADDR_CLR",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCLR);

  //
  // Add fpga interconnect pins
  //
  int right_port=UNASSIGNED;
  int left_port=UNASSIGNED;
  if (fpga_no != 4)
    {
      int enable_pin=seq_ext_signals->add_pin("Right_OE",0,36,STD_LOGIC,
					      OUTPUT_PIN_INTERCONNECT_ENABLE);
      int enable_pin2=seq_pins->add_pin("Right_OE",0,36,STD_LOGIC,
					OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_ext_signals->set_pinpriority(enable_pin,1);
      seq_pins->set_pinpriority(enable_pin2,1);
      right_port=fpga_elem->query_mode(fpga_no+1);
    }
  if (fpga_no != 1)
    {
      int enable_pin=seq_ext_signals->add_pin("Left_OE",0,36,STD_LOGIC,
					      OUTPUT_PIN_INTERCONNECT_ENABLE);
      int enable_pin2=seq_pins->add_pin("Left_OE",0,36,STD_LOGIC,
					OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_ext_signals->set_pinpriority(enable_pin,0);
      seq_pins->set_pinpriority(enable_pin2,0);
      left_port=fpga_elem->query_mode(fpga_no-1);
    }
    
  if (DEBUG_SEQUENCER)
    {
      printf("right_port=%d, left_port=%d\n",right_port,left_port);

      if (right_port!=UNASSIGNED)
	printf("fpga %d is connected to fpga %d as a %d\n",
	       fpga_no,
	       fpga_no+1,
	       right_port);
      if (left_port!=UNASSIGNED)
	printf("fpga %d is connected to fpga %d as a %d\n",
	       fpga_no,
	       fpga_no-1,
	       left_port);
    }

  if (DEBUG_SEQUENCER)
    {
      printf("\n\nDumping ioport timing info\n");
      fpga_elem->mode_dump();
    }

  //
  // Establish pins for any source counter
  //
  char* source_carry_name=new char[MAX_STR];
  if (total_srcs)
    {
      ACSCGFPGACore* src_add=sequencer->src_add;
      Pin* srcadd_pins=src_add->pins;
      int port_id=srcadd_pins->retrieve_type(OUTPUT_PIN_SRC_WC);
      int major_bit=srcadd_pins->query_majorbit(port_id);
      int bitlen=srcadd_pins->query_bitlen(port_id);
      seq_ext_signals->add_pin("SRC_Carry",major_bit,bitlen,STD_LOGIC,INPUT_PIN_SRC_WC);
      seq_ext_signals->add_pin("SRC_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SRC_MUX);
      seq_ext_signals->add_pin("SRC_CE",0,1,STD_LOGIC,OUTPUT_PIN_SRC_CE);
      seq_pins->add_pin("SRC_Carry",major_bit,bitlen,STD_LOGIC,INPUT_PIN_SRC_WC);
      seq_pins->add_pin("SRC_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SRC_MUX);
      seq_pins->add_pin("SRC_CE",0,1,STD_LOGIC,OUTPUT_PIN_SRC_CE);
      sprintf(source_carry_name,"SRC_Carry(%d)",bitlen-1);
    }

  //
  // Establish pins for any sink and delay counters
  //
  char* sink_carry_name=new char[MAX_STR];
  char* delay_carry_name=new char[MAX_STR];
  if (total_snks)
    {
      ACSCGFPGACore* snk_adder=sequencer->snk_add;
      Pin* snkadd_pins=snk_adder->pins;
      int snk_id=snkadd_pins->retrieve_type(OUTPUT_PIN_SNK_WC);
      int mbit=snkadd_pins->query_majorbit(snk_id);
      int blen=snkadd_pins->query_bitlen(snk_id);
      seq_ext_signals->add_pin("SNK_CE",0,1,STD_LOGIC,OUTPUT_PIN_SNK_CE);
      seq_ext_signals->add_pin("SNK_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SNK_MUX);
      seq_ext_signals->add_pin("SNK_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_SNK_WC);
      seq_pins->add_pin("SNK_CE",0,1,STD_LOGIC,OUTPUT_PIN_SNK_CE);
      seq_pins->add_pin("SNK_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SNK_MUX);
      seq_pins->add_pin("SNK_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_SNK_WC);
      sprintf(sink_carry_name,"SNK_Carry(%d)",blen-1);

      ACSCGFPGACore* delay_adder=sequencer->delay_add;
      Pin* deladd_pins=delay_adder->pins;
      int del_id=deladd_pins->retrieve_type(OUTPUT_PIN_DELAY_WC);
      mbit=deladd_pins->query_majorbit(del_id);
      blen=deladd_pins->query_bitlen(del_id);
      seq_ext_signals->add_pin("Delay_CE",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_CE);
      seq_ext_signals->add_pin("Delay_MUX",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_MUX);
      seq_ext_signals->add_pin("Delay_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_DELAY_WC);
      seq_pins->add_pin("Delay_CE",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_CE);
      seq_pins->add_pin("Delay_MUX",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_MUX);
      seq_pins->add_pin("Delay_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_DELAY_WC);
      sprintf(delay_carry_name,"Delay_Carry(%d)",blen-1);
    }
	      
  Pin* seq_data_signals=NULL;

  if (DEBUG_SEQUENCER)
    printf("Generating seq_ctrl_signals\n");
  Pin* seq_ctrl_signals=new Pin;
  seq_ctrl_signals->add_pin("Go_Addr",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Wait_AlgFin",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Addr_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Ring_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Go_Ring",0,1,STD_LOGIC,INTERNAL_PIN);
  
  // Word counter relevant, reduced if not applicable by synthesis
  seq_ctrl_signals->add_pin("Go_WC_SRC",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_WC_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_Addr_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_Ring_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Go_WC_SNK",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_WC_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_Addr_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_Ring_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_OK",0,1,STD_LOGIC,INTERNAL_PIN);

  Pin* seq_constant_signals=new Pin;
  seq_constant_signals->add_pin("VCC",1.0,STD_LOGIC,OUTPUT_PIN_VCC);
  seq_constant_signals->add_pin("GND",1.0,STD_LOGIC,OUTPUT_PIN_GND);



  // Connectivity model
  ostrstream preamble_expr;
  ostrstream default_expr;
  ostrstream process_expr;

  /////////////////////////////
  // State machine declarations
  /////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating state machine states\n");
  StringArray* master_states=new StringArray;
  master_states->add("Init_State");
  master_states->add("Wait_For_Mem");
  master_states->add("Wait_For_Finish");
  master_states->add("End_State");
  preamble_expr << "--Master state machine states" << endl;
  preamble_expr << lang.type_def("Master_States",master_states);
  preamble_expr << endl;

  StringArray* addr_states=new StringArray;
  addr_states->add("Init_State");
  addr_states->add("Clear_Addr");
  addr_states->add("Enable_Addr");
  addr_states->add("Wait_For_Finish");
  preamble_expr << "--Address state machine states" << endl;
  preamble_expr << lang.type_def("Addr_States",addr_states);
  preamble_expr << endl;

  StringArray* ring_states=new StringArray;
  ring_states->add("Init_State");
  char* state_name=new char[MAX_STR];
  char* sig_name=new char[MAX_STR];
  for (int loop=0;loop<seqlen;loop++)
    {
      sprintf(state_name,"Phase%d",loop);
      sprintf(sig_name,"%s_Go",state_name);
      int phase_pin=seq_ext_signals->
	add_pin(sig_name,0,1,STD_LOGIC,OUTPUT_PIN_PHASE);
      seq_pins->add_pin(sig_name,0,1,STD_LOGIC,OUTPUT_PIN_PHASE);
      seq_ext_signals->set_pinpriority(phase_pin,loop);
      seq_pins->set_pinpriority(phase_pin,loop);
      ring_states->add(state_name);
    }
  delete []state_name;
  delete []sig_name;
  
  preamble_expr << "--Ring state machine states" << endl;
  preamble_expr << lang.type_def("Ring_States",ring_states);
  preamble_expr << endl;

  StringArray* src_wc_states=new StringArray;
  src_wc_states->add("Init_State");
  src_wc_states->add("Clear_WC");
  src_wc_states->add("Prep_WC");
  preamble_expr << "-- Source WC state machine states" << endl;
  preamble_expr << lang.type_def("SRCWC_States",src_wc_states);
  preamble_expr << endl;

  StringArray* snk_wc_states=new StringArray;
  snk_wc_states->add("Init_State");
  snk_wc_states->add("Clear_WC");
  snk_wc_states->add("Wait_For_Delay");
  snk_wc_states->add("Prep_WC");
  preamble_expr << "-- Sink WC state machine states" << endl;
  preamble_expr << lang.type_def("SNKWC_States",snk_wc_states);
  preamble_expr << endl;


  /////////////////////////////
  // State machine declarations
  /////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating state machine declarations\n");

  preamble_expr << "--State machine signals" << endl;
  preamble_expr << lang.signal("Master_State","Master_States");
  preamble_expr << lang.signal("Next_Master_State","Master_States");
  preamble_expr << lang.signal("Addr_State","Addr_States");
  preamble_expr << lang.signal("Next_Addr_State","Addr_States");
  preamble_expr << lang.signal("Ring_State","Ring_States");
  preamble_expr << lang.signal("Next_Ring_State","Ring_States");
  preamble_expr << lang.signal("SRCWC_State","SRCWC_States");
  preamble_expr << lang.signal("Next_SRCWC_State","SRCWC_States");
  preamble_expr << lang.signal("SNKWC_State","SNKWC_States");
  preamble_expr << lang.signal("Next_SNKWC_State","SNKWC_States");
  preamble_expr << endl;


  //////////////////////////////
  // Local variable declarations
  //////////////////////////////

  ///////////////////////////////
  // Generate default assignments
  ///////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating default assignments\n");

  // Master kickoff
  if (total_srcs)
    default_expr << "-- Source word counter control" << endl
		 << lang.equals("SRC_CE","SRC_Addr_Control") 
		 << lang.or("SRC_Ring_Control")
		 << lang.or("SRC_WC_Control")
		 << lang.end_statement << endl << endl;
  if (total_snks)
    default_expr << "-- Sink word counter control" << endl
		 << lang.equals("SNK_CE","SNK_Addr_Control") 
		 << lang.or("(SNK_Ring_Control") << lang.and("SNK_OK)")
		 << lang.or("SNK_WC_Control")
		 << lang.end_statement << endl << endl;

  default_expr << "-- Addr counter control" << endl
	       << lang.equals("ADDR_CE","Addr_Addr_CE");
  if ((total_snks) & (!total_srcs))
    default_expr << lang.or("(Addr_Ring_CE") << lang.and("SNK_OK)");
  else
    default_expr << lang.or("Addr_Ring_CE");
  default_expr << lang.end_statement << endl << endl;


  default_expr << "-- InterFPGA port control" << endl;
  if (right_port==1)
    default_expr << lang.equals("Right_OE","(others => '1')")
		 << lang.end_statement << endl;
  else
    if (fpga_no != 4)
      default_expr << lang.equals("Right_OE","(others => '0')")
		   << lang.end_statement << endl;
  if (left_port==1)
    default_expr << lang.equals("Left_OE","(others => '1')")
		 << lang.end_statement << endl << endl;
  else
    if (fpga_no != 1)
      default_expr << lang.equals("Left_OE","(others => '0')")
		   << lang.end_statement << endl << endl;
    

  ////////////////////////
  // Synchronous processes
  ////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating synchronous process\n");
  StringArray* sync_sensies=new StringArray;

  sync_sensies->add("Reset");
  sync_sensies->add("Clock");

  process_expr << "-- Synchronous process" << endl;
  process_expr << lang.process("Sync",sync_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  
  process_expr << lang.if_statement << lang.test("Reset","'1'") 
	       << lang.then_statement << endl
	       // THEN
	       << "\t" << lang.equals("Master_State","Init_State") 
	       << lang.end_statement << endl 
	       << "\t" << lang.equals("Addr_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Ring_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SRCWC_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SNKWC_State","Init_State") 
	       << lang.end_statement << endl
	       // ELSE_IF
	       << lang.elseif_statement << lang.test("Clock","'1'") 
	       << lang.and("(Clock'event)") << lang.then_statement << endl
	       << "\t" << lang.equals("Master_State","Next_Master_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Addr_State","Next_Addr_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Ring_State","Next_Ring_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SRCWC_State","Next_SRCWC_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SNKWC_State","Next_SNKWC_State") 
	       << lang.end_statement << endl
	       << lang.endif_statement << lang.end_statement << endl;
  
  process_expr << lang.end_function_scope("Sync") << endl;
  
  ////////////////////////////////////////
  // Generate master state machine process 
  ////////////////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating master state machine process\n");
  StringArray* master_sensies=new StringArray;
  master_sensies->add("Master_State");
  master_sensies->add("MemBusGrant_n");
  master_sensies->add("Go");
  master_sensies->add("Wait_AlgFin");
/*
  if (total_srcs)
    master_sensies->add("SRC_Carry");
    */
  if (total_snks)
    {
      master_sensies->add("Delay_Carry");
//      master_sensies->add("SNK_Carry");
    }


  StringArray* master_cases=new StringArray;

  // State1 -- Init_State
  // If there are no memory requirements, then bypass all the master states and idle
  ostrstream master_state1;
  if (total_srcs || total_snks)
    master_state1 << lang.if_statement << lang.test("Go","'1'") 
		  << lang.then_statement << endl
		  << "\t" << lang.equals("Next_Master_State","Wait_For_Mem")
		  << lang.end_statement << endl;
  else
    // For FPGA's without memory accesses, then no memory sequencing needed
    master_state1 << lang.if_statement << lang.test("Go","'1'") 
		  << lang.then_statement << endl
		  << "\t" << lang.equals("Next_Master_State","End_State")
		  << lang.end_statement << endl;
    
  master_state1 << lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Init_State") 
		<< lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;

  master_cases->add(master_state1.str());
  
  // State2 -- Wait_For_Mem
  ostrstream master_state2;
  master_state2 << lang.equals("MemBusReq_n","'0'") << lang.end_statement << endl
		<< lang.if_statement << lang.test("MemBusGrant_n","'0'") 
		<< lang.then_statement << endl;
  
  // Start the address counter and word counter monitors
  master_state2 << "\t" << lang.equals("Go_Addr","'1'") << lang.end_statement << endl;
  if (total_srcs)
    master_state2 << "\t" << lang.equals("Go_WC_SRC","'1'") << lang.end_statement << endl;
  if (total_snks)
    master_state2 << "\t" << lang.equals("Go_WC_SNK","'1'") << lang.end_statement << endl;
		  
  master_state2 << "\t" << lang.equals("Next_Master_State","Wait_For_Finish") 
		<< lang.end_statement << endl
		<< lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Wait_For_Mem") 
		<< lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;
  master_cases->add(master_state2.str());

  // State3 -- Wait_For_Finish
  ostrstream master_state3;
  master_state3 << lang.if_statement << lang.test("Wait_AlgFin","'0'") 
		<< lang.then_statement << endl
		<< "\t" << lang.equals("Next_Master_State","End_State") 
		<< lang.end_statement << endl 
		<< lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Wait_For_Finish") 
		<< lang.end_statement << endl
		<< "\t" << lang.equals("MemBusReq_n","'0'") 
		<< lang.end_statement << endl << endl

		// Address kickoff
		<< "\t" << "--Address kickoff signal" << endl
		<< "\t" << lang.equals("Go_Addr","'1'") << lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;
  master_cases->add(master_state3.str());


  // State4 -- End_State
  ostrstream master_state4;
  master_state4 << lang.equals("Done","'1'") << lang.end_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Init_State") << lang.end_statement 
		<< endl << ends;
  master_cases->add(master_state4.str());

  process_expr << "--" << endl 
	       << "-- Master process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Master",master_sensies) << endl;

  process_expr << lang.begin_scope << endl
	       << endl << "-- Defaults" << endl 
	       << lang.equals("MemBusReq_n","'1'") << lang.end_statement << endl
	       << lang.equals("Go_Addr","'0'") << lang.end_statement << endl
	       << lang.equals("Done","'0'") << lang.end_statement << endl
	       << lang.equals("Go_WC_SRC","'0'") << lang.end_statement << endl
	       << lang.equals("Go_WC_SNK","'0'") << lang.end_statement << endl;
  process_expr << lang.l_case("Master_State",master_states,master_cases);
  process_expr << lang.end_function_scope("Master") << endl;
  



  ///////////////////////////////////////////////////
  // Generate source word counter state machine logic
  ///////////////////////////////////////////////////
  if (total_srcs)
    {
      StringArray* src_wc_sensies=new StringArray;
      src_wc_sensies->add("SRCWC_State");
      src_wc_sensies->add("Go_WC_SRC");
      StringArray* src_wc_cases=new StringArray;

      // State1 -- Init_State
      ostrstream src_state1;
      src_state1 << lang.if_statement << lang.test("Go_WC_SRC","'1'") << lang.then_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Clear_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Init_State") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state1.str());
      
      // State2 -- Clear_WC
      ostrstream src_state2;
      src_state2 << lang.equals("SRC_WC_Control","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Prep_WC") << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state2.str());

      // State3 -- Prep_WC
      ostrstream src_state3;
      src_state3 << lang.equals("SRC_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Prep_WC") << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state3.str());
      
      process_expr << endl << endl
		   << "--" << endl << "-- Source word counter startup process" << endl 
		   << "--" << endl
		   << lang.process("Source_WC",src_wc_sensies) << endl
		   << lang.begin_scope << endl
		   << endl << "-- Source word counter preload default state" << endl
		   << lang.equals("SRC_MUX","'0'") << lang.end_statement << endl
		   << lang.equals("SRC_WC_Control","'0'") << lang.end_statement << endl
		   << lang.l_case("SRCWC_State",src_wc_states,src_wc_cases) << endl
		   << lang.end_function_scope("Source_WC") << endl;

      // Cleanup
      delete src_wc_sensies;
      delete src_wc_cases;
    }


  ///////////////////////////////////////////////////
  // Generate sink word counter state machine logic
  ///////////////////////////////////////////////////
  if (total_snks)
    {
      StringArray* snk_wc_sensies=new StringArray;
      snk_wc_sensies->add("SNKWC_State");
      snk_wc_sensies->add("Go_WC_SNK");
      snk_wc_sensies->add(delay_carry_name);
      StringArray* snk_wc_cases=new StringArray;
      
      // State1 -- Init_State
      ostrstream snk_state1;
      snk_state1 << lang.if_statement << lang.test("Go_WC_SNK","'1'") << lang.then_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Clear_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Init_State") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << endl << ends;
      snk_wc_cases->add(snk_state1.str());
      
      // State2 -- Clear_WC
      ostrstream snk_state2;
      snk_state2 << lang.equals("SNK_WC_Control","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Delay_CE","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Wait_For_Delay") << lang.end_statement << endl 
		 << ends;
      snk_wc_cases->add(snk_state2.str());
      
      // State3 -- Wait_For_Delay
      ostrstream snk_state3;
      snk_state3 << lang.if_statement << lang.test(delay_carry_name,"'0'") << lang.then_statement << endl
		 << "\t" << lang.equals("SNK_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Prep_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Delay_CE","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Delay_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Wait_For_Delay") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << ends;
      snk_wc_cases->add(snk_state3.str());
      
      // State4 -- Prep_WC
      ostrstream snk_state4;
      snk_state4 << lang.equals("SNK_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("SNK_OK","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Prep_WC") << lang.end_statement << endl << ends;
      snk_wc_cases->add(snk_state4.str());
      
      process_expr << endl << endl
		   << "--" << endl << "-- Sink word counter startup process" << endl 
		   << "--" << endl
		   << lang.process("Sink_WC",snk_wc_sensies) << endl
		   << lang.begin_scope << endl
		   << endl << "-- Sink word counter preload default state" << endl
		   << lang.equals("SNK_MUX","'0'") << lang.end_statement << endl
		   << lang.equals("SNK_WC_Control","'0'") << lang.end_statement << endl
		   << lang.equals("SNK_OK","'0'") << lang.end_statement << endl
		   << lang.equals("Delay_CE","'0'") << lang.end_statement << endl
		   << lang.equals("Delay_MUX","'0'") << lang.end_statement << endl
		   << lang.l_case("SNKWC_State",snk_wc_states,snk_wc_cases) << endl
		   << lang.end_function_scope("Sink_WC") << endl;

      // Cleanup
      delete snk_wc_sensies;
      delete snk_wc_cases;
    }


  ///////////////////////////////////////
  // Generate address state machine logic
  ///////////////////////////////////////
  StringArray* addr_sensies=new StringArray;
  addr_sensies->add("Addr_State");
  addr_sensies->add("Go_Addr");
  addr_sensies->add("Wait_AlgFin");

  StringArray* addr_cases=new StringArray;

  // State1 -- Init_State
  ostrstream addr_state1;
  addr_state1 << lang.if_statement << lang.test("Go_Addr","'1'") << lang.then_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Clear_Addr") << lang.end_statement << endl
	      << lang.else_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Init_State") << lang.end_statement << endl 
	      << lang.endif_statement << lang.end_statement << endl << ends;
  addr_cases->add(addr_state1.str());

  // State2 -- Clear_Addr
  ostrstream addr_state2;
  addr_state2 << lang.equals("Next_Addr_State","Enable_Addr") << lang.end_statement << endl << endl

	      // Enable the preload signal (clr on the adder)
	      << "-- Address generator preload generator" << endl
	      << lang.equals("ADDR_CLR","'1'") << lang.end_statement << endl << endl
	      << "-- Initialize algorithm" << endl
	      << lang.equals("Alg_Start","'1'") << lang.end_statement << endl << ends;
  addr_cases->add(addr_state2.str());

  // State3 -- Enable_Addr
  ostrstream addr_state3;
  addr_state3 << "-- Address count enable engaged" << endl
	      << "\t" << lang.equals("Addr_Addr_CE","'1'") << lang.end_statement << endl
	      << "\t" << "-- Ring counter enable engaged" << endl
	      << "\t" << lang.equals("Go_Ring","'1'") << lang.end_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Wait_For_Finish") << lang.end_statement << endl
	      << ends;
  addr_cases->add(addr_state3.str());

  // Stat4 -- Wait_For_Finish
  ostrstream addr_state4;
  addr_state4 << lang.if_statement << lang.test("Wait_AlgFin","'0'") << lang.then_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Init_State") << lang.end_statement << endl
	      << lang.else_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Wait_For_Finish") << lang.end_statement << endl 
	      << lang.endif_statement << lang.end_statement << endl << ends;
  addr_cases->add(addr_state4.str());


  process_expr << endl << endl;
  process_expr << "--" << endl << "-- Address startup process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Addr",addr_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  process_expr << endl << "-- Address generator preload default state" << endl
	       << lang.equals("ADDR_CLR","'0'") << lang.end_statement  << endl << endl
	       << "-- Address counter count enable default state" << endl
	       << lang.equals("Addr_Addr_CE","'0'") << lang.end_statement << endl << endl
	       << "-- Ringer counter enable default state" << endl
	       << lang.equals("Go_Ring","'0'") << lang.end_statement << endl << endl
	       << "-- Algorithm initialization default state" << endl
	       << lang.equals("Alg_Start","'0'") << lang.end_statement << endl << endl
	       << "-- Source and/or Sink Word counter kickoff default state" << endl;
  if (total_srcs)
    process_expr << lang.equals("SRC_Addr_Control","'0'") << lang.end_statement << endl;
  if (total_snks)
    process_expr << lang.equals("SNK_Addr_Control","'0'") << lang.end_statement << endl;

  process_expr << lang.l_case("Addr_State",addr_states,addr_cases) << endl;
  process_expr << lang.end_function_scope("Addr") << endl;

  // Store mux activation signals in an index stream array.  The index
  // specifies the applicable Phase.
  ostrstream* phase_streams=new ostrstream[seqlen+1];
  ostrstream mux_defaults;

  ///////////////////////////////
  // Generate output mux controls
  ///////////////////////////////
  // ASSUMPTION: MUXs are organized with the LSB
  //             assigned to the first algoritm smart generator and 
  //             the MSB assigned to the last algorithm smart generator

  if (DEBUG_SEQUENCER)
    printf("Generating the output mux controls\n");
  // FIX: Should compress method with address mux controls!
  ostrstream ram_defaults;
  ostrstream ram_init_state;
  ostrstream ctrl_defaults;
  for (int mem_loop=0;mem_loop<fpga_elem->mem_count;mem_loop++)
    if ((fpga_elem->mem_connected)->query(mem_loop)==MEMORY_AVAILABLE)
      {
	MemPort* mem_port=arch->get_memory_handle(mem_loop);
	ACSIntArray* mem_con=fpga_elem->mem_connected;
	int mem_connected=mem_con->get(mem_loop);
	if (DEBUG_SEQUENCER)
	  printf("Memory %d con status to fpga %d is %d\n",mem_loop,fpga_no,mem_connected);
	
	if ((mem_port->portuse==MEMORY_USED) && (mem_connected==MEMORY_AVAILABLE))
	  {
	    SequentialList* source_stars=mem_port->source_stars;
	    SequentialList* sink_stars=mem_port->sink_stars;
	    int src_sgs=source_stars->size();
	    int snk_sgs=sink_stars->size();
	    int total_sgs=src_sgs+snk_sgs;
	    
	    int sources_reconnected=FALSE;
	    int sinks_reconnected=FALSE;
	    
	    if (mem_port->bidir_data)
	      {
		if (src_sgs+snk_sgs > 1)
		  {
		    sources_reconnected=TRUE;
		    sinks_reconnected=TRUE;
		  }
	      }
	    else
	      {
		if (src_sgs > 1)
		  sources_reconnected=TRUE;
		if (snk_sgs > 1)
		  sinks_reconnected=TRUE;
	      }
	    
	    Port_Timing* port_timing=mem_port->port_timing;
	    
	    if (DEBUG_SEQUENCER)
	      {
		int entries=(port_timing->data_activation)->population();
		printf("Times for port %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->data_activation)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->data_activation)->query(sg_loop));
		printf("Types for memory %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mem_type)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mem_type)->query(sg_loop));
		printf("List ids for port %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mem_id)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mem_id)->query(sg_loop));
		printf("Mux_lines for memory %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mux_line)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mux_line)->query(sg_loop));
	      }
	    


	    //////////////////////////////////////
	    // Reassign mux control pin identities
	    //////////////////////////////////////
	    int imux_cnodes=0;
	    if (sources_reconnected)
	      {
		ACSCGFPGACore* data_imux=mem_port->dataimux_star;
		Pin* imux_pins=data_imux->pins;
		ACSIntArray* imux_cnode=imux_pins->query_cnodes();
		imux_cnodes=imux_cnode->population();
		imux_pins->reclassify_pin(INPUT_PIN_CTRL,INPUT_PIN_MEMORYMUX);
	      }
	    
	    int omux_cnodes=0;
	    if (sinks_reconnected)
	      {
		ACSCGFPGACore* data_omux=mem_port->dataomux_star;
		Pin* omux_pins=data_omux->pins;
		ACSIntArray* omux_cnode=omux_pins->query_cnodes();
		omux_cnodes=omux_cnode->population();
		omux_pins->reclassify_pin(INPUT_PIN_CTRL,INPUT_PIN_MEMORYMUX);
	      }
	    
	    // Establish data mux selection control line
	    char* mux_cname=new char[MAX_STR];
	    char* mux_prefix=new char[MAX_STR];
	    
	    // Maintain a phase list for each control line
	    int mode;
	    if (mem_port->bidir_data)
	      {
		mode=1;
		strcpy(mux_prefix,"_BI_");
	      }
	    else
	      mode=2;
	    

	    // FIX: Boy, this is sure ugly stuff....
	    for (int mux_loop=1;mux_loop<=mode;mux_loop++)
	      {
		mux_defaults << "--Data MUX" << mem_loop 
			     << " Controls (defaults)" << endl;
		if (!mem_port->bidir_data)
		  if (mux_loop==1)
		    strcpy(mux_prefix,"_IN_");
		  else
		    strcpy(mux_prefix,"_OUT_");
		
		int ctrl_count=imux_cnodes;
		if (mux_loop==2)
		  ctrl_count=omux_cnodes;
		
		for (int ctrl_loop=0;ctrl_loop<ctrl_count;ctrl_loop++)
		  {
		    // ASSUMPTION: mux_sline controls an entire row of muxs in the hierarchy
		    sprintf(mux_cname,"%s%s%d_%d","MEMORY_MUX",mux_prefix,mem_loop,ctrl_loop);
		    mux_defaults << lang.equals(mux_cname,"'0'") 
				 << lang.end_statement << endl;
		    
		    // FIX: And kludge up the entry in the UniSeq port definition
		    int mux_pin=seq_pins->add_pin(mux_cname,
						  0,1,
						  STD_LOGIC,
						  OUTPUT_PIN_MEMORYMUX);
		    seq_ext_signals->add_pin(mux_cname,
					     0,1,
					     STD_LOGIC,
					     OUTPUT_PIN_MEMORYMUX);
		    seq_pins->set_pinpriority(mux_pin,ctrl_loop);
		    
		    // Test to see if data is sourced or sunked on any given phase
		    for (int sg_loop=0;sg_loop < (port_timing->data_activation)->population();sg_loop++)
		      {
			int data_act=(port_timing->data_activation)->query(sg_loop);
			int sg_pos=(port_timing->mem_id)->query(sg_loop)-1;
			if (DEBUG_SEQUENCER)
			  printf("DataMux:data_act=%d, sg_pos=%d\n",
				 data_act,
				 sg_pos);
			
			if (mem_port->bidir_data)
			  {
			    if (bselcode(sg_pos,ctrl_loop)==1)
			      phase_streams[data_act] << lang.equals(mux_cname,"'1'")
						      << lang.end_statement << endl;
			  }
			else
			  {
			    if (DEBUG_SEQUENCER)
			      printf("ctrl=%d, sg_loop=%d, bselcode=%d, mem_type=%d\n",
				     ctrl_loop,
				     sg_loop,
				     bselcode(sg_pos,ctrl_loop),
				     (port_timing->mem_type)->query(sg_loop));
			    if (mux_loop==1)
			      {
				if ((bselcode(sg_pos,ctrl_loop)==1) &&
				    ((port_timing->mem_type)->query(sg_loop)==SOURCE))
				  {
				    phase_streams[data_act] << lang.equals(mux_cname,"'1'")
							    << lang.end_statement << endl;
				    if (DEBUG_SEQUENCER)
				      {
					ACSCGFPGACore* src_star=(ACSCGFPGACore*) 
					  source_stars->elem(sg_pos+1);
					printf("Source sg %s, addr_act=%d, selected on time %d\n",
					       src_star->comment_name(),
					       src_star->act_input,
					       data_act);
				      }
				  }
			      }
			    else
			      if ((bselcode(sg_pos,ctrl_loop)==1) &&
				  ((port_timing->mem_type)->query(sg_loop)==SINK))
				{
				  phase_streams[data_act] << lang.equals(mux_cname,"'1'")
							  << lang.end_statement << endl;
				  if (DEBUG_SEQUENCER)
				    {
				      ACSCGFPGACore* snk_star=(ACSCGFPGACore*) sink_stars->elem(sg_pos+1);
				      printf("Sink sg %s, selected on time %d\n",
					     snk_star->comment_name(),
					     data_act);
				    }
				}
			  }
		      }
		  }
	      }
	    
	    // Cleanup
	    mux_defaults << endl;
	    delete []mux_cname;
	    delete []mux_prefix;
	    

	    /////////////////////////////////////////////
	    // Generate address generator control signals
	    /////////////////////////////////////////////
	    // Hookup the reset line
	    mux_defaults << "-- Address Generator " << mem_loop 
			 << " Controls (defaults)" << endl;

	    // If address is active, then engage the count enabler
	    // Trap for duplicates, although will not hinder Synplicity VHDL-comp.
	    int* dup_phase=NULL;
	    int dup_count=0;
	    for (int sg_loop=0;sg_loop<total_sgs;sg_loop++)
	      {
		// Address mux switching must occur one clock period
		// prior to its necessity
		Port_Timing* port_timing=mem_port->port_timing;
		int addr_phase=(port_timing->address_activation)->query(sg_loop)-1;
		if (addr_phase < 0)
		  addr_phase=seqlen-1;
		if (DEBUG_SEQUENCER)
		  printf("sg %d will engage addr counter on phase %d, orig %d\n",
			 sg_loop,
			 addr_phase,
			 (port_timing->address_activation)->query(sg_loop));
		
		int not_dup=TRUE;
		if (dup_count>0)
		  for (int dup_loop=0;dup_loop<dup_count;dup_loop++)
		    if (addr_phase==dup_phase[dup_loop])
		      {
			not_dup=FALSE;
			break;
		      }
		if (not_dup)
		  {
		    phase_streams[addr_phase] << lang.equals("Addr_Ring_CE","'1'")
					      << lang.end_statement << endl;
		    if (dup_count==0)
		      dup_phase=new int;
		    else
		      {
			int *tmp_dup=new int[dup_count+1];
			for (int cloop=0;cloop<dup_count;cloop++)
			  tmp_dup[cloop]=dup_phase[cloop];
			delete []dup_phase;
			dup_phase=tmp_dup;
		      }
		    dup_phase[dup_count]=addr_phase;
		    dup_count++;
		  }
	      }
	    delete []dup_phase;
	    
	    // Test for MUXs
	    if (DEBUG_SEQUENCER)
	      printf("Generating the mux/ram processes\n");
	    if (mem_port->addrmux_star != NULL)
	      {
		// Generate control lines for each MUX "bank"
		ACSCGFPGACore* addr_mux=mem_port->addrmux_star;
		Pin* mux_ctrl=addr_mux->pins;
		Port_Timing* port_timing=mem_port->port_timing;
		
		// Establish addr mux selection control line
		char* mux_name=new char[MAX_STR];
		char* dmux_name=new char[MAX_STR];
		char* tmp_name=new char[MAX_STR];
		
		// Maintain a phase list for each control line
		// ASSUMPTION:Only mux switching lines are in the cnodes list
		for (int pin_loop=0;pin_loop<mux_ctrl->query_pincount();pin_loop++)
		  if (mux_ctrl->query_pintype(pin_loop)==INPUT_PIN_CTRL)
		    mux_ctrl->set_pintype(pin_loop,INPUT_PIN_ADDRMUX);
		
		Pin* addrmux_pins=addr_mux->pins;
		ACSIntArray* cnode_pins=addrmux_pins->query_cnodes();
		int cnode_count=cnode_pins->population();
		for (int ctrl_loop=0;ctrl_loop<cnode_count;ctrl_loop++)
		  {
		    // FIX: Kludge up the entry in the UniSeq port definition
		    sprintf(mux_name,"%s%d_%d","ADDR_MUX",mem_loop,ctrl_loop);
		    mux_defaults << lang.equals(mux_name,"'0'") 
				 << lang.end_statement << endl;
		    
		    int mux_pin=seq_pins->add_pin(mux_name,
						  0,1,
						  STD_LOGIC,
						  OUTPUT_PIN_ADDRMUX);
		    seq_ext_signals->add_pin(mux_name,
					     0,1,
					     STD_LOGIC,
					     OUTPUT_PIN_ADDRMUX);
		    seq_pins->set_pinpriority(mux_pin,ctrl_loop);
		    
		    // Test each star to see if it will be selected on this phase
		    for (int time_loop=0;time_loop<total_sgs;time_loop++)
		      {
			int mux_phase=(port_timing->address_activation)->query(time_loop)-1;
			
			// Mux position is skewed since pos 0 is the initial
			// address
			int mux_pos=0;
			if (mux_phase < 0)
			  {
			    mux_phase=seqlen-1;
			    mux_pos=total_sgs;
			  }
			else
			  // Since addresses are ordered
			  mux_pos=time_loop;
			

			if (DEBUG_SEQUENCER)
			  {
			    int debug=bselcode(mux_pos,ctrl_loop);
			    printf("MemPort %d, ctrl_loop= %d, AddrMux:addr_act=%d, mux_phase=%d, mux_pos=%d, bselcode=%d\n",
				   mem_loop,
				   ctrl_loop,
				   (port_timing->address_activation)->query(time_loop),
				   mux_phase,
				   mux_pos,
				   debug);
			  }
			
			if (bselcode(mux_pos,ctrl_loop)==1)
			  phase_streams[mux_phase] << lang.equals(mux_name,"'1'")
						   << lang.end_statement << endl;
			
		      }
		  }
		
		// Cleanup
		mux_defaults << endl;
		delete []dmux_name;
		delete []mux_name;
		delete []tmp_name;
	      }
	    mux_defaults << endl;
	    
	    
	    //////////////////////////////////
	    // Generate memory control signals
	    //////////////////////////////////
	    char* ramg_name=new char[80];
	    char* ramw_name=new char[80];
	    char* rame_name=new char[80];
	    
	    // FIX: If a single R/W control line, which is active(H)??
	    //      Defaulting to W is active high
	    sprintf(ramg_name,"%s_%d","RAMG",mem_loop);
	    sprintf(ramw_name,"%s_%d","RAMW",mem_loop);
	    sprintf(rame_name,"%s_%d","RAME",mem_loop);
	    
	    // Add pin definitions on sequencer smart generator for available
	    // control signals
	    ram_defaults << endl << "--Address Generator" << mem_loop 
			 << " RAM Control Lines (defaults)" << endl;
	    
	    if (fpga_elem->separate_rw)
	      if ((fpga_elem->read_signal_id)->get(mem_loop)!=UNASSIGNED)
		{
		  int ramg_pin=seq_pins->add_pin(ramg_name,
						 0,1,
						 STD_LOGIC,
						 OUTPUT_PIN_RAMG);
		  seq_ext_signals->add_pin(ramg_name,
					   0,1,
					   STD_LOGIC,
					   OUTPUT_PIN_RAMG);
		  seq_pins->set_pinpriority(ramg_pin,mem_loop);
		  
		  ram_defaults << lang.equals(ramg_name,"'1'") 
			       << lang.end_statement << endl;
		}
	    
	    if ((fpga_elem->write_signal_id)->get(mem_loop)!=UNASSIGNED)
	      {
		int ramw_pin=seq_pins->add_pin(ramw_name,
					       0,1,
					       STD_LOGIC,
					       OUTPUT_PIN_RAMW);
		seq_ext_signals->add_pin(ramw_name,
					 0,1,
					 STD_LOGIC,
					 OUTPUT_PIN_RAMW);
		seq_pins->set_pinpriority(ramw_pin,mem_loop);
		
		ram_defaults << lang.equals(ramw_name,"'1'") 
			     << lang.end_statement << endl;
	      }
	    
	    if ((fpga_elem->enable_signal_id)->get(mem_loop)!=UNASSIGNED)
	      {
		int rame_pin=seq_pins->add_pin(rame_name,
					       0,1,
					       STD_LOGIC,
					       OUTPUT_PIN_RAME);
		seq_ext_signals->add_pin(rame_name,
					 0,1,
					 STD_LOGIC,
					 OUTPUT_PIN_RAME);
		seq_pins->set_pinpriority(rame_pin,mem_loop);
		
		ram_defaults << lang.equals(rame_name,"'0'") << lang.end_statement << endl;
		ram_init_state << lang.equals(rame_name,"'1'") << lang.end_statement << endl;
	      }
	    
	    for (int phase_loop=0;phase_loop<mem_port->pt_count;phase_loop++)
	      {
//		printf("phase %d, mem_port->fetch_pt=%d\n",phase_loop,
//		       mem_port->fetch_pt(phase_loop));
		if (mem_port->fetch_pt(phase_loop)==SOURCE)
		  {
//		    printf("sourcing\n");
		    if ((fpga_elem->separate_rw) && 
			((fpga_elem->read_signal_id)->get(mem_loop)!=UNASSIGNED))
		      {
//			printf("read high\n");
			phase_streams[phase_loop] << "\t" 
						  << lang.equals(ramg_name,"'1'")
						  << lang.end_statement << endl;
		      }
		  }
		else if (mem_port->fetch_pt(phase_loop)==SINK)
		  {
//		    printf("sinking\n");
		    if ((fpga_elem->write_signal_id)->get(mem_loop)!=UNASSIGNED)
		      {
//			printf("write = delay?\n");
			phase_streams[phase_loop] << "\t" 
						  << lang.equals(ramw_name,"'0'") 
						  << lang.or("(") << lang.not("SNK_OK)")
						  << lang.end_statement << endl;
		      }
		  }
		else
		  {
//		    printf("othering\n");
		    phase_streams[phase_loop] << "\t" 
					      << lang.equals(rame_name,"'1'")
					      << lang.end_statement << endl;
		  }
	      }
	    
	    
	    delete []ramg_name;
	    delete []ramw_name;
	    delete []rame_name;
	  } //if ((mem_port->portuse==MEMORY_USED) && (mem_connected==MEMORY_AVAILABLE))
	mux_defaults << endl;
	
      } // if (mem_port->portuse==MEMORY_USED)
  
  ///////////////////////////////////
  // Generate default control signals
  ///////////////////////////////////
  ctrl_defaults << endl << "-- Source and/or Sink Control defaults" << endl;
  if (total_srcs)
    ctrl_defaults << lang.equals("SRC_Ring_Control","'0'") << lang.end_statement << endl;
  if (total_snks)
    ctrl_defaults << lang.equals("SNK_Ring_Control","'0'") << lang.end_statement << endl;
  
  ctrl_defaults << lang.equals("Addr_Ring_CE","'0'")
		<< lang.end_statement << endl;

  ctrl_defaults << endl << "-- Completion signals" << endl;
  ctrl_defaults << lang.equals("Wait_AlgFin","'1'") << lang.end_statement << endl;
	  
  char* rsig_name=new char[MAX_STR];
  ctrl_defaults << endl << "-- Current phase signals" << endl;
  for (int loop=0;loop<seqlen;loop++)
    {
      sprintf(rsig_name,"Phase%d_Go",loop);
      ctrl_defaults << lang.equals(rsig_name,"'0'") << lang.end_statement << endl;
    }
  ctrl_defaults << endl;
  delete []rsig_name;

  ///////////////////////////////////////////////////////
  // Generate the ring (i.e., phase0 through phase(seqlen)
  ////////////////////////////////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating the ring process\n");

  StringArray* ring_sensies=new StringArray;
  StringArray* ring_cases=new StringArray;
  ring_sensies->add("Ring_State");
  ring_sensies->add("Go_Ring");
  if (total_snks)
    {
      ring_sensies->add("SNK_Carry");
      ring_sensies->add("SNK_OK");
    }
  else if (total_srcs) 
    ring_sensies->add("SRC_Carry");
      
  ram_init_state << ends;
  
  ostrstream ring_state;
  ring_state << ram_init_state.str()
	     << lang.equals("Addr_Ring_CE","'1'") << lang.end_statement << endl
	     << lang.if_statement << lang.test("Go_Ring","'1'") << lang.then_statement << endl
	     << "\t" << lang.equals("Next_Ring_State","Phase0") << lang.end_statement << endl
	     << lang.else_statement << endl
	     << "\t" << lang.equals("Next_Ring_State","Init_State") << lang.end_statement << endl
	     << lang.endif_statement << lang.end_statement << endl << ends;
  ring_cases->add(ring_state.str());

  char* ring_name=new char[MAX_STR];
  char* ringsig_name=new char[MAX_STR];

  // Terminate all phase streams
  for (int loop=0;loop<seqlen;loop++)
    phase_streams[loop] << ends;

  for (int loop=0;loop<seqlen;loop++)
    {
      ostrstream lring_case;
      if (loop!=0)
	{
	  if ((loop+1)==seqlen)
	    {
	      strcpy(ring_name,"Phase0");
	      lring_case << "\t" << "-- Source and/or Sink word counter control generator" << endl;
	      if (total_snks)
		lring_case << lang.if_statement << lang.test(sink_carry_name,"'0'");
	      else if (total_srcs)
		lring_case << lang.if_statement << lang.test(source_carry_name,"'0'");

	      if (total_snks || total_srcs)
		{
		  lring_case << lang.then_statement << endl
			     << "\t" << lang.equals("Wait_AlgFin","'0'") << lang.end_statement << endl
			     << "\t" << lang.equals("Next_Ring_State","Init_State") 
			     << lang.end_statement << endl
			     << lang.else_statement << endl
			     << "\t" << lang.equals("Next_Ring_State","Phase0") << lang.end_statement
			     << endl;
		  if (total_srcs)
		    lring_case << lang.equals("SRC_Ring_Control","'1'") << lang.end_statement << endl;
		  if (total_snks)
		    lring_case << lang.equals("SNK_Ring_Control","'1'") << lang.end_statement << endl;

		  lring_case << lang.endif_statement << lang.end_statement << endl;
		}
	    }
	  else
	    {
	      sprintf(ring_name,"Phase%d",loop+1);
	      lring_case << lang.equals("Next_Ring_State",ring_name) 
			 << lang.end_statement << endl;
	    }

	  sprintf(ringsig_name,"Phase%d_Go",loop);
	      
	  lring_case << phase_streams[loop].str() 
		     << lang.equals(ringsig_name,"'1'") << lang.end_statement
		     << ends;
	}
      else
	{
	  lring_case << lang.equals("Next_Ring_State","Phase1") << lang.end_statement << endl << endl
		     << "\t" << "-- Increment word count for this sequence" << endl
		     << "\t" << phase_streams[0].str() << endl
		     << "\t" << lang.equals("Phase0_Go","'1'") << lang.end_statement << endl 
		     << ends;
	}
      ring_cases->add(lring_case.str());
    }
  delete []ring_name;
  delete []ringsig_name;
  delete []phase_streams;
  delete []source_carry_name;
  delete []sink_carry_name;
  delete []delay_carry_name;


  // Terminate streams
  mux_defaults << ends;
  ram_defaults << ends;
  ctrl_defaults << ends;

  
  process_expr << "--" << endl 
	       << "-- Ring process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Ring",ring_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  process_expr << mux_defaults.str();
  process_expr << ram_defaults.str();
  process_expr << ctrl_defaults.str();
  process_expr << lang.l_case("Ring_State",ring_states,ring_cases) << endl;
  process_expr << lang.end_function_scope("Ring") << endl;
  

  ///////////////////////////
  // Now propagate bit widths
  ///////////////////////////
  /*
  HWset_bw(smart_generators,
	   seq_ext_signals,
	   root_constant_signals,
	   seq_ctrl_signals);
	   */


  ////////////////////
  // Conduct stitching
  ////////////////////
  if (DEBUG_SEQUENCER)
    printf("Exporting sequencer vhdl\n");

  // Establish filenames and open file stream
  ostrstream of_filename;
  of_filename << design_directory->retrieve_extended() 
	      << seq_core->name() << ".vhd" << ends;
  ofstream fstr(of_filename.str());

  ostrstream entity_name;
  entity_name << seq_core->name() << ends;

  // Freeze streams
  preamble_expr << ends;
  default_expr << ends;
  process_expr << ends;

  // Generate code
  fstr << lang.gen_libraries(all_libraries,all_includes);
  fstr << lang.gen_entity(entity_name.str(),seq_ext_signals);
  fstr << lang.gen_architecture(entity_name.str(),
				NULL,
				BEHAVIORAL,
				seq_ext_signals,
				seq_data_signals,
				seq_ctrl_signals,
				seq_constant_signals);
//  fstr << lang.gen_components(entity_name.str(),NULL);
//  fstr << lang.gen_configurations(NULL);
//  fstr << lang.gen_instantiations(NULL,seq_ext_signals,seq_data_signals,
//				  seq_ctrl_signals,root_constant_signals);
  fstr << preamble_expr.str() << endl << endl;
  fstr << lang.begin_scope << endl;
  fstr << default_expr.str() << endl;
  fstr << process_expr.str() << endl;
  fstr << lang.end_scope << lang.end_statement << endl;

  // Cleanup
  delete seq_ext_signals;
  delete seq_ctrl_signals;
//  delete seq_data_signals;
  delete seq_constant_signals;
  delete sync_sensies;
  delete master_states;
  delete master_sensies;
  delete master_cases;
  delete addr_states;
  delete addr_sensies;
  delete addr_cases;
  delete ring_states;
  delete ring_sensies;
  delete ring_cases;
  delete src_wc_states;
  delete snk_wc_states;
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
      construct->set_targetdevice(fpga_elem,output_core->acs_device);

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
	  dest_pins=ctrl_signals;
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
      construct->set_targetdevice(dest_fpga,dest_core->acs_device);
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

  if ((src_majorbit != dest_majorbit) ||
      (src_bitlen != dest_bitlen))
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
/*
	      printf("Inserting a buffer core %s between the extpin (type %d) and %s\n",
		     buffer_core->comment_name(),
		     ptype,
		     dest_core->comment_name());
	      printf("output_pin=%d, output_node=%d, input_pin=%d, input_node=%d\n",
		     output_pin,
		     output_node,
		     input_pin,
		     input_node);
		     */

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
				 LOCKED);
      buffer_pins->set_precision(1,
				 dest_majorbit,
				 dest_bitlen,
				 LOCKED);

      if (DEBUG_BWRESOLVER)
	printf("BWerror resolved\n\n");
    }

  // Cleanup
//  delete construct;
}


////////////////////////////////////////////////////
// Identify sg links which cross domains and resolve
////////////////////////////////////////////////////
void ACSCGFPGATarget::HWalg_resolver(void)
{
  if (DEBUG_RESOLVER)
    printf("\n\nResolver\n");

  // Determine data signal links between smart generators
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_core->acs_device);
      Pin* fpga_pins=fpga_core->pins;

      ACSIntArray* opins=fpga_pins->query_onodes();
      if (DEBUG_RESOLVER)
	printf("resolving driving links for star %s with %d opins\n",
	       fpga_core->comment_name(),
	       opins->population());
      
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int output_pin=opins->query(opin_loop);
	  Connectivity* output_nodes=fpga_pins->query_connection(output_pin);

	  if (DEBUG_RESOLVER)
	    printf("pin %d has %d nodes\n",opin_loop,output_nodes->node_count);
	  for (int onode_loop=0;onode_loop<output_nodes->node_count;onode_loop++)
	    {
	      Pin* dest_pins;
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
		  dest_pins=fpga_elem->ctrl_signals;
		  strcpy(name,"ctrl_signals");
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
		  if (onode_loop!=0) // Bind all additonal node signals to 
		                     // the primary
		    {
		      data_signal=output_nodes->query_pinsignal(0);
		      output_nodes->set_pinsignal(onode_loop,data_signal);
		    }
		  else
		    {
		      // Need to assign an intermediate signal name for these
		      // Add a new data signal name
		      char* tmp_name=new char[MAX_STR];
		      Pin* local_dsignals=fpga_elem->data_signals;
		      sprintf(tmp_name,"data_signal%d",
			      local_dsignals->query_pincount());
		      data_signal=local_dsignals->
			add_pin(tmp_name,
				fpga_pins->query_majorbit(output_pin),
				fpga_pins->query_bitlen(output_pin),
				STD_LOGIC,
				INTERNAL_PIN);
		      // Cleanup
		      delete []tmp_name;
		      
		      output_nodes->set_pinsignal(onode_loop,data_signal);
		    }
		}
	      else
		data_signal=output_nodes->query_pinsignal(onode_loop);
	      
	      input_nodes->set_pinsignal(input_node,data_signal);

	      if (DEBUG_RESOLVER)
		{
		  printf("it is connected to star %s via input_node %d\n",
			 name,
			 input_node);
		  printf("Assigning signal %d of type %s to this link\n",
			 data_signal,
			 name);
		}
	      delete []name;
	      
	    } // for (int onode_loop=0;onode_loop<output_nodes->node_count; ...
	} // for (int opin_loop=0;opin_loop<opins->size();opin_loop++)
    } // for (int loop=1;loop<=smart_generators->size();loop++)

  ////////////////////////////////////////////////////////
  // Now examine control signals and find a correspondance
  ////////////////////////////////////////////////////////
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_core->acs_device);
      Pin* fpga_pins=fpga_core->pins;
      if (DEBUG_RESOLVER)
	printf("\nResolving control links for star %s\n",
	       fpga_core->comment_name());
      
      ACSIntArray* cpins=fpga_pins->query_cnodes();
      for (int cpin_loop=0;cpin_loop<cpins->population();cpin_loop++)
	{
	  int control_pin=cpins->query(cpin_loop);
	  if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
	    {
	      // Port was not assigned previously, need to resolve with 
	      // externals
	      if (DEBUG_RESOLVER)
		printf("Pin %s, of type %d is unresolved\n",
		       fpga_pins->query_pinname(control_pin),
		       fpga_pins->query_pintype(control_pin));
	      
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
		  
		  // FIX: Only local control signals are allowed!
		  SequentialList* fpga_children=
		    fpga_elem->get_childcores_handle();

		  for (int iloop=1;iloop<=fpga_children->size();iloop++)
		    {
		      ACSCGFPGACore* candidate_core=
			(ACSCGFPGACore*) fpga_children->elem(iloop);
		      
		      // No inbreeding:)
		      if (fpga_core->acs_id != candidate_core->acs_id)
			{
			  if (DEBUG_RESOLVER)
			    printf("Checking sg %s\n",
				   candidate_core->comment_name());

			  // Check smart generators 
			  // fpga_elem->ctrl_signals or candidate_core????
			  HWsig_compare(fpga_pins->query_pintype(control_pin),
					fpga_pins,
					control_pin,
					candidate_core->pins,
					CTRL_NODE,
					fpga_elem->ctrl_signals);
			}
		    }
		}
	      
	      // Still unresolved?
	      // Then check against constant signals
	      if (fpga_pins->query_pinassigned(control_pin)==UNASSIGNED)
		{
		  if (DEBUG_RESOLVER)
		    printf("Comparing to constant signals\n");
		  
		  HWsig_compare(fpga_pins->query_pintype(control_pin),
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
		printf("Pin %s, of type %d is already resolved\n",
		       fpga_pins->query_pinname(control_pin),
		       fpga_pins->query_pintype(control_pin));
	    }
	} // for (int cpin_loop=0;cpin_loop<cpins->size();cpin_loop++)
    } //   for (int loop=1;loop<=smart_generators->size();loop++)
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
      switch (rule_type)
	{
	case EXT_NODE:
	  match=HWresolver_extrules(refpin_type,
				    candidate_pins->query_pintype(pin_loop));
	  break;
	case CTRL_NODE:
	  match=HWresolver_ctrlrules(refpin_type,
				     candidate_pins->query_pintype(pin_loop));
	  break;
	case CONSTANT_NODE:
	  match=HWresolver_construles(refpin_type,
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
	    // FIX?: Should smart generator connectivity be emplaced?
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
	  printf(" does not match, srctype=%d, cand_type=%d\n",
		 refpin_type,
		 candidate_pins->query_pintype(pin_loop));
    }
}
	    


// These rules define if a control signal should be associated given a reference
// pin on a smart generator (sg_sig) and a likely candidate signal (pool_sig)
int ACSCGFPGATarget::HWresolver_extrules(int sg_sig,int pool_sig)
{
  switch (sg_sig)
    {
      // Clocking pins
    case INPUT_PIN_CLK:
      switch (pool_sig)
	{
	  // Latch onto an external pin first
	case INPUT_PIN_CLK:
	  return(1);
	case OUTPUT_PIN_CLK:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_RESET:
      switch (pool_sig)
	{
	  // Latch onto an external pin first
	case INPUT_PIN_RESET:
	  return(1);
	case OUTPUT_PIN_RESET:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_CE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_CE:
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
    case OUTPUT_PIN_CLK:
      switch (pool_sig)
	{
	  // Latch onto an external pin first
	case INPUT_PIN_CLK:
	  return(1);
	case OUTPUT_PIN_CLK:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_RESET:
      switch (pool_sig)
	{
	  // Latch onto an external pin first
	case INPUT_PIN_RESET:
	  return(1);
	case OUTPUT_PIN_RESET:
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
    case OUTPUT_PIN_RAMG:
      switch (pool_sig)
	{
	case OUTPUT_PIN_RAMG:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_RAMW:
      switch (pool_sig)
	{
	case OUTPUT_PIN_RAMW:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_RAME:
      switch (pool_sig)
	{
	case OUTPUT_PIN_RAME:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_INTERCONNECT_ENABLE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_INTERCONNECT_ENABLE:
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
int ACSCGFPGATarget::HWresolver_ctrlrules(int sg_sig,int pool_sig)
{
  switch (sg_sig)
    {
    case INPUT_PIN_SRC_WC:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SRC_WC:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SRC_WC:
      switch (pool_sig)
	{
	case INPUT_PIN_SRC_WC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SRC_MUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SRC_MUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SRC_MUX:
      switch (pool_sig)
	{
	case INPUT_PIN_SRC_MUX:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SRC_CE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SRC_CE:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SRC_CE:
      switch (pool_sig)
	{
	case INPUT_PIN_SRC_CE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_MEMORYMUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_MEMORYMUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_MEMORYMUX:
      switch (pool_sig)
	{
	case INPUT_PIN_MEMORYMUX:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_ADDRMUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_ADDRMUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_ADDRMUX:
      switch (pool_sig)
	{
	case INPUT_PIN_ADDRMUX:
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
    case OUTPUT_PIN_ADDRCE:
      switch (pool_sig)
	{
	case INPUT_PIN_ADDRCE:
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
    case OUTPUT_PIN_ADDRCLR:
      switch (pool_sig)
	{
	case INPUT_PIN_ADDRCLR:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_DMUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_DMUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_DMUX:
      switch (pool_sig)
	{
	case INPUT_PIN_DMUX:
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
    case OUTPUT_PIN_PHASE:
      switch (pool_sig)
	{
	case INPUT_PIN_PHASE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SNK_WC:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SNK_WC:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SNK_WC:
      switch (pool_sig)
	{
	case INPUT_PIN_SNK_WC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SNK_CE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SNK_CE:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SNK_CE:
      switch (pool_sig)
	{
	case INPUT_PIN_SNK_CE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_SNK_MUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_SNK_MUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_SNK_MUX:
      switch (pool_sig)
	{
	case INPUT_PIN_SNK_MUX:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_DELAY_WC:
      switch (pool_sig)
	{
	case OUTPUT_PIN_DELAY_WC:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_DELAY_WC:
      switch (pool_sig)
	{
	case INPUT_PIN_DELAY_WC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_DELAY_CE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_DELAY_CE:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_DELAY_CE:
      switch (pool_sig)
	{
	case INPUT_PIN_DELAY_CE:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_DELAY_MUX:
      switch (pool_sig)
	{
	case OUTPUT_PIN_DELAY_MUX:
	  return(1);
	default:
	  return(0);
	}
    case OUTPUT_PIN_DELAY_MUX:
      switch (pool_sig)
	{
	case INPUT_PIN_DELAY_MUX:
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
    case OUTPUT_PIN_START:
      switch (pool_sig)
	{
	case INPUT_PIN_START:
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
      // Clocking pins
    case INPUT_PIN_RESET:
      switch (pool_sig)
	{
	  // Latch onto an external pin first
	case OUTPUT_PIN_VCC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_CE:
      switch (pool_sig)
	{
	case OUTPUT_PIN_VCC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_CLR:
      switch (pool_sig)
	{
	case OUTPUT_PIN_GND:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_AH:
      switch (pool_sig)
	{
	case OUTPUT_PIN_VCC:
	  return(1);
	default:
	  return(0);
	}
    case INPUT_PIN_AL:
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


///////////////////////////////////////////////////////////////////
// This method refines BWs, sizing estimates, and names utilized by
// each star.
///////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWalg_setup()
{
  // Invoke setup methods for each star.  This should refine BWs and sizing
  // estimates, as determined by the star's smart generator
  int status=0;

  // Loop over all the smart generators
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      StringArray* star_libraries=fpga_core->libs;
      StringArray* star_includes=fpga_core->incs;
      if (!fpga_core->queried)
	{
	  // Bundle up library and include dependencies
	  if (status)
	    {
	      for (int lib_loop=0;
		   lib_loop<star_libraries->population();
		   lib_loop++)
		all_libraries->add(star_libraries->get(lib_loop));
	      for (int inc_loop=0;
		   inc_loop<star_includes->population();
		   inc_loop++)
		all_includes->add(star_includes->get(inc_loop));
	    }

	  fpga_core->sg_resources(UNLOCKED);
	  int row_size=0;
	  int col_size=0;
	  status=(fpga_core->resources)->get_occupancy(row_size, col_size);
	  if (DEBUG_ALGSETUP)
	    if (status)
	      {
		printf("Invoked acs_size on star %s: (%d,%d)\n",
		       fpga_core->comment_name(),
		       row_size,col_size);
	      }

	}
      else
	if (DEBUG_ALGSETUP)
	  printf("sg_setup already invoked for sg %s\n",
		 fpga_core->comment_name());
    }

  // Ensure that precision requirements are met before proceeding to scheduling
  HWset_bw(smart_generators);
}


void ACSCGFPGATarget::HWalg_create(const int fpga_no)
{
  if (DEBUG_ALGCREATE)
    printf("\n\nHWalg_create invoked for galaxy %s\n\n",galaxy()->name());

  // Only generate information for the smart generator at-hand
  Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
  SequentialList* sg_subset=fpga_elem->get_childcores_handle();

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
  open_file << lang->gen_entity(fpga_elem->retrieve_rootfilename(),
				fpga_elem->ext_signals);
  open_file << lang->gen_architecture(fpga_elem->retrieve_rootfilename(),
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
  for (int loop=1;loop<=sg_subset->size();loop++)
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
void ACSCGFPGATarget::HWexport_simulation(SequentialList* smart_generators)
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

  compile_file << "set WF4_BASE " << hw_path->retrieve() << endl;
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
	       << "vmap unisim $COREGEN/unisim" << endl
	       << "vmap WF4 $WF4_BASE/lib/mti" << endl
	       << "vmap work ./mti_" << root_filename << endl
	       << "vmap system ./mti_" << root_filename << endl
	       << "vmap cpe0 ./mti_" << root_filename << "/mti_fpga0" << endl;

  
  char* compile=new char[MAX_STR];
  char* compile_fpga0=new char[MAX_STR];
  char* board_path=new char[MAX_STR];
  strcpy(compile,"vcom -93 -explicit -work work ");
  strcpy(compile_fpga0,"vcom -93 -explicit -work cpe0 ");  
  strcpy(board_path,"$WF4_BASE/src/comp/");

  compile_file << "--" << endl
	       << "--  Compile the VHDL files" << endl
	       << "--" << endl
	       << compile << "$WF4_BASE/src/pkg/wf4comp.vhd" << endl
	       << compile << board_path << "cpe0ife.vhd" << endl
	       << compile << board_path << "cpe0ifa.vhd" << endl
	       << compile << board_path << "pe1ife.vhd" << endl
	       << compile << board_path << "pe1ifa.vhd" << endl
	       << compile << board_path << "pe2ife.vhd" << endl
	       << compile << board_path << "pe2ifa.vhd" << endl
	       << compile << board_path << "pe3ife.vhd" << endl
	       << compile << board_path << "pe3ifa.vhd" << endl
	       << compile << board_path << "pe4ife.vhd" << endl
	       << compile << board_path << "pe4ifa.vhd" << endl
	       << compile << board_path << "cpe0lce.vhd" << endl
	       << endl;

  compile_file << "--" << endl
	       << "-- ACS Controller Interface (standard)" << endl
	       << "--" << endl
	       << compile_fpga0 << "cpe0_counter/FPGA0_UniSeq.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_Start.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_Step.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_Switch.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_Switch_Buffer.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/add21.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_SBuffer.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/add31.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Stat_Buffer.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/reg2.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/Completion_Switch.vhd" << endl
	       << compile_fpga0 << "cpe0_counter/fpga0.vhd" << endl
	       << compile << "./cpe0lca_counter.vhd" << endl << endl;

  compile_file << compile << board_path << "cpe0e.vhd" << endl
	       << compile << board_path << "cpe0a.vhd" << endl;

  /////////////////////////////////////////////////////////////////////////
  // Fix:  Pe0 not encorporated due to its utilization as a host controller
  /////////////////////////////////////////////////////////////////////////
  for (int fpga_no=1;fpga_no<arch->fpga_count;fpga_no++)
    {
      compile_file << "--" << endl;
      Fpga* fpga_elem=arch->get_fpga_handle(fpga_no);
      if (fpga_elem->usage_state==FPGA_USED)
	compile_file << "-- ACS-generated Algorithm for FPGA " << fpga_no << endl
		     << "--" << endl;
      else
	compile_file << "-- No ACS-generated Algorithm for FPGA " << fpga_no << endl
		     << "--" << endl;

      compile_file << compile << board_path << "pe" << fpga_no << "lce.vhd" << endl;
      
      if (fpga_elem->usage_state==FPGA_USED)
	{

	  SequentialList* sg_subset=fpga_elem->get_childcores_handle();
	  for (int sg_loop=1;sg_loop<=sg_subset->size();sg_loop++)
	    {
	      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_subset->elem(sg_loop);
	      if (fpga_core->acs_type==BOTH)
		{
//		  printf("NOVOID:Simulating %s\n",fpga_core->comment_name());
		  // CAUTION: May not need to examine for children, since the FPGA is aware of
		  //          all it's children?
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
		    compile_file << compile << fpga_core->unique_name.str() << ".vhd" << endl;
		  else
		    if (fpga_core->black_box==0)
		      compile_file << compile  
				   << fpga_core->name() << ".vhd" << endl;
		    else
		      compile_file << compile 
				   << fpga_core->lc_name() << ".vhd" << endl;
		}
//	      else
//		printf("VOID:Not simulating core %s\n",fpga_core->comment_name());
	    }
	  compile_file << compile << fpga_elem->retrieve_rootfilename() << ".vhd" << endl;
	  compile_file << compile << "pe" << fpga_no << "lca.vhd" << endl;
	}
      else
	compile_file << compile << board_path << "pe" << fpga_no << "lca.vhd" << endl;
      
      compile_file << compile << board_path << "pe" << fpga_no << "e.vhd" << endl;

      compile_file << "-- Should be obtained from standard directory and copied to your design area" 
		   << endl;
      compile_file << compile << "pe" << fpga_no << "a.vhd" << endl << endl;
    }

  // Misc Wildforce files
  compile_file << compile << "$WF4_BASE/template/vhdl/dsp_card.vhd" << endl
	       << compile << "$WF4_BASE/template/vhdl/mez_card.vhd" << endl
	       << compile << "$WF4_BASE/template/vhdl/ext_io.vhd" << endl
	       << compile << "$WF4_BASE/template/vhdl/simd.vhd" << endl;
  compile_file << compile << board_path << "wf4board.vhd" << endl
	       << compile << board_path << "wf4host.vhd" << endl;
  
  // Particular Annapolis configuration
  compile_file << compile << "./wfhstcfg.vhd" << endl;


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
  delete []board_path;
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
  SequentialList* sg_subset=fpga_elem->get_childcores_handle();

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
  compile_file << "# Precanned design files" << endl
	       << command << "WF4 \"" << hw_path->retrieve() << "/src/pkg/ams4ksyn.vhd\"" << endl
	       << command << "WF4 \"" << hw_path->retrieve() << "/src/pkg/wf4pack.vhd\"" << endl
	       << command << "work \"" << hw_path->retrieve() << "/src/pkg/wf4comp.vhd\"" << endl
	       << command << "work \"" << hw_path->retrieve() 
	       << "/src/comp/pe" << fpga_no << "ife.vhd\"" << endl
	       << command << "work \"" << hw_path->retrieve() 
	       << "/src/comp/pe" << fpga_no << "ifa.vhd\"" << endl
	       << command << "work \"" << hw_path->retrieve() 
	       << "/src/comp/pe" << fpga_no << "lce.vhd\"" << endl
	       << endl;

  compile_file << "# ACS-generated files" << endl;
  for (int loop=1;loop<=sg_subset->size();loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_subset->elem(loop);
      if ((fpga_core->acs_type==BOTH) && (fpga_core->black_box==0))
	if ((fpga_core->unique_component) &&  (fpga_core->declaration_flag))
	  compile_file << command << "work \"" << fpga_core->unique_name.str() << ".vhd" << endl;
	else
	  compile_file << command << "work \"" << fpga_core->name() << ".vhd\"" << endl;
    }
  compile_file << command << "work \"pe" << fpga_no << "lca.vhd\"" << endl
	       << command << "work \"" << fpga_elem->retrieve_rootfilename() << ".vhd\"" << endl
	       << endl;

  compile_file << "# Remainder of precanned design files" << endl
	       << command << "work \"" << hw_path->retrieve() << "/src/comp/pe" 
	       << fpga_no << "e.vhd\"" << endl
	       << command << "work \"pe" << fpga_no << "a.vhd\"" << endl
	       << endl;

  compile_file << "# Compilation/Mapping options" << endl
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
void ACSCGFPGATarget::HWexport_controlinfo(SequentialList* smart_generators)
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
      MemPort* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->portuse==MEMORY_USED)
	total++;
      
      if (mem_port->total_latency > worst_latency)
	{
	  worst_latency=mem_port->total_latency;
	  last_fpga=mem_port->controller_fpga;
	}
    }

  meminfo_file << vector_length << endl;
  meminfo_file << last_fpga-1 << endl;
  meminfo_file << total << endl;
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      MemPort* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->portuse==MEMORY_USED)
	{
	  Port_Timing* port_timing=mem_port->port_timing;
	  for (int loop=0;loop<port_timing->count;loop++)
	    if (port_timing->get_memtype(loop)==SOURCE)
	      {
		SequentialList* src_list=mem_port->source_stars;
		ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) src_list->elem(port_timing->get_memid(loop));
		meminfo_file << mem_loop << " " 
			     << 1 << " "
			     << fpga_core->address_start << " " 
			     << port_timing->get_majorbit(loop) << " "
			     << port_timing->get_bitlen(loop) << endl;
	      }
	    else
	      {
		SequentialList* snk_list=mem_port->sink_stars;
		ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) snk_list->elem(port_timing->get_memid(loop));
		meminfo_file << mem_loop << " "
			     << 0 << " "
			     << fpga_core->address_start << " " 
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




