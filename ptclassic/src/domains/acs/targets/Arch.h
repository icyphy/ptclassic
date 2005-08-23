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
 Version: @(#)Arch.h	1.5 08/02/01
***********************************************************************/
#ifndef ARCH_H
#define ARCH_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "CoreList.h"
#include "Fpga.h"
#include "Port.h"
#include "ACSIntArray.h"
#include "StringArray.h"

static const int MEMORY=1;
static const int SYSTOLIC=2;

class Arch
{
  static const int DEBUG_ARCH=0;
  static const int DEBUG_ADDMEM=0;
  static const int DEBUG_ROUTES=0;
  static const int DEBUG_LAUNCHRATE=0;
  static const int DEBUG_PACKIO=0;

 public:
  // IO Dept.
  char* arch_directory;
  ifstream* arch_stream;

  // Available FPGAs
  int fpga_count;     // Total number of available FPGAs
  Fpga* fpgas;
  
  // Temporary storage for a route solution between fpgas
  // FIX: Reduce this to a seperate class will ya!
  int total_solutions;
  int src_fpga;
  int dest_fpga;
  int runaway;
  ACSIntArray** route_solutions;
  ACSIntArray* current_solution;

  // Available Memory elements
  int mem_count;     // Total number of available memories
  Port* mem_ports;
  int mem_slice;

  // Availabe Systolic interconnects
  // FIX: Should a distinction be made at this level between memories and  systolic interconnects?
  int systolic_count;
  ACSIntArray** systolic_connections;
  Port* sys_ports;

  // Scheduling-related
  int launch_rate;

  // Build related
  StringArray* archsynthesis_libname;
  StringArray* archsynthesis_files;
  StringArray* archsynthesis_path;
  StringArray* archsupport_files;
  StringArray* archsupport_path;
  StringArray* misc_files;
  StringArray* misc_path;
  ACSIntArray* misc_origin;
  StringArray* simulation_commands;
    
 public:
 Arch::Arch(void);
 Arch::Arch(const char*,const char*);
 Arch::~Arch(void);

  void Arch::read_header(void);
  void Arch::wipe_solution(void);
  void Arch::set_fpga(void);
  void Arch::set_memory(void);
  void Arch::set_systolic(void);

  int Arch::evaluate_fpga_assignment(int&);
  int Arch::activate_fpga(const int);

  int Arch::evaluate_memory_assignment(int&);
  int Arch::activate_memory(const int);

  Fpga* Arch::get_fpga_handle(const int);
  Port* Arch::get_memory_handle(const int);
  Port* Arch::get_systolic_handle(const int);
  int Arch::determine_systolic_handle(const int,const int);
  int Arch::unassign_all(void);
  int Arch::mem_to_fpga(const int);

  // Routing search
  void Arch::find_routes(const int,const int);
  void Arch::find_route(const int);
  void Arch::add_solution(void);

  // Scheduling
  int Arch::assess_io(void);
  ACSIntArray* Arch::compressed_io(const int);
  int Arch::calc_launchrate(void);
  int Arch::query_addressing(void);

  // Sequencing
  void Arch::generate_sequencer(void);
    
  // Support
  void Arch::print_arch(const char*);
};

#endif
