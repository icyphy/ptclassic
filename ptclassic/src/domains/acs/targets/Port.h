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
 Version: @(#)Port.h      1.0     4/13/00
***********************************************************************/
#ifndef PORT_H
#define PORT_H

#include <stdio.h>
#include "ACSCGFPGACore.h"
#include "Sg_Constructs.h"
#include "CoreList.h"
#include "ACSIntArray.h"
#include "Fpga.h"
#include "Port_Timing.h"
#include "Address.h"

class Port 
{
 public:
  // Switches
  static const int DEBUG_PORT=0;
  static const int DEBUG_BITPACK=0;
  static const int DEBUG_ADDRESSGEN=0;
  static const int DEBUG_IOCONTROLLER=0;

  // Port types
  static const int UNKNOWN_PORT=-1;
  static const int MEMORY=1;
  static const int SYSTOLIC_INTERCONNECT=2;
  static const int RECONFIGURABLE_INTERCONNECT=3;

  // Port usage
  static const int PORT_UNUSED=0;
  static const int PORT_USED=1;
  static const int PORT_RESERVED=2;

  // Class attributes
  int port_type;
  int total_sgs;
  int total_assignments;
  CoreList* io_cores;
  CoreList* source_cores;
  int sources_packed;
  CoreList* sink_cores;
  int sinks_packed;
  CoreList* lut_cores;
  ACSCGFPGACore* dataimux_star;
  ACSCGFPGACore* dataomux_star;
  int data_size;

  // Pointers to devices connected to this port
  int fpga_count;
  Fpga** connected_fpgas;

  // Memory-dependent
  Address* port_addressing;

  // Misc
  int portuse;
  int bidir_data;      // True if memory port is bidirectional
  int separate_rw;     // True of read/write control signal for memories are separate
  int read_skew;       // Number of clocks that a read request is delayed by
  int write_skew;      // Number of clocks that a write request is delayed by

  // Timing info
  Port_Timing* port_timing;
  int pt_count;                     
  ACSIntArray* port_scheduling;
  int total_latency;
  int total_launchrate;

public:
 Port::Port(void);
 Port::Port(const int);
 Port::~Port(void);
  int Port::init_pt(int);
  int Port::add_pt(int,int);
  int Port::fetch_pt(int);
  int Port::reset_cores(void);
  int Port::activate_port(void);
  int Port::port_active(void);
  void Port::reserve_port(void);
  int Port::query_addressing(void);

  int Port::assign_iocore(ACSCGFPGACore*);
  int Port::assign_srccore(ACSCGFPGACore*);
  int Port::assign_snkcore(ACSCGFPGACore*);
  int Port::assign_lutcore(ACSCGFPGACore*);
  int Port::schedule_src(const int, const int);
  int Port::schedule_snk(const int, const int);
  int Port::schedule_snk(ACSCGFPGACore*, const int);
  int Port::assess_port(void);
  int Port::assess_bitwidths(const int);
  int Port::pack_port(const int);
  int Port::set_launchrate(const int);

  int Port::set_port(const int,
		     const int,
		     const int,
		     const int,
		     const int);
  int Port::set_port(const int,
		     const int,
		     const int,
		     const int,
		     const int,
		     const int,
		     const int,
		     const int);
  int Port::get_highaddress(void);

  void Port::add_memorysupport(Sg_Constructs*,
			       Fpga*,
			       const int,
			       CoreList*);
  void Port::add_addressgen(Sg_Constructs*,
			    Fpga*,
			    const int,
			    CoreList*);
  void Port::genhw_address(Sg_Constructs*,
			   const int,
			   const int,
			   Pin*,
			   const int,
			   Port_Timing*,
			   CoreList*);
  ACSCGFPGACore* Port::get_addrmux_star(void);

  void Port::add_io_controller(Sg_Constructs*,
			       Fpga*,
			       const int,
			       CoreList*);

  int Port::memory_test(void);
  void Port::designate_memory(void);
  void Port::designate_sysinterconnect(void);
  void Port::designate_reconfigconnect(void);

  void Port::add_connected(const int);
  void Port::set_connected(const int, Fpga&);
  int Port::query_fpga_connections(void);
  Fpga* Port::get_fpga_handle(const int);
};

#endif
