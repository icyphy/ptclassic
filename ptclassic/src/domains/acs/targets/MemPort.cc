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
 Version: @(#)MemPort.cc      1.0     06/16/99
***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MemPort.h"

MemPort::MemPort() 
: total_sgs(0), controller_fpga(-1), source_cores(NULL), sink_cores(NULL),
  dataimux_star(NULL), dataomux_star(NULL), data_size(0), 
  addrmux_star(NULL), addrgen_star(NULL),  addrbuf_star(NULL),  
  addr_size(0), addr_lo(0), addr_hi(0),
  portuse(MEMORY_UNUSED),
  bidir_data(0), separate_rw(0), read_skew(0), write_skew(0),
  current_act(0), pt_count(0), mem_timing(NULL), 
  total_latency(0)
{
  const_stars=new CoreList;
}


MemPort::~MemPort() 
{
  total_sgs=0;
  controller_fpga=-1;
  delete source_cores;
  delete sink_cores;
  delete dataimux_star;
  delete dataomux_star;
  delete addrmux_star;
  delete addrbuf_star;
  delete const_stars;
  delete addrgen_star;
  delete mem_timing;
  total_latency=0;

  delete port_timing;
}

int MemPort::init_pt(int seqlen)
{
  pt_count=seqlen;
  if (mem_timing!=NULL)
    {
      printf("MemPort::init_pt:Error, multiple initialization\n");
      return(-1);
    }
  else
    mem_timing=new ACSIntArray(seqlen,UNKNOWN);

  // Return happy condition
  return(1);
}


int MemPort::add_pt(int node_act,int node_type)
{
  mem_timing->set(node_act,node_type);

  // Return happy condition
  return(1);
}


int MemPort::fetch_pt(int index)
{
  return(mem_timing->query(index));
}  


/////////////////////////////////////////
// Start the core assignment process over
/////////////////////////////////////////
int MemPort::reset_cores()
{
  total_sgs=0;
  delete source_cores;
  delete sink_cores;

  if (portuse!=MEMORY_RESERVED)
    portuse=MEMORY_UNUSED;

  source_cores=new CoreList;
  sink_cores=new CoreList;
  current_act=0;

  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////////////////////////////
// Each memory port is tasked to keep track of all source and sink stars that 
// are associated with it.  Additionally, the memory port will conduct 
// preliminary scheduling of all source stars.  
//////////////////////////////////////////////////////////////////////////////
int MemPort::assign_srccore(ACSCGFPGACore* fpga_core)
{
  total_sgs++;
  if (portuse==MEMORY_UNUSED)
    {
      if (DEBUG_MEMPORT)
	printf("Port has not been used yet\n");
      
      // Port has not been used yet
      portuse=MEMORY_USED;
    }
  else
    {
      if (DEBUG_MEMPORT)
	printf("Port has been used, using next available clock period\n");
    }

  // Track sources
  source_cores->append(fpga_core);

  // Assign activation time
  fpga_core->act_input=current_act;
  fpga_core->act_output=current_act+read_skew;
      
  // Update next available time slot
  current_act++;
      
  // Return happy condition
  return(1);
}
int MemPort::assign_snkcore(ACSCGFPGACore* fpga_core,int act)
{
  total_sgs++;
  if (portuse==MEMORY_UNUSED)
    {
      if (DEBUG_MEMPORT)
	printf("Port has not been used yet\n");
      
      // Port has not been used yet
      portuse=MEMORY_USED;
    }
  else
    {
      if (DEBUG_MEMPORT)
	printf("Port has been used, using next available clock period\n");
    }

  sink_cores->append(fpga_core);
  
  // Assign activation time
  fpga_core->act_input=act-write_skew;
  fpga_core->act_output=act;
  
  if (fpga_core->act_output > total_latency)
    total_latency=fpga_core->act_output;
      
  // Return happy condition
  return(1);
}

// Which fpga should be referenced for memory control
int MemPort::set_controller(const int fpga_no)
{
  controller_fpga=fpga_no;

  // Return happy condition
  return(1);
}

int MemPort::set_memory(const int data_bits,
			const int addr_bits,
			const int low_address,
			const int high_address,
			const int bidirectional_bus,
			const int rw_control_separate,
			const int rskew,
			const int wskew)
{
  data_size=data_bits;
  addr_size=addr_bits;
  addr_lo=low_address;
  addr_hi=high_address;
  bidir_data=bidirectional_bus;
  separate_rw=rw_control_separate;
  read_skew=rskew;
  write_skew=wskew;

  // Return happy condition
  return(1);
}
