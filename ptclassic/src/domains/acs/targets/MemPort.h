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
 Version: @(#)MemPort.h      1.0     06/16/99
***********************************************************************/
#ifndef MEMPORT_H
#define MEMPORT_H

#include <stdio.h>
#include "ACSCGFPGACore.h"
#include "StringList.h"
#include "ACSIntArray.h"
#include "Port_Timing.h"

// Class constants
static const int MEMORY_RESERVED=2;
static const int MEMORY_USED=1;
static const int MEMORY_UNUSED=0;


class MemPort 
{
 public:
  // Switches
  static const int DEBUG_MEMPORT=0;

  // Class attributes
  int total_sgs;
  int controller_fpga;
  CoreList* source_cores;
  CoreList* sink_cores;
  ACSCGFPGACore* dataimux_star;
  ACSCGFPGACore* dataomux_star;
  int data_size;
  ACSCGFPGACore* addrmux_star;
  CoreList* const_stars;
  ACSCGFPGACore* addrgen_star;
  ACSCGFPGACore* addrbuf_star;
  int addr_size;
  int addr_lo;
  int addr_hi;
  Port_Timing* port_timing;
  int portuse;
  int bidir_data;      // True if memory port is bidirectional
  int separate_rw;     // True of read/write control signal for memories are separate
  int read_skew;      // Number of clocks that a read request is delayed by
  int write_skew;      // Number of clocks that a write request is delayed by

  // Timing info
  int current_act;
  int pt_count;                     
  ACSIntArray* mem_timing;
  int total_latency;

public:
  MemPort::MemPort(void);
  MemPort::~MemPort(void);
  int MemPort::init_pt(int);
  int MemPort::add_pt(int,int);
  int MemPort::fetch_pt(int);
  int MemPort::reset_cores(void);
  int MemPort::assign_srccore(ACSCGFPGACore*);
  int MemPort::assign_snkcore(ACSCGFPGACore*,int);
  int MemPort::set_controller(const int);
  int MemPort::set_memory(const int,
			  const int,
			  const int,
			  const int,
			  const int,
			  const int,
			  const int,
			  const int);
};

#endif
