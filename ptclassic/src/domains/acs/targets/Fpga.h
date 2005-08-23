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
 Version: @(#)Fpga.h	1.6 08/02/01
***********************************************************************/
#ifndef FPGA_H
#define FPGA_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACSIntArray.h"
#include "CoreList.h"
#include "Pin.h"
#include "Sequencer.h"
#include "Port_Timing.h"
#include "Sg_Constructs.h"
#include "acs_vhdl_lang.h"

static const int FPGA_RESERVED=2;
static const int FPGA_USED=1;
static const int FPGA_UNUSED=0;

static const int MEMORY_AVAILABLE=1;
static const int MEMORY_NOTAVAILABLE=0;

static const int FPGA_AVAILABLE=1;
static const int FPGA_NOTAVAILABLE=0;

// Prevent circular declarations
class Sequencer;
class CoreList;

class Fpga
{
 public:
  static const int DEBUG_SEQUENCER=0;
  static const int DEBUG_TIMING=0;

  static const int MAX_PARTNAME=80;

  // Info
  int fpga_id;  // It's acs_device designation

  // Design info
  char* root_filename;
  char* io_filename;

  // Fpga target-usage
  int usage_state;

  // Fpga members
  char* part_make;
  char* part_tech;
  char* part_name;
  char* part_pack;
  int speed_grade;

  // Fpga pins
  Pin* ext_signals;
  Pin* data_signals;
  Pin* ctrl_signals;
  Pin* constant_signals;
  StringArray* arch_bindlist;

  // Assigned components
  CoreList* child_cores;

  // Memory interface members
  Sequencer* sequencer;
  int completion_time;
  int mem_count;
  ACSIntArray* mem_connected;
  ACSIntArray* datain_signal_id;
  ACSIntArray* dataout_signal_id;
  ACSIntArray* addr_signal_id;

  int separate_rw;
  ACSIntArray* read_signal_id;
  ACSIntArray* write_signal_id;
  ACSIntArray* enable_signal_id;

  // Fpga interface members (non-shared)
  int fpga_count;
  ACSIntArray* fpga_connected;
  ACSIntArray* ifpga_signal_id;
  ACSIntArray* ifpga_enable_id;
  Port_Timing* ifpga_timing;

  // Fpga-specific information for the sequencer
  ACSIntArray* tmp_seqpins;

  // Build information
  int part_type;
  StringArray* synthesis_libname;  // i.e., files not used in simulation
  StringArray* synthesis_files;
  StringArray* synthesis_path;
  StringArray* preamble_files;
  StringArray* preamble_path;
  ACSIntArray* preamble_origin; // USER=1, DEFAULT_PATH=0
  StringArray* postamble_files;
  StringArray* postamble_path;
  ACSIntArray* postamble_origin; // USER=1, DEFAULT_PATH=0

  
 public:
 Fpga::Fpga(void);
 Fpga::~Fpga(void);
 void Fpga::set_acsdevice(const int);
 int Fpga::retrieve_acsdevice(void);
 void Fpga::set_rootfilename(char*);
 void Fpga::set_iofilename(char*);
 char* Fpga::retrieve_rootfilename(void);
 void Fpga::set_part(char*,
		     char*,
		     char*,
		     char*,
		     int);
 void Fpga::set_control_pins(const char*,
			    const char*,
			    const int,
			    const int,
                            const int);
 void Fpga::set_synthesis_file(const char*, const char*, const char*);
 void Fpga::set_preamble_file(const char*, const char*, const int);
 void Fpga::set_postamble_file(const char*, const char*, const int);
 void Fpga::add_memory(const int);
 void Fpga::set_memory_pins(const int,
			    char*, char*, const int,
			    char*, const int, const int, const int,
			    const int, const int,
			    char*, char*, char*, 
			    const int, const int);
  void Fpga::add_interconnect(const int);
  void Fpga::set_interconnect_pins(const int,
				   const char*, const char*, 
				   const int, const int, 
				   const char*, const int, const int, 
				   const int);
  void Fpga::set_child(ACSCGFPGACore*);
  CoreList* Fpga::get_childcores_handle(void);
  int Fpga::test_route(const int);
  int Fpga::add_timing(const int,const int,const int);
  int Fpga::query_mode(const int);
  void Fpga::mode_dump(void);
  int Fpga::query_memconnect(const int);

  // Sequencing
  void Fpga::set_completiontime(const int);
  void Fpga::generate_sequencer(void);

  void Fpga::print_children(void);
  void Fpga::print_fpga(void);
};

#endif
