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
 Version: @(#)Fpga.cc      1.0     06/16/99
***********************************************************************/
#include "Fpga.h"

Fpga::Fpga() 
: root_filename(NULL),
  usage_state(FPGA_UNUSED),
  part_make(NULL), part_tech(NULL), part_name(NULL), part_pack(NULL), 
  speed_grade(0),
  child_cores(NULL),
  sequencer(NULL), mem_count(0), mem_connected(NULL),
  datain_signal_id(NULL), dataout_signal_id(NULL),
  addr_signal_id(NULL), 
  separate_rw(0), read_signal_id(NULL), write_signal_id(NULL), enable_signal_id(NULL),
  fpga_count(0), fpga_connected(NULL), 
  ifpga_signal_id(NULL), ifpga_enable_id(NULL),  ifpga_timing(NULL)
{
  ext_signals=new Pin;
  data_signals=new Pin;
  ctrl_signals=new Pin;
  constant_signals=new Pin;
  arch_bindlist=new StringArray;
}

Fpga::~Fpga()
{
  delete []root_filename;

  usage_state=FPGA_UNUSED;

  delete []part_make;
  delete []part_tech;
  delete []part_name;
  delete []part_pack;

  delete ext_signals;
  delete data_signals;
  delete ctrl_signals;
  delete constant_signals;
  delete arch_bindlist;

  delete child_cores;

  delete sequencer;
  mem_count=0;
  delete mem_connected;
  delete datain_signal_id;
  delete dataout_signal_id;
  delete addr_signal_id;
  separate_rw=0;
  delete read_signal_id;
  delete write_signal_id;
  delete enable_signal_id;

  fpga_count=0;
  delete fpga_connected;
  delete ifpga_signal_id;
  delete ifpga_enable_id;
  delete []ifpga_timing;
}

void Fpga::set_rootfilename(char* supplied_name)
{
  if (root_filename!=NULL)
    strcpy(root_filename,supplied_name);
  else
    {
      root_filename=new char[MAX_PARTNAME];
      strcpy(root_filename,supplied_name);
    }
}

char* Fpga::retrieve_rootfilename(void)
{
  if (root_filename!=NULL)
    return(root_filename);
  else
    return("No_Filename_Specified");
}

void Fpga::set_part(char* part_mk,
		    char* part_th,
		    char* part_type,
		    char* part_pk,
		    int mx_speed)
{
  if (part_make!=NULL)
    strcpy(part_make,part_mk);
  else
    {
      part_make=new char[MAX_PARTNAME];
      strcpy(part_make,part_mk);
    }

  if (part_tech!=NULL)
    strcpy(part_tech,part_th);
  else
    {
      part_tech=new char[MAX_PARTNAME];
      strcpy(part_tech,part_th);
    }

  if (part_name!=NULL)
    strcpy(part_name,part_type);
  else
    {
      part_name=new char[MAX_PARTNAME];
      strcpy(part_name,part_type);
    }

  if (part_pack!=NULL)
    strcpy(part_pack,part_pk);
  else
    {
      part_pack=new char[MAX_PARTNAME];
      strcpy(part_pack,part_pk);
    }

  speed_grade=mx_speed;
}

// FIX: This should be generalized, but for now it is fixed for the
// Annapolis
void Fpga::set_control_pins(void)
{
  // This is generalized for constant signals
  constant_signals->add_pin("VCC",1.0,STD_LOGIC,OUTPUT_PIN_VCC);
  constant_signals->add_pin("GND",1.0,STD_LOGIC,OUTPUT_PIN_GND);

  ext_signals->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  ext_signals->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET);
  //  ext_signals->add_pin("XbarData_InReg",35,36,STD_LOGIC,
  //			    INPUT_PIN_EXTCTRL);
  ext_signals->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL);
  ext_signals->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK);
  ext_signals->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  ext_signals->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);

  // Ordered list of architecture signals that the ext_signals will
  // be bound to
  arch_bindlist->add("PE_Pclk");
  arch_bindlist->add("PE_RESET");
  arch_bindlist->add("PE_CPE_Bus_In(0)");
  arch_bindlist->add("PE_MemBusGrant_n");
  arch_bindlist->add("PE_MemBusReq_n");
  arch_bindlist->add("PE_CPE_Bus_Out(1)");
  arch_bindlist->add("PE_MemData_InReg");
  arch_bindlist->add("PE_MemData_OutReg");
  arch_bindlist->add("PE_MemAddr_OutReg");
  arch_bindlist->add("PE_MemWriteSel_n");
  arch_bindlist->add("PE_MemStrobe_n");
}

void Fpga::add_memory(const int memory_count)
{
  mem_count=memory_count;
  mem_connected=new ACSIntArray(mem_count,MEMORY_NOTAVAILABLE);
  datain_signal_id=new ACSIntArray(mem_count,UNASSIGNED);
  dataout_signal_id=new ACSIntArray(mem_count,UNASSIGNED);
  addr_signal_id=new ACSIntArray(mem_count,UNASSIGNED);
  write_signal_id=new ACSIntArray(mem_count,UNASSIGNED);
  enable_signal_id=new ACSIntArray(mem_count,UNASSIGNED);
}

void Fpga::set_memory_pins(const int memory_no,
			   char* memin_name,
			   char* memout_name,
			   const int data_bits,
			   char* addr_name,
			   const int addr_bits,
			   const int low_address,
			   const int high_address,
			   const int bidirectional_bus,
			   const int rw_control_separate,
			   char* memread_name,
			   char* memwrite_name,
			   char* memenable_name,
			   const int read_skew,
			   const int write_skew)
{
  // Flag this memory as available to this fpga
  mem_connected->set(memory_no,MEMORY_AVAILABLE);

  // Setup numerical pointer to the Arch mem_port index
  datain_signal_id->set(memory_no,ext_signals->add_pin(memin_name,
						       data_bits-1,
						       data_bits,
						       STD_LOGIC,INPUT_PIN_MEMORY));
  dataout_signal_id->set(memory_no,ext_signals->add_pin(memout_name,
							data_bits-1,
							data_bits,
							STD_LOGIC,OUTPUT_PIN_MEMORY));

  // Define memory address signals
  addr_signal_id->set(memory_no,ext_signals->add_pin(addr_name,
						     addr_bits-1,
						     addr_bits,
						     STD_LOGIC,OUTPUT_PIN));
  
  // Define memory control signals
  separate_rw=0;
  write_signal_id->set(memory_no,ext_signals->add_pin(memwrite_name,
						      0,1,
						      STD_LOGIC,OUTPUT_PIN_RAMW));
  
  enable_signal_id->set(memory_no,ext_signals->add_pin(memenable_name,
						       0,1,
						       STD_LOGIC,OUTPUT_PIN_RAME));
  
  // Priority will default to the ranking of the memory number
  // If the read signal is shared with write signal (H/L) then it is
  // referenced via the write_signal_id structure and read_signal_id is null
  ext_signals->set_pinpriority(write_signal_id->query(memory_no),memory_no);

  if (separate_rw)
    ext_signals->set_pinpriority(read_signal_id->query(memory_no),memory_no);
}

void Fpga::add_interconnect(const int total_fpgas)
{
  fpga_count=total_fpgas;
  fpga_connected=new ACSIntArray(fpga_count,FPGA_NOTAVAILABLE);
  ifpga_signal_id=new ACSIntArray(fpga_count,UNASSIGNED);
  ifpga_enable_id=new ACSIntArray(fpga_count,UNASSIGNED);
  ifpga_timing=new Port_Timing[total_fpgas];
}

/////////////////////////////////////////////////////////////
// Describe the inter-FPGA connections that are bidirectional
/////////////////////////////////////////////////////////////
void Fpga::set_interconnect_pins(const int fpga_no,
				 const char* outgoing_name,
				 const char* incoming_name,
				 const int data_bits,
				 const int directionality,
				 const char* enable_name,
				 const int enable_bits,
				 const int enable_direction,
				 const int enable_priority)
{
  // Flag this fpga as a neighbor to this fpga
  fpga_connected->set(fpga_no,FPGA_AVAILABLE);

  // Setup numerical pointer to the fpga interconnect data path
  ifpga_signal_id->set(fpga_no,ext_signals->add_pin(outgoing_name,
						    incoming_name,
						    data_bits-1,
						    data_bits,
						    STD_LOGIC,
						    directionality));

  // Setup numerical pointer to the fpga interconnect enable pin
  int enable_index=ext_signals->add_pin(enable_name,
					enable_bits-1,
					enable_bits,
					STD_LOGIC,
					enable_direction);
  ifpga_enable_id->set(fpga_no,enable_index);
  ext_signals->set_pinpriority(enable_index,enable_priority);
}

void Fpga::set_child(ACSCGFPGACore* child_core)
{
  if (child_cores==NULL)
    child_cores=new SequentialList;

  // Duplicates are not needed
  int duplicate=0;
  for (int loop=1;loop<=child_cores->size();loop++)
    {
      ACSCGFPGACore* current_core=(ACSCGFPGACore*) child_cores->elem(loop);
      if (child_core->acs_id==current_core->acs_id)
	{
	  printf("Dup sg %s found not adding\n",child_core->comment_name()); 
	  duplicate=1;
	  break;
	}
    }

  if (!duplicate)
    {
//      printf("Adding core %s\n",child_core->comment_name());
      child_cores->append((Pointer) child_core);
    }
}

SequentialList* Fpga::get_childcores_handle(void)
{
  if (child_cores==NULL)
    fprintf(stderr,"Fpga::get_childcores_handle:I hope you know what your doing, because your getting a NULL ptr\n");
  return(child_cores);
}

/////////////////////////////////////////////////////////////////
// Query if this fpga is directly connected to a destination fpga
/////////////////////////////////////////////////////////////////
int Fpga::test_route(const int dest_fpga)
{
  if (fpga_connected->query(dest_fpga)==FPGA_AVAILABLE)
    return(1);
  else
    return(0);
}

//////////////////////////////////////////////////////////////////////////
// Establish the timing information for the inter-fpga communication paths
//////////////////////////////////////////////////////////////////////////
int Fpga::add_timing(const int fpga_no, 
		     const int read_time, 
		     const int write_time)
{
  if (read_time != 0)
    ifpga_timing[fpga_no].add_read(0,0,0,read_time,0,0,0);
  else
    ifpga_timing[fpga_no].add_write(0,0,0,0,write_time,0,0);

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////////////
// Query the modality of the port for this simulation
// FIX: Only usefull in a static situation for the port!
////////////////////////////////////////////////////////
int Fpga::query_mode(const int fpga_no)
{
  if (ifpga_timing[fpga_no].count==0)
    return(UNASSIGNED);

  // FIX:Should only be one entry for now
  return(ifpga_timing[fpga_no].get_memtype(0));
}

void Fpga::print_children(void)
{
  if (child_cores==NULL)
    printf("None\n");
  else
    for (int loop=1;loop<=child_cores->size();loop++)
      {
	ACSCGFPGACore* current_core=(ACSCGFPGACore*) child_cores->elem(loop);
	printf("child:%s\n",current_core->comment_name()); 
      }
}
void Fpga::mode_dump(void)
{
  for (int loop=0;loop<fpga_count;loop++)
    {
      printf("Dumping connection to fpga%d\n",loop);
      ifpga_timing[loop].dump();
    }
}



void Fpga::print_fpga(void)
{
  printf("Speed Grade:-%d\n",speed_grade);
  printf("Part Make:%s\tPart Tech:%s\n",part_make,part_tech);
  printf("Part Type:%s\tPart Package:%s\n",part_name,part_pack);
  printf("%d associated memories:",mem_count);
  for (int loop=0;loop<mem_count;loop++)
    printf("%d ",mem_connected->query(loop));
  printf("\n");
  ext_signals->print_pins("External pins:");
}

