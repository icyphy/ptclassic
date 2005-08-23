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
 Version: @(#)HWSchedule.h	1.6 08/15/00
***********************************************************************/
#ifndef HWSCHED
#define HWSCHED

#ifdef __GNUG__
#pragma interface
#endif

#include <strstream.h>
#include <limits.h>

#include "Target.h"
#include "IntState.h"
#include "StringState.h"
#include "SymbolList.h"
#include "SimControl.h"
#include "CodeStreamList.h"
#include "StringList.h"
#include "DataFlowStar.h"
#include "ImplementationCost.h"
#include "ACSCGFPGATarget.h"
#include "ConversionTable.h"

#include "ACSIntArray.h"
#include "DoubleArray.h"
#include "StringArray.h"
#include "Sg_Constructs.h"
#include "Port_Timing.h"
#include "Connectivity.h"
#include "Delay.h"

class ACSCGFPGATarget;
class ACSStar;
class SDFSchedule;
class SDFScheduler;
class ACSCGCStar;
class ACSCGFPGACore;
class Port_Timing;
class Sg_Constructs;

static const int DEBUG=0;
static const int DEBUG_VHDL=0;
static const int DEBUG_MEMPORT=0;
static const int DEBUG_PIN=0;
static const int DEBUG_CONST=0;


class MEMPort {
public:
  int total_sgs;
  SequentialList* source_stars;
  SequentialList* sink_stars;
  ACSCGFPGACore* dataimux_star;
  ACSCGFPGACore* dataomux_star;
  ACSCGFPGACore* addrmux_star;
  SequentialList* const_stars;
  ACSCGFPGACore* addrgen_star;
  ACSCGFPGACore* addrbuf_star;
  Port_Timing* port_timing;
  int data_size;
  int addr_size;
  int addr_lo;
  int addr_hi;
  int portuse;
  int bidir_data;      // True if memory port is bidirectional
  int separate_rw;     // True of read/write control signal for memories are separate
  int read_skew;      // Number of clocks that a read request is delayed by
  int write_skew;      // Number of clocks that a write request is delayed by

  // Signal naming pointers
  int datain_signal_id;   // These ids are pointers to the ext_signal
  int dataout_signal_id;  // These ids are pointers to the ext_signal
  int addr_signal_id;     // that contains details about these signals
  int read_signal_id;
  int write_signal_id;
  int enable_signal_id;

  // Timing info
  int current_act;
  int pt_count;                     
  ACSIntArray* mem_timing;
  int sequence_overflow;

  // Names for signals
  char* datain_name;
  char* dataout_name;
  char* addr_name;
  char* read_name;
  char* write_name;
  char* enable_name;
							       
public:
  MEMPort();
  ~MEMPort();
  int init_pt(int);
  int add_pt(int,int);
  int fetch_pt(int);
  int assign_srccore(ACSCGFPGACore*);
  int assign_snkcore(ACSCGFPGACore*,int);
};


// Pin constants
static const int LOCKED=1;
static const int UNLOCKED=0;

// Pin limits
static const int INF=INT_MAX/4;

// Pin priorities
static const int NO_PRIORITY=-1;

class Pin {
  friend class ACSCGFPGATarget;
  friend class ACSCGFPGACore;
  friend class HWSchedule;
  friend class Connectivity;

 public:
  int pin_count;
  ACSIntArray* major_bit;
  ACSIntArray* vector_length;
  ACSIntArray* min_vlength;
  ACSIntArray* max_vlength;
  ACSIntArray* word_lock;      // Used by Word Length analysis tool
  ACSIntArray* prec_lock;      // Used by FPGA-target for mechanical growth
  ACSIntArray* netlist_ids;    // Netlist identifiers for algorithmic connections

  ACSIntArray* data_type;
  ACSIntArray* pin_type;
  ACSIntArray* pin_assigned;
  ACSIntArray* pin_limit;
  ACSIntArray* pin_priority;
  StringArray* pin_name;
  StringArray* corona_pinname;
  Connectivity** connect;
  Delay** delays;
  SequentialList* inode_pins;
  SequentialList* onode_pins;
  SequentialList* cnode_pins;

  public:
  Pin();
  ~Pin();
  Pin& operator=(Pin&);
  void update_pins();
  SequentialList* classify_datapins(int);
  SequentialList* classify_controlpins(void);
  int add_pin(char*,const int);
  int add_pin(char*,char*,const int);
  int add_pin(char*,const int,const int,const int);
  int add_pin(char*,const int,const int,const int,const int);
  int new_pin(void);
  int set_pinpriority(const int,
			   const int);
  int set_pinlim(const int,
		      const int);
  int set_pintype(const int,
		       const int);
  int set_datatype(const int,
			const int);
  int set_min_vlength(const int,
			   const int);
  int set_max_vlength(const int,
			   const int);
  int set_precision(const int,
			 const int,
			 const int,
			 const int);
  int setmax_precision(const int,
			    const int,
			    const int,
			    const int);
  int set_wordlock(const int,
			const int);
  int set_netlistid(const int,
			 const int);
  int query_pintype(const int);
  int query_majorbit(const int);
  int query_bitlen(const int);
  int query_minbitlen(const int);
  int query_maxbitlen(const int);
  int query_wordlock(const int);
  int query_preclock(const int);
  int query_netlistid(const int);
  int query_pinassigned(const int);

  char* retrieve_pinname(const int);
  char* retrieve_cpinname(const int);
  int prefix_string(const char*);

  int pintype(const int);
  int req_pintype(const int);
  int retrieve_type(const int);
  int retrieve_type(const int,
			 const int);
  void print_pins(char*);
  int free_pintype(const int);
  int connect_pin(const int,
		       const int,
		       const int,
		       const int);
  int connect_pin(const int,
		       const int,
		       const int,
		       const int,
		       const int);
  int assign_pin(const int,
		      const int,
		      const int,
		      const int);
  int match_acstype(const int,
			 const int);
  int match_acsnode(const int,
			 const int);
  int disconnect_pin(const int,
			  const int);
  int disconnect_allpins(const int);
  int bw_exclude(void);
  int pin_withname(const char*);
  int pin_withnetlist(const int);
  int reclassify_pin(const int,
			  const int);
};



class Capability {
  friend class ACSCGFPGATarget;
  friend class HWSchedule;

 public:
  StringList* domains;
  StringList* architectures;
  SequentialList* languages;

 public:
  Capability();
  ~Capability();
  Capability& operator=(const Capability&);
  char* retrieve_string(StringList*,int);
  int add_domain(char*);
  int add_architecture(char*);
  int add_language(int);
  int get_language(int);
};




class Constants 
{
public:
  int count;
  void** storage;
  ACSIntArray* types;

public:
 Constants(void);
 ~Constants(void);
 void add_double(double);
 void add_float(float);
 void add_long(long);
 void add_int(int);
 void add(void*,
		     const int);
 void add_storage(void*);
 int get_int(int);
 long get_long(int);
 float get_float(int);
 double get_double(int);

 char* query_bitstr(int,
			       int,
			       int);
 char* query_str(int,
			    int,
			    int);
 int  query_bitsize(int);
};


class Sequencer
{
public:
  ACSCGFPGACore* seq_sg;
  SequentialList* wc_consts;
  ACSCGFPGACore* wc_mux;
  ACSCGFPGACore* wc_add;
  
  // Currently attached to Memport
       //SequentialList* addr_consts;
  //ACSCGFPGACore* addr_mux;
  //ACSCGFPGACore* addr_add;
  
public:
  Sequencer(void);
  ~Sequencer(void);
};


class Resource
{
 public:
  // FIX: For now the size will simply represent clb size in a rectangular sense
  //      This should become a derivable class based on domain and technology type
  int row;
  int col;


public:
 Resource(void);
 ~Resource(void);
 int set_occupancy(int,int);
 int get_occupancy(int,int);
};


#endif




















