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
 Version: @(#)ACSCGFPGACore.h	1.4 10/30/99
***********************************************************************/
#ifndef _ACSCGFPGACore_h
#define _ACSCGFPGACore_h

#ifdef __GNUG__
#pragma interface
#endif

// JMS
#include "HWSchedule.h"
#include "StringArray.h"
#include "Pipealign.h"
#include "acs_starconsts.h"
#include "acs_vhdl_lang.h"

#include "ACSCGCore.h"
#include "ACSWormhole.h"
class Pin;
class Capability;
class Constants;
class Connectivity;
class Resource;
class Pipealign;
class VHDL_LANG;


// Global category string
extern const char ACSCGFPGACategory[];

class ACSCGFPGACore : public ACSCGCore {
public:

  
  // JMS
  /*virtual*/ int isA(const char*) const;
  
  // This is the default constructor for the FPGA core category.
  ACSCGFPGACore(void);
  ACSCGFPGACore(const char* category);

  // This is the flavor of constructor used by ACSCore* makeNew( ACSCorona & ).
  ACSCGFPGACore(ACSCorona& corona_);

  ~ACSCGFPGACore();

  // JMS
  // A unique identifier to this core smart generator
  int acs_id;
  char* comment;

  // Most of these attributes are intermediary between the Ptolemy
  // IO windows and the smart generators.  Refer to the Pins.
  int acs_type;     // SOURCE/SINK/BOTH, BOTHs are used for algorithms!!
  int acs_domain;
  int acs_fpga;     // A numeric identifier specifying where the sg will reside
  int acs_existence;
  int acs_speed;
  int sg_language;
  int acs_hwport;   // Indicates which I/O port this star is associated
		    // Only valid for SOURCE/SINK stars

  // Hierarchy related
  SequentialList* child_sgs;					      

  // Compilation related
  char* dest_dir;
  StringArray* child_filenames;
  int black_box;
  int bitslice_strategy;

  // JMS
  // Scheduler-related
  int acs_addract;  // Address activation time as dictated by the scheduler
  int acs_dataact;  // Data activation time as dicated by the scheduler
  int acs_delay;    // Delay amount as determined by the smart generator
  int alg_delay;    // Delay amount that is dictated by the algorithm
  int pipe_delay;   // Delay amount that is dictated by the scheduler
  Pipealign* pipe_alignment;

  // JMS
  // Partitioning-related

  // JMS
  // Delay class - handles intermediate delays assigned by the scheduler
  //               NOT the delays generated by the smart generator!
  //               ASSUMPTION:delays are contiguous
  int acsdelay_count;
  ACSIntArray* acsdelay_ids;
 
  // JMS
  // Data class - handles I/O information concerning values
  int acsdata_address;
  int acsdata_rsize;
  int acsdata_csize;
 
  // JMS
  // Setup related
  int initialized;
  int queried;

  // JMS
  // Mechanical growth related
  int bw_dirty;
  int bw_exempt;
  int bw_loop;
 
  // JMS
  // Resolver related

  // JMS
  // Smart generator working variables
  int sign_convention;          //FIX:Pin specific?
  int total_count;
  int input_count;              // Requests for parameterized port sizes
  int output_count;
  int control_count;
  int bidir_count;
  StringArray* libs;
  StringArray* incs;
  Pin* pins;
  Pin* ext_signals;
  Pin* data_signals;
  Pin* ctrl_signals;
  Pin* constant_signals;
  Resource* resources;
  Capability* sg_capability;   // Store language capability information
  Constants* sg_constants;     // Constant information
  VHDL_LANG* lang;             //FIX: For now default to VHDL
  // Smart generator working variables for cost
  char* root_filename;
  char* cost_filename;
  char* numsim_filename;
  char* rangecalc_filename;
  char* natcon_filename;

  // Core-based methods
  char* comment_name(void);
  char* name(void);
  char* lc_name(void);
  int vector_length(double);
  int intparam_query(const char*);
  void intparam_set(const char*,int);

  // Smart generator-based methods:
  int sg_initialize(void);
  int sg_initialize(char*,
		    int,
		    int*);
  int sg_costfunc(int);
  int bits_overlap(const int, const int,
		   const int, const int);
  char* tolowercase(char*);

  virtual int sg_cost(ofstream&,
		      ofstream&,
		      ofstream&,
		      ofstream&) { 
    printf("WARNING:sg_cost not implemented for star %s\n",name());
    return(0);
  };
  virtual int sg_resources(int) { 
    printf("WARNING:sg_resources not implemented for star %s\n",name());
    return(0);
  };
  virtual int sg_setup(void) {
    printf("WARNING:sg_setup not implemented for star %s\n",name());
    return(0);
  };
  virtual int sg_param_query(SequentialList*,SequentialList*) {
    printf("WARNING:sg_param_query not implemented for star %s\n",name());
    return(0);
  };
  virtual int macro_query(void) {
    printf("WARNING:macro_query not implemented for this star %s\n",name());
    return(0);
  };
  virtual int macro_build(int*) {
    printf("WARNING:macro_build not implemented for this star %s\n",name());
    return(0);
  };
  virtual int acs_build(void) {
    printf("WARNING:acs_build not implemented for this star %s\n",name());
    return(0);
  };

//JR
//add definitions for virtuals declared in ACSCGCore	
	/* virtual */ StringList setargStates(Attribute a=ANY);

protected:
	// Generate declarations for PortHoles and States.
        /* virtual */ StringList declareBuffer(const ACSPortHole*) ;
        /* virtual */ StringList declareOffset(const ACSPortHole*) ;
        /* virtual */ StringList declareState(const State*) ;
 
        // Generate declaration, initilization and function
        // codes for command-line settable states.
        /* virtual */ StringList cmdargState(const State*) ;
        /* virtual */ StringList cmdargStatesInit(const State*) ;
        /* virtual */ StringList setargState(const State*) ;
        /* virtual */ StringList setargStatesHelp(const State*) ;
 
        // Generate initialization code for PortHoles and States.
        /* virtual */ StringList initCodeBuffer(ACSPortHole*) ;
        /* virtual */ StringList initCodeOffset(const ACSPortHole*) ;
        /* virtual */ StringList initCodeState(const State*) ;
 
        /* virtual */ StringList portBufferIndex(const ACSPortHole*) ;
        /* virtual */ StringList portBufferIndex(const ACSPortHole*, const State
*, const char*) ;
 
        /* virtual */ StringList arrayStateIndexRef(const State*, const char*) ;
};


// JMS-added
// class CGStarMPHIter : public BlockMPHIter {
// public:
//         CGStarMPHIter(CGStar& s) : BlockMPHIter(s) {}
//         CGPortHole* next() { return (CGPortHole*)BlockMPHIter::next();}
//         CGPortHole* operator++(POSTFIX_OP) { return next();}
//};

#endif
