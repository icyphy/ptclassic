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
 Version: @(#)ACSCGFPGATarget.h	1.6 08/02/01
***********************************************************************/
#ifndef _ACSCGFPGATarget_h
#define _ACSCGFPGATarget_h

#ifdef __GNUG__
#pragma interface
#endif


#include "ACSCGTarget.h"
//JMS
#include "acs_starconsts.h"
#include "ACSIntArray.h"
#include "DoubleArray.h"
#include "StringArray.h"
#include "CoreList.h"
#include "Connection_List.h"
#include "Arch.h"
#include "Port.h"
#include "Pin.h"
#include "acs_vhdl_lang.h"
#include "Sg_Constructs.h"
#include "BindingRule.h"
#include "Directory.h"
#include "ACSPortHole.h"
#include "ACSCGFPGACore.h"
#include <string.h>
#include <strstream.h>
#include <fstream.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <new.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

class Signal_State;
class Port;
class Arch;
class ACSPortHole;
class ACSStar;
class SDFScheduler;
class ACSCGFPGACore;
class VHDL_LANG;
//class Pin;
class Sequencer;
class Connectivity;
class Directory;

class ACSCGFPGATarget : public ACSCGTarget {
  friend class VHDL_LANG;
public:

  ACSCGFPGATarget(const char* name, const char* starclass, const char* desc,
		  const char* category = NULL, 
		  const char* assocDomain = ACSdomainName);

  ~ACSCGFPGATarget();

  // Class identification.
  /*virtual*/ int isA(const char*) const;
 
  // Return a copy of itself
  /* virtual */ Block* makeNew() const;

  // Execution specific
  StringState project_state;
  StringState compile_state;
  StringState hw_state;
  StringState sim_state;
  StringState core_state;
  StringState build_state;
  StringState word_state;
  IntState vlen_state;
  int do_wordlength;
  int do_vhdlbuild;

  // Architecture specific
  Arch* arch;
  static const int NUM_FPGAS=5;
  static const int NUM_MEMS=5;
  static const int NUM_SYSS=3;

  Directory* hw_path;
  Directory* sim_path;
  Directory* core_path;

  // Design specific
  CoreList* smart_generators;
  Directory* design_directory;
  Directory* compile_directory;
  int multirate;
  int automatic_addressing;
  int automatic_wordlength;

  // Dynamic constructor
  Sg_Constructs* construct;

  // For interface with the wordlength analysis tool
  int free_netlist_id;
  
  // Scheduler specific
  int vector_length;
  int seqlen;
  int free_acsid;       // Next free id number for a smart generator instance
  int free_act;         // Next free clock activation number for a given star
  int free_signal;      // Next free inter-algorithmic data bus

  // DEBUG Flags
  // Note:Error flags switched from static const int to const int
  //      in order to resolve compatibility issues with SunCC4.1 compiler.
  //      Switch settings must be set in the .cc file
  char* err_message;
  static const int DEBUG=0;
  static const int DEBUG_TOPLEVEL=0;
  static const int DEBUG_WAIT=0;
  static const int DEBUG_PTPRECISION=0;
  static const int DEBUG_PTCONNECT=0;
  static const int DEBUG_PACK=0;
  static const int DEBUG_SCHEDULER=0;
  static const int DEBUG_IOSCHEDULER=0;
  static const int DEBUG_PIPESYNTH=0;
  static const int DEBUG_PARTITION=0;
  static const int DEBUG_IOADD=0;
  static const int DEBUG_IOREMAP=0;
  static const int DEBUG_PORTS=0;
  static const int DEBUG_DELAYS=0;
  static const int DEBUG_ARCH=0;
  static const int DEBUG_ASSIGNMENTS=0;
  static const int DEBUG_FORK=0;
  static const int DEBUG_FINDPORT=0;
  static const int DEBUG_ALGSETUP=0;
  static const int DEBUG_BWRESOLVER=0;
  static const int DEBUG_RESOLVER=0;
  static const int DEBUG_ALGCREATE=0;
  static const int DEBUG_ALGINTEGRATE=0;
  static const int DEBUG_ALGSUPPORT=0;
  static const int DEBUG_ADDSTAR=0;
  static const int DEBUG_SEQUENCER=0;
  static const int DEBUG_BW=0;
  static const int DEBUG_WORDCOUNT=0;
  static const int DEBUG_PORTSIZE=0;
  static const int DEBUG_REVISE=0;
  static const int DEBUG_WHITESTAR=0;
  
  
  ////////////////////
  // Queried star info
  ////////////////////
  StringArray *all_libraries;
  StringArray *all_includes;


  //////////////////////                                           
  // Function prototypes
  //////////////////////                                           
  void setup(void);
  void HWinitialize(void);
  int HWalg_query(CoreList*);
  void HWdup_component(ACSCGFPGACore*);
  void HWsg_query_params(ACSCGFPGACore*);
  void HWsg_updateprec(ACSCGFPGACore*,
		       StringArray*,
		       int,
		       const char*,
		       const char*);
  void HWtest(ACSCGFPGACore*);
  int HWsg_assign_pins(ACSCGFPGACore*&);
  void HWassign_netlist(Pin*,
			int,
			Pin*,
			int);
  int HWtest_link(ACSCGFPGACore*,int,int);
  
  
  void HWplot_schedule(void);
  void HWannounce_vhdldone(void);
  
  int HWstar_assignids(ACSCGFPGACore*&);
  void HWassign_devices(CoreList*);
  void HWassign_memory(CoreList*);
  void HWassign_fpga(CoreList*);
  int Ptquery_majorbit(const int,
		       ACSCGFPGACore*&);
  int Ptquery_bitlen(const int,
		     ACSCGFPGACore*&);
  int HWpack_ports(CoreList*);
  void HWunirate_sched(void);
  void HWmultirate_sg(CoreList*);
  void HWmultirate_controlpins(ACSCGFPGACore*,
			       ACSIntArray*);

  int HWsynth_pipealigns(void);
  int HWadd_delayto(ACSCGFPGACore*,
		    ACSCGFPGACore*,
		    int,
		    int,
		    int,
		    int,
		    const int);
  int HWadd_delaychain(ACSCGFPGACore*,int,int,
		       ACSCGFPGACore*,int,int,
		       int,ACSIntArray*,
		       int,int);
  
  int HWinsert_sg(ACSCGFPGACore*,
		  ACSCGFPGACore*,
		  ACSCGFPGACore*,
		  int,int,const int,
		  int,int,const int);
  int HWinsert_sg(ACSCGFPGACore*,
		  ACSCGFPGACore*,
		  Pin*,
		  int,int,const int,
		  int,int,const int);
  int HWall_scheduled(void);
  void HWio_sched(void);


  void HWsequencer(void);
  void HWmultirate_sequencer(Fpga*,const int);
  
  void HWbw_resolver(void);
  void HWlink_resolver(Pin*,Pin*,Pin*,Pin*,
		       ACSCGFPGACore*,Pin*,int,Connectivity*,int);
  void HWalg_resolver(CoreList*);
  void HWresolve_drivingsignals(CoreList*, const int);
  void HWresolve_ictrlsignals(CoreList*);
  void HWresolve_ectrlsignals(void);
  void HWresolve_ctrlpins(Fpga*,ACSCGFPGACore*,Pin*,ACSIntArray*);

  void HWsig_compare(const int,Pin*,const int,Pin*,const int,Pin*);
  void HWsig_compare_ext(const int,Pin*,const int,Pin*,const int,Pin*);
  int HWresolver_extrules_driving(int,int);
  int HWresolver_extrules_driven(int,int);
  int HWresolver_ctrlrules(const int,const int,const int,const int);
  int HWresolver_construles(int,int);
  void HWmacro(CoreList*);
  void HWalg_setup(CoreList*);
  void HWwhite_star(ACSCGFPGACore*,CoreList*);
  void HWdark_star(ACSCGFPGACore*,CoreList*);
  void HWlib_needs(CoreList*);
  void HWalg_create(const int);
  void HWalg_integrate(const int);
  void HWexport_simulation(CoreList*);
  void HWexport_synthesis(Fpga*,const int);
  void HWexport_controlinfo(CoreList*);
  void HWcleanup(void);

  //////////////////////////////////////////////////////////////////////////////
  // HWio_add.cc
  //////////////////////////////////////////////////////////////////////////////
  ACSIntArray* HWpack_bits(ACSIntArray*, const int, CoreList*);
  void HWadd_packer(Port*, const int, const int, const int, CoreList*, ACSIntArray*);
  int HWadd_support(void);
  int HWsegment_alg(CoreList*);
  int HWreturn_fpgadevice(ACSCGFPGACore*);
  int HWio_add(CoreList*);
  void HWio_add_sequencer(Fpga*);
  void HWio_add_addressgen(Fpga*, Port*, const int);
  void HWio_add_dataio(Fpga*, Port*, const int);
  void HWio_add_bidirio(Fpga*, 
			 Port*, const int);
  void HWio_add_dualio(Fpga*,
			Port*, const int,
			const int);
  void HWio_reconnect_src(ACSCGFPGACore*, ACSCGFPGACore*, const int);
  void HWio_reconnect_snk(ACSCGFPGACore*, ACSCGFPGACore*, const int);
  void HWio_add_reconnect(ACSCGFPGACore*,
			  ACSCGFPGACore*,
			  Pin*,
			  int,
			  int,
			  int,
			  Port*);
  void HWio_remaplut(ACSCGFPGACore*);
  void HWio_add_wordcounter(Fpga*,
			    CoreList*&,
			    const int,
			    ACSCGFPGACore*&,const char*,const int,
			    ACSCGFPGACore*&,const char*,
			    const int, const int);
  void HWremap_ioport(ACSCGFPGACore*,CoreList*);

  ////////////////////////////////////////////////////////////////////////////
  // HWutils.cc
  ////////////////////////////////////////////////////////////////////////////
  void memory_monitor(void);
  ACSCGFPGACore* fetch_fpgacore(Star *);
  ACSCorona* fetch_corona(Star *);
  ACSCGFPGACore* HWfind_star(int);
  void HWset_wordcount(CoreList*);
  void HWset_bw(CoreList*);
  void HWset_requirements(ACSCGFPGACore*);
  Pin* HWfind_origin(int,int);
  CoreList* HWfind_loop(ACSCGFPGACore*);
  int HWall_bwset(CoreList*);
  int HWall_wordset(CoreList*);
  void HWset_dirty(ACSCGFPGACore*);
  int bselcode_old(int,int);
  void HWdisplay_connectivity(char*,CoreList*);
  void HWdisplay_connects(Pin*);
  int intparam_query(const char*);
  int HWtest_contentions(void);
  int HWtest_contention(Pin*);


  ////////////////////////////////////////////////////////////////////////////
  // HWio_matlab.cc
  ////////////////////////////////////////////////////////////////////////////
  int HWnetlist_export(void);
  void HWexport_wgscript(void);
  char* HWexport_netversion(void);
  char* HWexport_net(ACSCGFPGACore*,
		     int,
		     const int);
  char* HWexport_pnames(void);
  char* HWexport_delaychain(void);
  int HWpause_for_wordgui(void);

  // Post-matlab update of the algorithm
  void HWrevise(CoreList*);
  int HWnetlist_import(void);
  int HWrevise_const(CoreList*);
  int HWrevise_fixed(CoreList*);
  int HWrevise_pins(ACSCorona&,
		    ACSCGFPGACore*,
		    ACSIntArray*,
		    StringArray*,
		    int,
		    char,
		    ofstream&);
  int HWrevise_pipes(CoreList*);
};

#endif



