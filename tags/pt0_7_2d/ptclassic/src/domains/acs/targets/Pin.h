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
 Version: @(#)Pin.h      1.0     06/16/99
***********************************************************************/
#ifndef PIN_H
#define PIN_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "ACSIntArray.h"
#include "StringList.h"
#include "StringArray.h"
#include "Connectivity.h"
#include "Delay.h"
#include "acs_starconsts.h"

// Debug switches
static const int DEBUG_PIN=0;
static const int DEBUG_PIN_ADD=0;
static const int DEBUG_PIN_OPERATOR=0;
static const int DEBUG_PIN_CONNECT=0;
static const int DEBUG_PIN_PRECISION=0;
static const int DEBUG_PIN_MATCH=0;

// Pin constants
static const int LOCKED=1;
static const int UNLOCKED=0;

// Pin alternate name (used if there won't be an alternate)
static const char NO_ALTERNATE[]="NO_ALTERNATE_PIN";

// Pin limits
static const int INF=INT_MAX/4;

// Pin priorities
static const int NO_PRIORITY=-1;

// Signal directivity constants
// Assign in groups:input pin group, output pin group, and bidir pin group
// NOTE:Update LANG::set_port with any updated pin groups!
// FIX:This should be broken down into classes, with attributes that could help
//     arbitrate amongst multiple INPUT_PIN_CLK types (etc.)
static const int INPUT_PIN=1;
static const int INPUT_PIN_CARRY=2;
static const int INPUT_PIN_MODE=3;
static const int INPUT_PIN_CLK=4;
static const int INPUT_PIN_RESET=5;
static const int INPUT_PIN_CTRL=6;
static const int INPUT_PIN_CE=7;
static const int INPUT_PIN_CLR=8;
static const int INPUT_PIN_GO=9;
static const int INPUT_PIN_EXT=10;
static const int INPUT_PIN_MEMOK=11;
static const int INPUT_PIN_SRC_WC=12;
static const int INPUT_PIN_SRC_MUX=13;
static const int INPUT_PIN_SRC_CE=14;
static const int INPUT_PIN_MEMORYMUX=15;
static const int INPUT_PIN_ADDRMUX=16;
static const int INPUT_PIN_ADDRCE=17;
static const int INPUT_PIN_ADDRCLR=18;
static const int INPUT_PIN_EXTCTRL=19;
static const int INPUT_PIN_MEMORY=20;
static const int INPUT_PIN_DMUX=21;
static const int INPUT_PIN_MUX_SWITCHABLE=22;
static const int INPUT_PIN_MUX_RESULT=23;
static const int INPUT_PIN_PRIMARY=24;
static const int INPUT_PIN_SECONDARY=25;
static const int INPUT_PIN_PHASE=26;
static const int INPUT_PIN_INTERCONNECT=27;
static const int INPUT_PIN_INTERCONNECT_ENABLE=28;
static const int INPUT_PIN_SNK_CE=29;
static const int INPUT_PIN_SNK_MUX=30;
static const int INPUT_PIN_SNK_WC=31;
static const int INPUT_PIN_DELAY_CE=32;
static const int INPUT_PIN_DELAY_MUX=33;
static const int INPUT_PIN_DELAY_WC=34;
static const int INPUT_PIN_START=35;
static const int INPUT_PIN_PHASE_START=36;
static const int INPUT_PIN_STOP=37;
static const int INPUT_PIN_MEMREQ=38;
static const int INPUT_PIN_DONE=39;
static const int INPUT_PIN_RAMW=40;
static const int INPUT_PIN_RAME=41;
static const int INPUT_PIN_RAMG=42;
static const int INPUT_PIN_SET=43;
static const int INPUT_PIN_PREPHASE=44;
static const int INPUT_PIN_SEQRESET=45;
static const int INPUT_PIN_LUTINDEX=46;
static const int OUTPUT_PIN=100;
static const int OUTPUT_PIN_NOEXT=101;
static const int OUTPUT_PIN_AND=102;
static const int OUTPUT_PIN_OR=103;
static const int OUTPUT_PIN_DONE=104;
static const int OUTPUT_PIN_EXT=105;
static const int OUTPUT_PIN_CLK=106;
static const int OUTPUT_PIN_RESET=107;
static const int OUTPUT_PIN_VCC=108;
static const int OUTPUT_PIN_GND=109;
static const int OUTPUT_PIN_AL=110;
static const int OUTPUT_PIN_AH=111;
static const int OUTPUT_PIN_CLR=112;
static const int OUTPUT_PIN_CE=113;
static const int OUTPUT_PIN_MEMREQ=114;
static const int OUTPUT_PIN_SRC_MUX=115;
static const int OUTPUT_PIN_SRC_CE=116;
static const int OUTPUT_PIN_SRC_WC=117;
static const int OUTPUT_PIN_MEMORYMUX=118;
static const int OUTPUT_PIN_ADDRMUX=119;
static const int OUTPUT_PIN_ADDRCE=120;
static const int OUTPUT_PIN_ADDRCLR=121;
static const int OUTPUT_PIN_WC=122;
static const int OUTPUT_PIN_RAMG=123;
static const int OUTPUT_PIN_RAMW=124;
static const int OUTPUT_PIN_RAME=125;
static const int OUTPUT_PIN_MEMORY=126;
static const int OUTPUT_PIN_DMUX=127;
static const int OUTPUT_PIN_MUX_SWITCHABLE=128;
static const int OUTPUT_PIN_MUX_RESULT=129;
static const int OUTPUT_PIN_PHASE=130;
static const int OUTPUT_PIN_INTERCONNECT=131;
static const int OUTPUT_PIN_INTERCONNECT_ENABLE=132;
static const int OUTPUT_PIN_SNK_WC=133;
static const int OUTPUT_PIN_SNK_CE=134;
static const int OUTPUT_PIN_SNK_MUX=135;
static const int OUTPUT_PIN_DELAY_WC=136;
static const int OUTPUT_PIN_DELAY_CE=137;
static const int OUTPUT_PIN_DELAY_MUX=138;
static const int OUTPUT_PIN_START=139;
static const int OUTPUT_PIN_PHASE_START=140;
static const int OUTPUT_PIN_STOP=141;
static const int OUTPUT_PIN_MEMOK=142;
static const int OUTPUT_PIN_EXTCTRL=143;
static const int OUTPUT_PIN_PREPHASE=144;
static const int OUTPUT_PIN_SEQRESET=145;
static const int OUTPUT_PIN_CTRL=146;
static const int OUTPUT_PIN_LUTINDEX=147;
static const int BIDIR_PIN=200;
static const int BIDIR_PIN_MEMORY=201;
static const int BIDIR_PIN_MUX_RESULT=202;
static const int BIDIR_PIN_MUX_SWITCHABLE=203;
static const int BIDIR_PIN_INTERCONNECT=204;
static const int BIDIR_PIN_INTERCONNECT_ENABLE=205;
static const int INTERNAL_PIN=999;

// IMPORTANT:Adjust to reflect span of pin groups defined above
static const int IPIN_MIN=1;
static const int IPIN_MAX=46;
static const int OPIN_MIN=100;
static const int OPIN_MAX=147;
static const int BIDIR_MIN=200;
static const int BIDIR_MAX=205;

// Assertion constants
static const char AL=0;
static const char AH=1;


class Pin {
 private:
  int pin_count;
  ACSIntArray* major_bit;
  ACSIntArray* vector_length;  // Really means bitlength *groan*
  ACSIntArray* word_count;
  ACSIntArray* min_vlength;
  ACSIntArray* max_vlength;
  ACSIntArray* word_lock;      // Used by Word Length analysis tool
  ACSIntArray* prec_lock;      // Used by FPGA-target for mechanical growth
  ACSIntArray* netlist_ids;    // Netlist identifiers for algorithmic connections

  ACSIntArray* skip_pin;       // For multiple implementations, block out pins that arent utilized
  ACSIntArray* data_type;
  ACSIntArray* pin_type;
  ACSIntArray* pin_assertion;
  ACSIntArray* pin_assigned;
  ACSIntArray* pin_limit;
  ACSIntArray* pin_priority;
  ACSIntArray* phase_dependency;
  ACSIntArray** pin_directions;  // Breaks a pin down into pin_types for each bit

  StringArray* pin_name;
  StringArray* alt_pinname;
  ACSIntArray* altpin_flag;
  StringArray* corona_pinname;
  Connectivity** connect;
  Delay** delays;

  ACSIntArray* inode_pins;
  ACSIntArray* onode_pins;
  ACSIntArray* icnode_pins;
  ACSIntArray* ocnode_pins;

  public:
  Pin::Pin();
  Pin::~Pin();
  Pin& operator=(Pin&);
  void Pin::update_pins();
  void Pin::copy_pin(Pin*,const int);
  void Pin::copy_pins(Pin*);
  void Pin::copy_pins(Pin*,ACSIntArray*);
  ACSIntArray* Pin::classify_datapins(const int);
  ACSIntArray* Pin::classify_controlpins(const int);
  int Pin::add_pin(const char*,const int);
  int Pin::add_pin(const char*,const int,const char);
  int Pin::add_pin(const char*,const char*,const int);
  int Pin::add_pin(const char*,const int,const int,const int);
  int Pin::add_pin(const char*,const int,const int,const int,const char);
  int Pin::add_pin(const char*,const int,const int,const int,const int);
  int Pin::add_pin(const char*,const int,const int,const int,const int,const char);
  int Pin::add_pin(const char*,const char*,const int,const int,const int,const int);
  int Pin::new_pin(void);
  int Pin::change_pinname(const int, const char*);
  int Pin::set_pinpriority(const int,
			   const int);
  int Pin::set_phasedependency(const int,
			       const int);
  int Pin::set_pinlim(const int,
		      const int);
  int Pin::set_pintype(const int,
		       const int);
  int Pin::set_pinassertion(const int,
		       const int);
  int Pin::set_directionality(const int,
			      const int,
			      const int);
  int Pin::set_wordcount(const int,
                         const int);
  int Pin::set_skip(const int,
                    const int);
  int Pin::set_datatype(const int,
			const int);
  int Pin::set_min_vlength(const int,
			   const int);
  int Pin::set_max_vlength(const int,
			   const int);
  int Pin::set_precision(const int,
			 const int,
			 const int,
			 const int);
  int Pin::set_preclock(const int,
                        const int);
  int Pin::match_precision(const int,
                           const int);
  int Pin::match_precision(Pin*,const int,const int);
  int Pin::setmax_precision(const int,
			    const int,
			    const int,
			    const int);
  int Pin::set_wordlock(const int,
			const int);
  int Pin::set_alllocks(void);
  int Pin::set_netlistid(const int,
			 const int);

  // Accessors to protected members:
  int Pin::query_pincount(void);
  int Pin::query_majorbit(const int);
  int Pin::query_bitlen(const int);
  int Pin::query_wordcount(const int);
  int Pin::query_minbitlen(const int);
  int Pin::query_maxbitlen(const int);
  int Pin::query_wordlock(const int);
  int Pin::query_preclock(const int);
  int Pin::query_netlistid(const int);
  int Pin::query_skip(const int);
  int Pin::query_datatype(const int);
  int Pin::query_pintype(const int);
  int Pin::query_pinassertion(const int);
  int Pin::query_pinassigned(const int);
  int Pin::query_pinlimit(const int);
  int Pin::query_pinpriority(const int);
  int Pin::query_phasedependency(const int);
  char* Pin::query_pinname(const int);
  char* Pin::query_altpinname(const int);
  int Pin::query_altpinflag(const int);
  char* Pin::query_cpinname(const int);
  Connectivity* Pin::query_connection(const int);
  Delay* Pin::query_delay(const int);
  ACSIntArray* Pin::query_inodes(void);
  ACSIntArray* Pin::query_onodes(void);
  ACSIntArray* Pin::query_icnodes(void);
  ACSIntArray* Pin::query_ocnodes(void);


  int Pin::pin_classtype(const int);
  int Pin::req_pintype(const int);
  int Pin::retrieve_type(const int);
  int Pin::retrieve_type(const int,
			 const int);
  void Pin::print_pins(const char*);
  void Pin::print_pinclasses(void);
  int Pin::free_pintype(const int);
  int Pin::free_pintype(const int,const int);
  int Pin::connect_pin(const int,
		       const int,
		       const int,
		       const int);
  int Pin::connect_pin(const int,
		       const int,
		       const int,
		       const int,
		       const int);
  int Pin::assign_pin(const int,
		      const int,
		      const int,
		      const int);
  int Pin::match_acstype(const int,
			 const int);
  int Pin::match_acsnode(const int,
			 const int);
  int Pin::disconnect_pin(const int,
			  const int);
  int Pin::disconnect_pin_to(const int, const int);
  int Pin::disconnect_pin_from(const int, const int);
  int Pin::disconnect_all_pinsto(const int);
  int Pin::disconnect_all_ipins(void);
  int Pin::disconnect_all_opins(void);
  int Pin::disconnect_allpin_nodes(const int);
  int Pin::disconnect_allpins(void);
  int Pin::bw_exclude(void);
  int Pin::pin_withname(const char*);
  int Pin::pin_withnetlist(const int);
  int Pin::pin_withnetlist(const int,
                           const int);
  int Pin::pins_withnetlist(const int);
  int Pin::reclassify_pin(const int,
			  const int);
  int Pin::max(const int,const int);
  int Pin::min(const int,const int);
};



#endif
