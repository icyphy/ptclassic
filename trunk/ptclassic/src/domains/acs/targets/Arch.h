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
 Version: @(#)Arch.h      1.0     06/16/99
***********************************************************************/
#ifndef ARCH_H
#define ARCH_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "Fpga.h"
#include "MemPort.h"

static const int DEBUG_ADDMEM=0;
static const int DEBUG_ROUTES=0;

class Arch
{
 public:
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
  MemPort* mem_ports;
    
 public:
 Arch::Arch(void);
 Arch::Arch(int);
 Arch::Arch(int,int);
 Arch::~Arch(void);
  
  void Arch::wipe_solution(void);
  void Arch::set_fpga(void);
  void Arch::set_memory(void);
  Fpga* Arch::get_fpga_handle(const int);
  MemPort* Arch::get_memory_handle(const int);
  int Arch::activate_fpga(const int);
  void Arch::find_routes(const int,const int);
  void Arch::find_route(const int);
  void Arch::add_solution(void);
  void Arch::print_arch(const char*);
};

#endif
