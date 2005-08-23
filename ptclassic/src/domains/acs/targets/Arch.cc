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
 Version: @(#)Arch.cc      1.0     06/16/99
***********************************************************************/
#include "Arch.h"

inline Arch::Arch(void) 
: fpga_count(0), fpgas(NULL), 
  total_solutions(0), src_fpga(-1), dest_fpga(-1), route_solutions(NULL),
  mem_count(0), mem_ports(NULL) 
{}

Arch::Arch(int fpga_total) 
: fpga_count(fpga_total), 
  total_solutions(0), src_fpga(-1), dest_fpga(-1), route_solutions(NULL), 
  mem_count(0), mem_ports(NULL) 
{
  fpgas=new Fpga[fpga_count];
}

Arch::Arch(int fpga_total, int mem_total) 
: fpga_count(fpga_total), 
  total_solutions(0), src_fpga(-1), dest_fpga(-1), route_solutions(NULL), 
  mem_count(mem_total) 
{
  fpgas=new Fpga[fpga_count];
  mem_ports=new MemPort[mem_count];
}

Arch::~Arch(void)
{
  fpga_count=0;
  delete []fpgas;

  mem_count=0;
  delete []mem_ports;

  wipe_solution();
}

void Arch::wipe_solution(void)
{
  for (int loop=0;loop<total_solutions;loop++)
    delete route_solutions[loop];
  delete route_solutions;
  total_solutions=0;
}

/////////////////////////////////////////////////////////////////////
// Get information for the fpga's currently used in this architecture
// FIX: Should be file-readable
/////////////////////////////////////////////////////////////////////
void Arch::set_fpga()
{
  // FIX: Annapolis configuration
  // Identify each part, and setup external control pin information
  for (int fpga_no=0;fpga_no<fpga_count;fpga_no++)
    {
      fpgas[fpga_no].set_part("Xilinx","XC4000XL","XC4062XL","HQ240",1);
      fpgas[fpga_no].set_control_pins();
      fpgas[fpga_no].add_interconnect(fpga_count);
    }

  // FIX: Annapolis-specific, ignoring Fifos, ignoring bidirectionality
  // Set systolic connections between fpgas
  fpgas[1].set_interconnect_pins(2,"Right_Out","Right_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Right_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,1);
  fpgas[2].set_interconnect_pins(1,"Left_Out","Left_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Left_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,0);
  fpgas[2].set_interconnect_pins(3,"Right_Out","Right_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Right_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,1);

  fpgas[3].set_interconnect_pins(2,"Left_Out","Left_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Left_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,0);
  fpgas[3].set_interconnect_pins(4,"Right_Out","Right_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Right_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,1);

  fpgas[4].set_interconnect_pins(3,"Left_Out","Left_InReg",36,
				 BIDIR_PIN_INTERCONNECT,
				 "Left_OE",36,
				 OUTPUT_PIN_INTERCONNECT_ENABLE,0);

  // FIX: Annapolis-specific
  // ACS Controller resides in fpga#0, it is therefore reserved
  fpgas[0].usage_state=FPGA_RESERVED;
}

///////////////////////////////////////////////////////////////////////
// Get information for the memories currently used in this architecture
// FIX:For now, the memories stats are fixed.
///////////////////////////////////////////////////////////////////////
void Arch::set_memory()
{
  // Specify all the memories first
  for (int loop=0;loop<mem_count;loop++)
    {
      // FIX:Annapolis wildforce has memory control under local fpga, fpga_num=mem_num
      mem_ports[loop].set_controller(loop);
      mem_ports[loop].set_memory(32,22,0,262144,0,1,2,0);
    }
      
  // FIX: Annapolis-specific
  // ACS Controller resides in fpga#0, it and its memory is therefore reserved
  mem_ports[0].portuse=MEMORY_RESERVED;

  // Specify which memories are connected to which fpgas
  for (int loop=0;loop<fpga_count;loop++)
    {
      if (DEBUG_ADDMEM)
	printf("Adding memory for fpga %d\n",loop);
      // FIX:Annapolis wildforce has one memory per fpga, fpga_Num==mem_Num
      fpgas[loop].add_memory(mem_count);
      fpgas[loop].set_memory_pins(loop,
				  "MemData_InReg","MemData_OutReg",32,
				  "MemAddr_OutReg",22,0,262144,
				  0,1,
				  "","MemWriteSel_n","MemStrobe_n",2,0);
    }

}

////////////////////////////////////
// For a given fpga, return a handle
////////////////////////////////////
Fpga* Arch::get_fpga_handle(const int fpga_no)
{
  if (fpga_no<fpga_count)
    return(&fpgas[fpga_no]);
  else
    {
      fprintf(stderr,"Arch::get_fpga_handle:Error, request for invalid fpga %d\n",fpga_no);
      return(NULL);
    }
}

////////////////////////////////////
// For a given fpga, return a handle
////////////////////////////////////
MemPort* Arch::get_memory_handle(const int memory_no)
{
  if (memory_no<mem_count)
    return(&mem_ports[memory_no]);
  else
    {
      fprintf(stderr,"Arch::get_memory_handle:Error, request for invalid memory %d\n",
	     memory_no);
      return(NULL);
    }
}

////////////////////////////////////////////////
// Set the fpga for usage in this implementation
////////////////////////////////////////////////
int Arch::activate_fpga(const int fpga_no)
{
  if (fpgas[fpga_no].usage_state==FPGA_RESERVED)
    return(0);

  fpgas[fpga_no].usage_state=FPGA_USED;

  // Return happy condition
  return(1);
}


////////////////////////////////////////////////////////////////////////
// Root method for determining all inter-fpgas routes between a starting
// and ending fpga number
////////////////////////////////////////////////////////////////////////
void Arch::find_routes(const int start_fpga, const int end_fpga)
{
  wipe_solution();

  if (DEBUG_ROUTES)
    printf("find_routes looking between fpga %d and fpga %d\n",
	   start_fpga,
	   end_fpga);
  runaway=0;
  total_solutions=0;
  src_fpga=start_fpga;
  dest_fpga=end_fpga;
  current_solution=new ACSIntArray;
  current_solution->add(start_fpga);
  find_route(src_fpga);
}

////////////////////////////////////////////////////////////////
// Return an integer array indicating which fpgas should be used
// for interconnecting between source and destination fpgas
// FIX: Need to test to see if route can be shared/used
////////////////////////////////////////////////////////////////
void Arch::find_route(const int start_fpga)
{
  if (DEBUG_ROUTES)
    printf("finding route based from %d\n",start_fpga);

  //
  // Watch for out of control situations
  //
  if (runaway>100)
    return;
  else
    runaway++;

  //
  // Track options
  //
  ACSIntArray* fpga_attempt=new ACSIntArray(fpga_count,0);

  // Constrain search space, removing current, reserved and previous fpgas
  fpga_attempt->set(src_fpga,1);
  fpga_attempt->set(start_fpga,1);
  for (int fpga_no=0;fpga_no<fpga_count;fpga_no++)
    if (fpgas[fpga_no].usage_state==FPGA_RESERVED)
      {
	fpga_attempt->set(fpga_no,1);
	if (DEBUG_ROUTES)
	  printf("fpga %d is reserved..excluding\n",fpga_no);
      }
  for (int sol_loop=0;sol_loop<current_solution->population();sol_loop++)
    {
      int fpga_sol=current_solution->get(sol_loop);
      fpga_attempt->set(fpga_sol,1);
      if (DEBUG_ROUTES)
	printf("fpga %d is part of current solution..excluding\n",fpga_sol);
    }

  //
  // Do these fpgas connect?
  //
  while (!fpga_attempt->full(1))
    {
      // Find the first untested fpga in the architecture
      int next_fpga=fpga_attempt->find_index(0);
      if (DEBUG_ROUTES)
	printf("will see if %d is connected to %d\n",next_fpga,start_fpga);
      fpga_attempt->set(next_fpga,1);
      if (fpgas[start_fpga].test_route(next_fpga))
	{
	  // If it is our destination, then we have a solution...add it
	  if (next_fpga==dest_fpga)
	    {
	      current_solution->add(dest_fpga);
	      add_solution();
	    }
	  else
	    {
	      // Otherwise, a good start, add it to our solution list and 
	      // search from here
	      current_solution->add(next_fpga);
	      if (DEBUG_ROUTES)
		printf("is connected, will see if %d connects to dest %d\n",
		       next_fpga,dest_fpga);
	      {
		find_route(next_fpga);
	      
		if (DEBUG_ROUTES)
		  fpga_attempt->print("fpga_attempts");
	      }
	      current_solution->pop();
	    }
	}
    }
  delete fpga_attempt;
}

void Arch::add_solution(void)
{
//  current_solution->print("Solution found");
//  printf("Adding solution %d\n",total_solutions);
  ACSIntArray** new_solutions=new ACSIntArray*[total_solutions+1];
  for (int swap_loop=0;swap_loop<total_solutions;swap_loop++)
    {
//      printf("swap_loop=%d\n",swap_loop);
      new_solutions[swap_loop]=route_solutions[swap_loop];
    }
  new_solutions[total_solutions]=new ACSIntArray;
  new_solutions[total_solutions]->copy(current_solution);
  delete []route_solutions;
  route_solutions=new_solutions;
  total_solutions++;
}



void Arch::print_arch(const char* title)
{
  printf("Architecture %s: has %d Fpgas\n",title,fpga_count);
  for (int loop=0;loop<fpga_count;loop++)
    {
      printf("Fpga %d:\n",loop);
      fpgas[loop].print_fpga();
    }
}
