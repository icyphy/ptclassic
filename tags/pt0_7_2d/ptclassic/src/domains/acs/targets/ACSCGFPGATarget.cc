static const char file_id[] = "ACSCGFPGATarget.cc";
/**********************************************************************
Copyright (c) 1998-2001 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
                                                        COPYRIGHTENDKEY


 Programmers:  Ken Smith (Sanders), Eric Pauer (Sanders), Christopher Hylands, Edward A. Lee
 Date of creation: 1/15/98
 Version: @(#)ACSCGFPGATarget.cc	1.5 08/02/01

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>             // Pick up R_OK for SunOS4.1 cfront
#include <ctype.h>              // Pick up isspace
#include "ACSCGFPGATarget.h"
#include "CGUtilities.h"
#include "ACSStar.h"
#include "GalIter.h"
#include "miscFuncs.h"
#include "dataType.h"
#include "EventHorizon.h"
#include "SDFScheduler.h"
#include "compat.h"             // Pickup symlink()
#include "InfString.h"          // Pick up InfString for use in pragma()
#include "ConversionTable.h"
#include "KnownTarget.h"
#include "pt_fstream.h"

ISA_FUNC(ACSCGFPGATarget,ACSCGTarget);
 
static ACSCGFPGATarget targ("ACS-CGFPGA","ACSStar",
        "A target for FPGA design generation  using Adaptive Computing System
Coronas and Cores", "CGFPGA");
 
static KnownTarget entry(targ,"ACS-CGFPGA");

ACSCGFPGATarget::ACSCGFPGATarget(const char* name, const char* starclass,
            const char* desc, const char* category, const char* assocDomain) :
            ACSCGTarget(name, starclass, desc, category, assocDomain) 
{
  Block::addState(project_state.setState("FPGA output directory",this,"",
				       "Complete path for FPGA target results"));
  Block::addState(compile_state.setState("FPGA compile directory",this,"",
					 "BASE path for VHDL/Xilinx compilation, if same as output directory leave blank"));
  Block::addState(hw_state.setState("Annapolis Wildforce Architecture library path",this,"",
				       "Complete path to libraries"));
  Block::addState(sim_state.setState("VHDL Simulation path",this,"",
				       "Complete path for FPGA simulation"));
  Block::addState(core_state.setState("Coregen path",this,"",
				      "Complete BASE path to the Xilinx xul directory"));
  Block::addState(build_state.setState("Generate VHDL",this,"Yes",
				       "Yes/No, to allow sgs to make VHDL"));

  Block::addState(word_state.setState("Export and pause for wordlength analysis",
				      this,"Yes",
				      "Yes/No, to allow netlist to be exported"));
  Block::addState(vlen_state.setState("Number of words to process",this,"-1",
				      "Numeric entry for total number of iterations, default setting will automatically calculate this value"));

  if (DEBUG_TOPLEVEL)
    printf("ACSCGFPGATarget:ACSCGFPGATarget constructor invoked\n");
}

ACSCGFPGATarget :: ~ACSCGFPGATarget() {
  if (DEBUG_TOPLEVEL)
    printf("ACSCGFPGATarget:ACSCGFPGATarget destructor invoked\n");
}

// clone
Block* ACSCGFPGATarget :: makeNew () const {
  if (DEBUG_TOPLEVEL)
    printf("ACSCGFPGATarget::makeNew invoked\n");
  LOG_NEW;
  return new ACSCGFPGATarget(name(), starType(), descriptor(), getAssociatedDomain());
}

void ACSCGFPGATarget::setup() {
  if (DEBUG_TOPLEVEL)
    printf("ACSCGFPGATarget::setup invoked\n");
 
  /////////////////////////
  // Determine architecture
  /////////////////////////
  printf("Initializing...");
  HWinitialize();
  construct=new Sg_Constructs(&free_acsid,&free_netlist_id,design_directory->retrieve_extended());
  printf("Done\n");
 
  myCode.initialize();
  makefile.initialize();
  makeAllList.initialize();
  
  // This only initializes the streams owned by 'codeStringLists'
  codeStringLists.initialize();
  
  
  if (!scheduler()) chooseScheduler();
  
  if (!galaxySetup()) return;
  if (haltRequested()) return;
  if (filePrefix.null() && galaxy()) filePrefix = galaxy()->name();
  
  
  // Reset the symbol lists.
  //  counter = 0;
  targetNestedSymbol.setCounter(symbolCounter());
  targetNestedSymbol.initialize();
  sharedSymbol.setCounter(symbolCounter());
  sharedSymbol.initialize();
  
  // be sure that this routine does not initialize the galaxy if it
  // is a child target. It is already initialized, and done more!
  if (!parent()) {
    if (!galaxy() || haltRequested()) return;
    galaxy()->initialize();
    if (!modifyGalaxy()) return;
  }
  
  if (inWormHole()) wormPrepare();
  
  // error checking
  if (!noSchedule) {
    if(!schedulerSetup()) return;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Precedence determined, conduct queries and generate internal representation
  //////////////////////////////////////////////////////////////////////////////
  printf("Interpreting algorithm from Ptolemy graph....");
  int rc_aq=HWalg_query(smart_generators);
  if (rc_aq==0)
    return;
  if (haltRequested()) return;
  printf("Done.\n");

  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after alg_query",smart_generators);

  ////////////////////////////////////////////////////////////////
  // Allocate local core storage, assign default settings and pins
  ////////////////////////////////////////////////////////////////
  printf("Setting up algorithm....");
//  HWmacro(smart_generators);
  HWalg_setup(smart_generators);
  printf("Done.\n");
  if (haltRequested()) return;
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after alg_setup",smart_generators);
 
  //////////////////////////////////////
  // Revise word lengths (if applicable)
  //////////////////////////////////////
  if (do_wordlength)
    {
      printf("Exporting netlist for wordlength analysis....");
      HWnetlist_export();
      printf("Done.\n");
      int rc=HWpause_for_wordgui();
      if (rc==0)
	{
	  printf("Revising precisions and schedule....");
	  HWrevise(smart_generators);
	  printf("Done\n");
	}
      if (rc!=0)
	{
	  do_vhdlbuild=0;
	  fprintf(stderr,"Build cancelled\n");
	}

      if (haltRequested()) return;
    }
  if (!do_vhdlbuild)
    return;

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after wordlength analysis");

  ///////////////////////////////////////////////////
  // Allocate resources and initially schedule inputs
  ///////////////////////////////////////////////////
  printf("Revising algorithm to device mapping....");
  HWassign_devices(smart_generators);
  printf("Done.\n");
  if (haltRequested()) return;
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after assign_memory",smart_generators);

  /////////////////
  // Add partitions
  /////////////////
  printf("Segmenting algorithm into individual fpgas (if applicable)....");
//  construct=new Sg_Constructs(&free_acsid,&free_netlist_id,design_directory->retrieve_extended());
  HWsegment_alg(smart_generators);
  printf("Done.\n");
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after HWsegment_alg",smart_generators);

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after HWsegment_alg");

  ///////////////////////
  // Test for contentions
  ///////////////////////
  int rc_contention=HWtest_contentions();
  if (rc_contention==0)
    return;

  ////////////////////////////////////////////////////
  // Smart generator info derived, now schedule
  // FIX: Not needed, once wordgui supports scheduling
  ////////////////////////////////////////////////////
  if (!multirate)
    {
      printf("Scheduling algorithm....");
      HWunirate_sched();
      printf("Done.\n");
      if (haltRequested()) return;
      if (DEBUG_TOPLEVEL)
	HWdisplay_connectivity("after unirate_sched",smart_generators);
    }
//  else
//    HWset_wordcount(smart_generators);

  /////////////////////////////////////////////////////////////////////////
  // Detect for smart generators that are memory-based and prepare them for
  // proper phase execution
  /////////////////////////////////////////////////////////////////////////
  HWmultirate_sg(smart_generators);

  ////////////////////////////////////////////////////
  // Check for I/O conflicts and adjust
  // FIX: Not needed, once wordgui supports scheduling
  ////////////////////////////////////////////////////
  if (!multirate)
    {
      printf("Scheduling IO....");
      HWio_sched();
      printf("Done.\n");
      if (haltRequested()) return;
      if (DEBUG_TOPLEVEL)
	HWdisplay_connectivity("after io_sched",smart_generators);
    }

  //////////////////////////////////////////////////////////////////////
  // Scheduling completed, now align algorithm with pipe delays
  // FIX: Merge with HWadd_support(), once wordgui directs additional HW
  //////////////////////////////////////////////////////////////////////
  printf("Generating pipes....");
  HWsynth_pipealigns();
  printf("Done.\n");
  if (haltRequested()) return;
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after synth_pipealigns",smart_generators);

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after HWsynth_pipealigns");

  /////////////////////////////////////////////////////////////////////////////
  // Look for any opportunities to pack bits in order to reduce overall latency
  // REVISIT:Currently wordgui tool will not accomodate port packing
  /////////////////////////////////////////////////////////////////////////////
  printf("Examining IO ports for bit packing constraints....");
  int rc_overload=HWpack_ports(smart_generators);
  if (rc_overload==0)
    {
      printf("IO port overloaded, cannot complete code generation\n");
      return;
    }
  printf("Done.\n");
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after HWpack_ports",smart_generators);

  ////////////////////
  // Plot the schedule
  ////////////////////
//  HWplot_schedule();

  //////////////////////////////////////////////////////////////////////
  // Now add the support smart generators for data sorting, address
  // generation, and word length counting
  // FIX: Merge with HWadd_support(), once wordgui directs additional HW
  //////////////////////////////////////////////////////////////////////
  printf("Adding IO elements....");
  int rc_ioadd=HWio_add(smart_generators);
  if (rc_ioadd==0)
    return;
  if (haltRequested()) return;
  printf("Done.\n");
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after_io_add",smart_generators);

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after HWio_add");

  ///////////////////////
  // Test for contentions
  ///////////////////////
  int rc_contention2=HWtest_contentions();
  if (rc_contention2==0)
    return;

  /////////////////////////////////////////////////////////////////////////
  // Since stars have been dynamically added, we need to setup the Universe 
  // again
  /////////////////////////////////////////////////////////////////////////
  galaxySetup();
  HWmacro(smart_generators);
  HWalg_setup(smart_generators);
  if (haltRequested()) return;

  /////////////////////////////////////////////
  // Generate sequencers for each utilized FPGA
  /////////////////////////////////////////////
  // FIX:Simply adds a sequencer core, should reduce
  HWadd_support();

  if (DEBUG_ASSIGNMENTS)
    arch->print_arch("after HWadd_support");

  printf("Generating sequencer HW....");
  HWsequencer();
  printf("Done.\n");
  if (haltRequested()) return;
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("after HWunirate_sequencer",smart_generators);

  ///////////////////////////////////////////////////////////////////////////////
  // Since stars have been dynamically added, we need to setup the Universe again
  ///////////////////////////////////////////////////////////////////////////////
  galaxySetup();
  HWmacro(smart_generators);
  HWalg_setup(smart_generators);
  if (haltRequested()) return;

  ///////////////////////////
  // Now propagate bit widths
  ///////////////////////////
  printf("Propagating bitwidths....");
  HWset_bw(smart_generators);
  if (haltRequested()) return;
  printf("Done.\n");

  ////////////////////////////////////////////////////////////
  // Determine and bitwidth mismatches and remedy with buffers
  ////////////////////////////////////////////////////////////
  printf("Resolving bitwidths....");
  HWbw_resolver();
  if (haltRequested()) return;
  printf("Done.\n");

  ////////////////////////////////////////////////
  // Conduct automatic linking of dangling signals
  ////////////////////////////////////////////////
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("pre-resolver",smart_generators);
  printf("Resolving all unconnected signals....");
  HWalg_resolver(smart_generators);
  printf("Done.\n");
  if (haltRequested()) return;
//  schedulerSetup();
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("post-resolver",smart_generators);

//  return;

  /////////////////////
  // Now build all code
  /////////////////////
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("pre-alg_create",smart_generators);
  printf("Invoking all smart generators to build VHDL:\n");
  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      Fpga* curr_fpga=arch->get_fpga_handle(fpga_no);
      if (curr_fpga->usage_state==FPGA_USED)
	{
	  printf("\nBuilding FPGA %d\n",fpga_no);
	  HWalg_create(fpga_no);
//	  HWalg_integrate(fpga_no);
	}
      else
	printf("\nNot building FPGA %d\n",fpga_no);
    }
  printf("Building FPGAs....Done.\n");
  if (haltRequested()) return;
  if (DEBUG_TOPLEVEL)
    HWdisplay_connectivity("post-alg_create",smart_generators);

  ///////////////////////////////////////////////////////////////////////////
  // Export support files for simulation, compiling, and runtime environments
  ///////////////////////////////////////////////////////////////////////////
  printf("Exporting support files...");
  HWexport_simulation(smart_generators);
  HWexport_controlinfo(smart_generators);
  for (int fpga_no=0;fpga_no<arch->fpga_count;fpga_no++)
    {
      Fpga* curr_fpga=arch->get_fpga_handle(fpga_no);
      if (curr_fpga->usage_state==FPGA_USED)
	HWexport_synthesis(curr_fpga,fpga_no);
    }
  printf("Done.\n");
  if (haltRequested()) return;

  //////////////////////////
  // Completion:wrapup stuff
  //////////////////////////
  HWannounce_vhdldone();
  HWcleanup();
  printf("Stitching complete\n");
 
  if (haltRequested()) return;
  
  // reset for next setup
  noSchedule = 0;
  
  // If in a WormHole, generate, compile, load, and run code.
  // Ignore flags which may otherwise disable these functions.
  //  if (inWormHole() && alone()) {
  if (inWormHole()) {
    adjustSampleRates();

// JMS
//    generateCode();
    if (haltRequested()) return;
    if (compileCode()) {
      if (loadCode()) {
        if (!runCode())
          Error::abortRun(*this, "could not run!");
      }
      else {
        Error::abortRun(*this, "could not load!");
      }
    }
    else {
      Error::abortRun(*this, "could not compile!");
    }
  }
}





