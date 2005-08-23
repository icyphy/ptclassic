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
 Version: @(#)Fpga.cc	1.5 08/02/01
***********************************************************************/
#include "Fpga.h"

Fpga::Fpga() 
: fpga_id(-1),
  root_filename(NULL), io_filename(NULL),
  usage_state(FPGA_UNUSED),
  part_make(NULL), part_tech(NULL), 
  part_name(NULL), part_pack(NULL), 
  speed_grade(0),
  child_cores(NULL),
  sequencer(NULL), mem_count(0), mem_connected(NULL),
  datain_signal_id(NULL), dataout_signal_id(NULL),
  addr_signal_id(NULL), 
  separate_rw(0), read_signal_id(NULL), write_signal_id(NULL), enable_signal_id(NULL),
  fpga_count(0), fpga_connected(NULL), 
  ifpga_signal_id(NULL), ifpga_enable_id(NULL),  ifpga_timing(NULL),  part_type(UNKNOWN_DEVICE)
{
  ext_signals=new Pin;
  data_signals=new Pin;
  ctrl_signals=new Pin;
  constant_signals=new Pin;
  arch_bindlist=new StringArray;

  root_filename=new char[MAX_PARTNAME];
  io_filename=new char[MAX_PARTNAME];

  part_make=new char[MAX_PARTNAME];
  part_tech=new char[MAX_PARTNAME];
  part_name=new char[MAX_PARTNAME];
  part_pack=new char[MAX_PARTNAME];

  // This is generalized for constant signals
  constant_signals->add_pin("VCC",1.0,STD_LOGIC,OUTPUT_PIN_VCC);
  constant_signals->add_pin("GND",1.0,STD_LOGIC,OUTPUT_PIN_GND);

  tmp_seqpins=new ACSIntArray;

  synthesis_libname=new StringArray;
  synthesis_path=new StringArray;
  synthesis_files=new StringArray;
  preamble_path=new StringArray;
  preamble_files=new StringArray;
  preamble_origin=new ACSIntArray;
  postamble_path=new StringArray;
  postamble_files=new StringArray;
  postamble_origin=new ACSIntArray;
}

Fpga::~Fpga()
{
  fpga_id=-1;
  delete []root_filename;
  delete []io_filename;

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

  delete tmp_seqpins;

  delete synthesis_libname;
  delete synthesis_path;
  delete synthesis_files;
  delete preamble_path;
  delete preamble_files;
  delete preamble_origin;
  delete postamble_path;
  delete postamble_files;
  delete postamble_origin;
}

void Fpga::set_acsdevice(const int id)
{
  fpga_id=id;
}

int Fpga::retrieve_acsdevice(void)
{
  return(fpga_id);
}

void Fpga::set_rootfilename(char* supplied_name)
{
  strcpy(root_filename,supplied_name);
}

void Fpga::set_iofilename(char* supplied_name)
{
  strcpy(io_filename,supplied_name);
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
		    char* part_tp,
		    char* part_pk,
		    int mx_speed)
{
  strcpy(part_make,part_mk);
  strcpy(part_tech,part_th);
  strcpy(part_name,part_tp);
  strcpy(part_pack,part_pk);
  speed_grade=mx_speed;
}

void Fpga::set_control_pins(const char* pin_name,
			    const char* bind_name,
			    const int bitlen,
			    const int pin_type,
			    const int pin_assert)
{
  int sig_id=ext_signals->add_pin(pin_name,bitlen-1,bitlen,
				  STD_LOGIC,pin_type,pin_assert);
  tmp_seqpins->add(sig_id);
  
  arch_bindlist->add(bind_name);
}

void Fpga::set_synthesis_file(const char* libname, const char* path, const char* filename)
{
  synthesis_libname->add(libname);
  synthesis_path->add(path);
  synthesis_files->add(filename);
}

void Fpga::set_preamble_file(const char* path, const char* filename, const int origin)
{
  preamble_path->add(path);
  preamble_files->add(filename);
  preamble_origin->add(origin);
}
void Fpga::set_postamble_file(const char* path, const char* filename, const int origin)
{
  postamble_path->add(path);
  postamble_files->add(filename);
  postamble_origin->add(origin);
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
  if (rw_control_separate)
    read_signal_id->set(memory_no,ext_signals->add_pin(memread_name,
						       0,1,
						       STD_LOGIC,OUTPUT_PIN_RAMG));
  
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

  if (rw_control_separate)
    ext_signals->set_pinpriority(read_signal_id->query(memory_no),memory_no);
}

// FIX: do not rely on a single signal to a given fpga!!
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
    child_cores=new CoreList;

  // Duplicates are not needed
  int duplicate=0;
  for (int loop=0;loop<child_cores->size();loop++)
    {
      ACSCGFPGACore* current_core=(ACSCGFPGACore*) child_cores->elem(loop);
      if (child_core->acs_id==current_core->acs_id)
	{
	  printf("Fpga::set_child:Warning:Dup sg %s found not adding\n",child_core->comment_name()); 
	  duplicate=1;
	  break;
	}
    }

  if (!duplicate)
    {
//      printf("Adding core %s\n",child_core->comment_name());
      child_cores->append(child_core);
    }
}

CoreList* Fpga::get_childcores_handle(void)
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

////////////////////////////////////////////////////////////////////////////
// Establish the timing information for the inter-fpga communication paths
// Note: Since this is an ioport, the concept of vector length is irrelevant
////////////////////////////////////////////////////////////////////////////
int Fpga::add_timing(const int fpga_no,
		     const int read_time, 
		     const int write_time)
{
  if (read_time != 0)
    {
      ifpga_timing[fpga_no].add_read(0,0,0,-1,0,read_time,0,0,0);
      if (DEBUG_TIMING)
	printf("fpga %d, adding read, count=%d\n",fpga_no,ifpga_timing[fpga_no].count);
    }
  else
    {
      ifpga_timing[fpga_no].add_write(0,0,0,-1,0,0,write_time,0,0);
      if (DEBUG_TIMING)
	printf("fpga %d, adding write, count=%d\n",fpga_no,ifpga_timing[fpga_no].count);
    }

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////////////
// Query the modality of the port for this simulation
// FIX: Only usefull in a static situation for the port!
////////////////////////////////////////////////////////
int Fpga::query_mode(const int fpga_no)
{
  if (DEBUG_TIMING)
    printf("query mode for %d\n",fpga_no);
  if (ifpga_timing[fpga_no].count==0)
    return(UNASSIGNED);

  if (DEBUG_TIMING)
    printf("return %d\n",ifpga_timing[fpga_no].get_memtype(0));

  // FIX:Should only be one entry for now
  return(ifpga_timing[fpga_no].get_memtype(0));
}

void Fpga::print_children(void)
{
  if (child_cores==NULL)
    printf("None\n");
  else
    for (int loop=0;loop<child_cores->size();loop++)
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

int Fpga::query_memconnect(const int mem_index)
{
  if (mem_connected->query(mem_index)==MEMORY_AVAILABLE)
    return(1);
  else
    return(0);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Completion time will refer to the phaser that the interface shall be sensitive towards
// for interruption generation
/////////////////////////////////////////////////////////////////////////////////////////
void Fpga::set_completiontime(const int the_time)
{
  completion_time=the_time;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Populate the generic sequencer smart generator with fpga-specific interface constructs
// FIX:Taken from the old unirate sequencer model, information here should be obtainable
//     from local structures, rather than the hardcodings observed here.
/////////////////////////////////////////////////////////////////////////////////////////
void Fpga::generate_sequencer(void)
{
  //
  // Generate local handles
  //
  if (DEBUG_SEQUENCER)
    {
      ext_signals->print_pins("ext_signals at generate_sequencer");
      tmp_seqpins->print("relevant pins");
    }

  ACSCGFPGACore* seq_core=sequencer->seq_sg;
  Pin* seq_pins=seq_core->pins;
  Pin* seq_ctrl_signals=seq_core->ctrl_signals;
//  Pin* seq_constant_signals=seq_core->constant_signals;

  //
  // Define general purpose architecture signals
  //
  seq_pins->copy_pins(ext_signals,tmp_seqpins);
  if (DEBUG_SEQUENCER)
    seq_pins->print_pins("copied pins");

  //
  // Define common external signals
  //
/*
  seq_pins->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  seq_pins->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET,AH);
  seq_pins->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL,AH);
  seq_pins->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK,AL);
  seq_pins->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  seq_pins->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);
  */

  seq_pins->add_pin("Alg_Start",0,1,STD_LOGIC,OUTPUT_PIN_START);
  seq_pins->add_pin("Alg_Done",0,1,STD_LOGIC,INPUT_PIN_STOP);
  seq_pins->add_pin("ADDR_CLR",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCLR);
  seq_pins->add_pin("Seq_Reset",0,1,STD_LOGIC,OUTPUT_PIN_SEQRESET);

  //
  // Define internal signals
  //
  seq_ctrl_signals->add_pin("Go_Addr",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Addr_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Ring_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Go_Ring",0,1,STD_LOGIC,INTERNAL_PIN);

  //
  // Define constant signals
  //
//  seq_constant_signals->add_pin("VCC",1.0,STD_LOGIC,OUTPUT_PIN_VCC);
//  seq_constant_signals->add_pin("GND",1.0,STD_LOGIC,OUTPUT_PIN_GND);

  // Set the completion time for the Alg_Done pin
  seq_pins->set_pinpriority(seq_core->find_hardpin(INPUT_PIN_STOP),completion_time);

  //
  // Add fpga interconnect pins
  //
  // FIX:Need to coordinate pin priorities with the set_interconnect_pins method!!!
  int right_port=UNASSIGNED;
  int left_port=UNASSIGNED;
  int right_index=UNASSIGNED;
  int left_index=UNASSIGNED;
  for (int loop=0;loop<ifpga_enable_id->population();loop++)
    {
      int index=ifpga_enable_id->query(loop);
      if (index != UNASSIGNED)
	{
	  seq_pins->copy_pin(ext_signals,index);
	  if ((ext_signals->query_pinpriority(index))==1)
	    {
	      right_port=query_mode(fpga_id+1);
	      right_index=index;
	    }
	  else
	    {
	      left_port=query_mode(fpga_id-1);
	      left_index=index;
	    }
	}
    }

/*
  int right_port=UNASSIGNED;
  int left_port=UNASSIGNED;
  if (fpga_id != 4)
    {
      int enable_pin=seq_pins->add_pin("Right_OE",0,36,STD_LOGIC,
				       OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_pins->set_pinpriority(enable_pin,1);
      right_port=query_mode(fpga_id+1);
    }
  if (fpga_id != 1)
    {
      int enable_pin=seq_pins->add_pin("Left_OE",0,36,STD_LOGIC,
				       OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_pins->set_pinpriority(enable_pin,0);
      left_port=query_mode(fpga_id-1);
    }
    */

  if (DEBUG_SEQUENCER)
    for (int qloop=0;qloop<5;qloop++)
      printf("query_mode(%d)=%d, fpga_id=%d\n",qloop,query_mode(qloop),fpga_id);

  // Load the code for the sequencer's I/O interface and kickoff
  // and transfer for use in the main body of the master sequencer
  // FIX: Language-independency testing needed?
  ostrstream declare_filename;
  ostrstream body_filename;
  declare_filename << io_filename << "_declaratives.vhd" << ends;
  body_filename << io_filename << "_mainbody.vhd" << ends;

  ifstream declare_file(declare_filename.str());
  ifstream body_file(body_filename.str());

  char* char_buf=new char[MAX_STR];
  while (declare_file.getline(char_buf,MAX_STR))
    seq_core->sg_declarative << char_buf << endl;
  seq_core->sg_declarative << ends;

  // Determine inter-FPGA connections and set the systolic bus controls appropriately
  // FIX: Remove once inter-FPGA control has been de-centralized
  if (right_port == SOURCE)
    seq_core->sg_body << ext_signals->query_pinname(right_index) 
	     << " <= (others => '1');" << endl;
  else
    if (fpga_id != 4)
      seq_core->sg_body << ext_signals->query_pinname(right_index) 
	<< " <= (others => '0');" << endl;
  if (left_port == SOURCE)
    seq_core->sg_body << ext_signals->query_pinname(left_index) 
	     << " <= (others => '1');" << endl;
  else
    if (fpga_id != 1)
      seq_core->sg_body << ext_signals->query_pinname(left_index)
	       << " <= (others => '0');" << endl;

/*
  if (right_port == SOURCE)
    seq_core->sg_body << "Right_OE <= (others => '1');" << endl;
  else
    if (fpga_id != 4)
      seq_core->sg_body << "Right_OE <= (others => '0');" << endl;
  if (left_port == SOURCE)
    seq_core->sg_body << "Left_OE <= (others => '1');" << endl;
  else
    if (fpga_id != 1)
      seq_core->sg_body << "Left_OE <= (others => '0');" << endl;
      */

  while (body_file.getline(char_buf,MAX_STR))
    seq_core->sg_body << char_buf << endl;
  seq_core->sg_body << ends;

  delete []char_buf;
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

