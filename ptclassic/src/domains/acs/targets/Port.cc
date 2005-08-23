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
 Version: @(#)Port.cc      1.0     04/13/00
***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Port.h"

Port::Port()
: port_type(UNKNOWN_PORT), total_sgs(0), total_assignments(0),
  io_cores(NULL),
  source_cores(NULL), sources_packed(0),
  sink_cores(NULL), sinks_packed(0),
  lut_cores(NULL),
  dataimux_star(NULL), dataomux_star(NULL), data_size(0),
  fpga_count(0), connected_fpgas(NULL), 
  port_addressing(NULL),
  portuse(PORT_UNUSED),
  bidir_data(0), separate_rw(0), read_skew(0), write_skew(0),
  port_timing(NULL), pt_count(0), port_scheduling(NULL), 
  total_latency(0)
{
}
Port::Port(int device_count)
: port_type(UNKNOWN_PORT), total_sgs(0),  total_assignments(0),
  io_cores(NULL),
  source_cores(NULL), sources_packed(0),
  sink_cores(NULL), sinks_packed(0),
  lut_cores(NULL),
  dataimux_star(NULL), dataomux_star(NULL), data_size(0),
  fpga_count(device_count),
  port_addressing(NULL),
  portuse(PORT_UNUSED),
  bidir_data(0), separate_rw(0), read_skew(0), write_skew(0),
  port_timing(NULL), pt_count(0), port_scheduling(NULL), 
  total_latency(0)
{
  connected_fpgas=new Fpga*[fpga_count];
}


Port::~Port() 
{
  port_type=UNKNOWN_PORT;
  total_sgs=0;
  total_assignments=0;
  delete []connected_fpgas;
  delete io_cores;
  delete source_cores;
  sources_packed=0;
  delete sink_cores;
  sinks_packed=0;
  delete lut_cores;
  delete dataimux_star;
  delete dataomux_star;
  delete port_addressing;
  delete port_scheduling;
  total_latency=0;

  delete port_timing;
}

int Port::init_pt(int seqlen)
{
  pt_count=seqlen;
  if (port_timing!=NULL)
    {
      printf("Port::init_pt:Error, multiple initialization\n");
      abort();
    }
  else
    port_scheduling=new ACSIntArray(seqlen,UNKNOWN);

  // Return happy condition
  return(1);
}


int Port::add_pt(int node_act,int node_type)
{
  port_scheduling->set(node_act,node_type);

  // Return happy condition
  return(1);
}


int Port::fetch_pt(int index)
{
  return(port_scheduling->query(index));
}  


/////////////////////////////////////////
// Start the core assignment process over
/////////////////////////////////////////
int Port::reset_cores()
{
  total_sgs=0;
  total_assignments=0;
  delete io_cores;
  delete source_cores;
  sources_packed=0;
  delete sink_cores;
  sinks_packed=0;
  delete lut_cores;

  io_cores=new CoreList;
  source_cores=new CoreList;
  sink_cores=new CoreList;
  lut_cores=new CoreList;

  // Return happy condition
  return(1);
}


/////////////////////////////////////////////////////////////////
// Set notification that the algorithm will need to use this port
/////////////////////////////////////////////////////////////////
int Port::activate_port(void)
{
  if (portuse==PORT_RESERVED)
    return(0);

  portuse=PORT_USED;

  // Return happy condition
  return(1);
}



///////////////////////////////////////////////////////////
// Return boolean notification should this port be utilized
///////////////////////////////////////////////////////////
int Port::port_active(void)
{
  if (portuse==PORT_USED)
    return(1);
  return(0);
}

////////////////////////////////////////////////////////////////////////////////
// Designate a port as being reserved and therefore not available for algorithms
////////////////////////////////////////////////////////////////////////////////
void Port::reserve_port(void)
{
  if (portuse==PORT_USED)
    {
      fprintf(stderr,"Port::reserve_port:Error: reserving a port in use!\n");
      abort();
    }
  portuse=PORT_RESERVED;
}

//////////////////////////////////////////////////////////////////////////
// Determine if automatic or fixed addressing is need for this memory port
// ASSUMPTION:Invoker knows that this is a memory
//////////////////////////////////////////////////////////////////////////
int Port::query_addressing(void)
{
  // If the memories have been packed then automatic addressing is needed
  if ((sources_packed) || (sinks_packed))
    return(1);

  // If any of the sources or sinks have an address that is unassigned then
  // automatic addressing is needed
  for (int loop=0;loop<source_cores->size();loop++)
    {
      ACSCGFPGACore* src_core=(ACSCGFPGACore*) source_cores->elem(loop);
      if ((src_core->address_start <0) || (src_core->address_step < 0))
	return(1);
    }
  for (int loop=0;loop<sink_cores->size();loop++)
    {
      ACSCGFPGACore* snk_core=(ACSCGFPGACore*) sink_cores->elem(loop);
      if ((snk_core->address_start <0) || (snk_core->address_step < 0))
	return(1);
    }

  // All addresses have been properly set.  Automatic addressing not needed
  return(0);
}

//////////////////////////////////////////////////////////////////////
// Each port is tasked to keep track of all source and sink cores that 
// are associated with it.  Additionally, the port will conduct set
// preliminary scheduling of all cores.  
// The sxx_assignment array will track which cores can be combined
// via bit-packing methods to reduce total latency.
//////////////////////////////////////////////////////////////////////
int Port::assign_iocore(ACSCGFPGACore* fpga_core)
{
  io_cores->append(fpga_core);

  // Return happy condition
  return(1);
}
int Port::assign_srccore(ACSCGFPGACore* fpga_core)
{
  // Track sources
  total_sgs++;
  source_cores->append(fpga_core);

  // Return happy condition
  return(1);
}
int Port::assign_snkcore(ACSCGFPGACore* fpga_core)
{
  // Track sinks
  total_sgs++;
  sink_cores->append(fpga_core);

  if (fpga_core->act_output > total_latency)
    total_latency=fpga_core->act_output;

  // Return happy condition
  return(1);
}
int Port::assign_lutcore(ACSCGFPGACore* fpga_core)
{
  lut_cores->append(fpga_core);

  // Return happy condition
  return(1);
}

int Port::schedule_src(const int src_no, const int activation_time)
{
  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) source_cores->elem(src_no);

  // Assign activation time
  fpga_core->act_input=activation_time;
  fpga_core->act_output=activation_time+read_skew;
      
  // Return happy condition
  return(1);
}
int Port::schedule_snk(const int snk_no, const int activation_time)
{
  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sink_cores->elem(snk_no);

  // Assign activation time
  fpga_core->act_input=activation_time-write_skew;
  fpga_core->act_output=activation_time;
      
  // Return happy condition
  return(1);
}
int Port::schedule_snk(ACSCGFPGACore* fpga_core, const int activation_time)
{
  // Assign activation time
  fpga_core->act_input=activation_time-write_skew;
  fpga_core->act_output=activation_time;
      
  // Return happy condition
  return(1);
}

// ASSUMPTION: Sources and Sinks only have one I/O pin
int Port::assess_port(void)
{
  int total_bitwidth=assess_bitwidths(SOURCE);

  if ((total_bitwidth <= data_size) && (total_bitwidth > 0))
    sources_packed=1;

  if (DEBUG_BITPACK)
    printf("source bitwidth=%d, available datasize=%d\n",total_bitwidth,data_size);

  // Memories have separate channels for sinking, total bandwidth calc can be zeroed
  if (port_type!=MEMORY) 
    if (total_bitwidth > data_size)
      return(0);
    else
      {
	sinks_packed=1;
	return(1);
      }

  // Only memories should be examined here!
  // FIX:Bidirectional port not treated here
  total_bitwidth=assess_bitwidths(SINK);
  
  if (DEBUG_BITPACK)
    printf("final bitwidth=%d, available datasize=%d\n",total_bitwidth,data_size);
  if ((total_bitwidth <= data_size) && (total_bitwidth > 0))
    {
      sinks_packed=1;
      return(1);
    }
  else
    return(0);
}


///////////////////////////////////////////////////////////////////////
// Examine a given set of cores assigned to this port and determine the
// total bitwidth used and return that amount.
///////////////////////////////////////////////////////////////////////
int Port::assess_bitwidths(const int type)
{
  CoreList* port_list=NULL;
  if (type==SOURCE)
    port_list=source_cores;
  else
    port_list=sink_cores;

  int total_bitwidth=0;
  for (int loop=0;loop<port_list->size();loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) port_list->elem(loop);
      Pin* fpga_pins=fpga_core->pins;
      ACSIntArray* nodes=NULL;
      if (type==SOURCE)
	nodes=fpga_pins->query_onodes();
      else
	nodes=fpga_pins->query_inodes();
      int pin=nodes->query(0);
      total_bitwidth+=fpga_pins->query_bitlen(pin);
      if (DEBUG_BITPACK)
	printf("total bitwidth increased by star %s type %d to %d\n",
	       fpga_core->comment_name(),
	       type,
	       total_bitwidth);
    }
  
  return(total_bitwidth);
}

int Port::set_launchrate(const int rate)
{
  total_launchrate=rate;

  // Return happy condition
  return(1);
}


int Port::set_port(const int data_bits,
		   const int bidirectional_bus,
		   const int rw_control_separate,
		   const int rskew,
		   const int wskew)
{
  data_size=data_bits;
  bidir_data=bidirectional_bus;
  separate_rw=rw_control_separate;
  read_skew=rskew;
  write_skew=wskew;

  // Return happy condition
  return(1);
}
int Port::set_port(const int data_bits,
		   const int addr_bits,
		   const int low_address,
		   const int high_address,
		   const int bidirectional_bus,
		   const int rw_control_separate,
		   const int rskew,
		   const int wskew)
{
  data_size=data_bits;
  bidir_data=bidirectional_bus;
  separate_rw=rw_control_separate;
  read_skew=rskew;
  write_skew=wskew;

  port_addressing=new Address;
  port_addressing->set_address(addr_bits,low_address,high_address);

  // Return happy condition
  return(1);
}

int Port::get_highaddress(void)
{
  return(port_addressing->addr_hi);
}

void Port::add_memorysupport(Sg_Constructs* construct,
			     Fpga* fpga_elem,
			     const int mem_id,
			     CoreList* sg_list)
{
  add_addressgen(construct,fpga_elem,mem_id,sg_list);
}

///////////////////////////////////////////////////////////////////////////////
// Add smart generators to function as an address generator for a given memory,
// that will be resident in a specific fpga
// FIX:Should revisit once multirate algorithms are being generated
///////////////////////////////////////////////////////////////////////////////
void Port::add_addressgen(Sg_Constructs* construct,
			  Fpga* fpga_elem,
			  const int mem_id,
			  CoreList* sg_list)
{
  if (DEBUG_ADDRESSGEN)
    printf("Adding address generator for memory %d, in fpga %d\n",mem_id,
	   fpga_elem->retrieve_acsdevice());
  	
  // Retrieve the starting addresses used by each star
  // on this memory port as well as their node activation times
  int src_sgs=source_cores->size();
  int snk_sgs=sink_cores->size();
  int lut_sgs=lut_cores->size();
  port_timing=new Port_Timing(read_skew,
			      write_skew);

  if (DEBUG_ADDRESSGEN)
    printf("This memory has %d sources and %d sinks associated\n",src_sgs,snk_sgs);

  // If automatic addressing is needed, then generate temporary storage for free addresses
  // NOTE:Will always start at address zero and work up, address stepping will be one
  int free_address=0;
  int step_factor=1;

  // Identify all source elements and source execution times
  for (int sg_loop=0;sg_loop<src_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) source_cores->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;

      // If automatic addressing is needed
      if (query_addressing())
	{
	  fpga_core->address_start=free_address;
	  fpga_core->address_step=step_factor;
	  free_address+=fpga_core->word_count;
	  if (DEBUG_ADDRESSGEN)
	    printf("automatic addressing for memory %d, source %s assigned to %d\n",
		   mem_id,
		   fpga_core->comment_name(),
		   fpga_core->address_start);
	}

      port_timing->add_read(fpga_core->address_start,
			    fpga_core->address_step,
			    fpga_core->act_launch,
			    fpga_core->word_count,
			    fpga_core->act_input,
			    fpga_core->act_output,
			    fpga_pins->query_majorbit(0),
			    fpga_pins->query_bitlen(0),
			    sg_loop);
      
      if (DEBUG_ADDRESSGEN)
	printf("added src for addr %d, time %d\n",
	       fpga_core->address_start,
	       fpga_core->act_input);
    }

  // Identify all sink elements and sink execution times
  for (int sg_loop=0;sg_loop<snk_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) sink_cores->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;
      
      // Adjust the node activation time for sink stars to 
      // reflect architecture delays
      int new_nodeact=fpga_core->act_output;
      int new_address=fpga_core->address_start;

      if (query_addressing())
	{
	  new_address=free_address;
	  fpga_core->address_start=free_address;
	  fpga_core->address_step=step_factor;
	  free_address+=fpga_core->word_count;
	  if (DEBUG_ADDRESSGEN)
	    printf("automatic addressing for memory %d, sink %s assigned to %d\n",
		   mem_id,
		   fpga_core->comment_name(),
		   fpga_core->address_start);
	}


      // If memory is shared as a source, then the source will be
      // activated before any sinks are scheduled.  Need to adjust
      // the address of the sink, so that the proper address will be
      // determined once the memory is ready to be used as a sink
      // NOTE: Obviated with new multirate sequencer
/*
      if (src_sgs)
	{
//	  float fsetback=floor(total_latency/pt_count);
	  float fsetback=floor(total_latency/total_sgs);
	  int address_setback=(int) fsetback;

	  if (DEBUG_ADDRESSGEN)
	    {
	      printf("latency=%d, pt_count(seqlen)=%d, setback=%d, fsetback=%f\n",
		     total_latency,
		     pt_count,
		     address_setback,
		     fsetback);
	      printf("Since there are sources as well as sinks, ");
	      printf("sink address %d will be moved to %d\n",
		     new_address,new_address-address_setback);
	    }
	  new_address-=address_setback;
	}
	*/

      port_timing->add_write(new_address,
			     fpga_core->address_step,
			     fpga_core->act_launch,
			     fpga_core->word_count,
			     UNASSIGNED,
			     new_nodeact,
			     fpga_pins->query_majorbit(0),
			     fpga_pins->query_bitlen(0),
			     sg_loop);
      
      if (DEBUG_ADDRESSGEN)
	printf("added sink for addr %d, time %d\n",
	       new_address,
	       new_nodeact);
    }
  
  // Identify all lut elements and lut execution times
  for (int sg_loop=0;sg_loop<lut_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) lut_cores->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;

      ACSIntArray* addresses=fpga_core->sg_get_privaledge();
      printf("LUT has %d address\n",addresses->population());

      
      for (int addr_loop=0;addr_loop<addresses->population();addr_loop++)
	{
	  // FIX:Shouldnt need to modify the input timing, need to improve ctrl signal traps
	  //     past the input and output only stages!
	  port_timing->add_lutread(addresses->query(addr_loop),
				   1,
				   fpga_core->act_launch,
				   fpga_core->word_count,
				   fpga_core->act_input+2,
				   fpga_core->act_output,
				   fpga_pins->query_majorbit(0),
				   fpga_pins->query_bitlen(0),
				   sg_loop);
	  if (DEBUG_ADDRESSGEN)
	    printf("added lut for addr %d, time %d\n",
		   addresses->query(addr_loop),
		   fpga_core->act_input);
	}
      
    }

  // Sort the addresses based on execution time and calculate the 
  // update sequence
  port_timing->sort_times();
  port_timing->calc_updseq();

  // FIX:addr_signal_id should be encapsulated with the Address class?
  //     Should the construct instance be generated by Address or shared?
  genhw_address(construct,
		src_sgs+snk_sgs,
		total_launchrate,
		fpga_elem->ext_signals,
		(fpga_elem->addr_signal_id)->get(mem_id),
		port_timing,
		sg_list);
}

void Port::genhw_address(Sg_Constructs* construct,
			 const int port_hits,
			 const int max_launch,
			 Pin* ext_signals,
			 const int addr_signal_id,
			 Port_Timing* port_timing,
			 CoreList* sg_list)
{
  // FIX:LUTs currently cannot coexist with normal sources and sinks
  int lut_sgs=lut_cores->size();

  if (port_type==MEMORY)
    if (lut_sgs==0)
      port_addressing->genhw_address(construct,
				     port_hits,
				     max_launch,
				     ext_signals,
				     addr_signal_id,
				     port_timing,
				     sg_list);
    else
      {
	ACSCGFPGACore* lut_core=(ACSCGFPGACore*) lut_cores->elem(0);
	Pin* lut_pins=lut_core->pins;
	int index_mbit=lut_pins->query_majorbit(0);
	int index_bitlen=lut_pins->query_bitlen(0);
	port_addressing->genhw_lutaddress(construct,
					  lut_core->acs_id,
					  index_mbit,
					  index_bitlen,
					  max_launch,
					  ext_signals,
					  addr_signal_id,
					  port_timing,
					  sg_list);
      }
  else
    fprintf(stderr,"Port::genhw_address:Error, attempt to create an address generator for a non-memory\n");
}

ACSCGFPGACore* Port::get_addrmux_star(void)
{
  if (port_type==MEMORY)
    return(port_addressing->addrmux_star);
  else
    fprintf(stderr,"Port::get_addrmux_star:Error, attempt to access an address generator for a non-memory port\n");
  return(NULL);
}

void Port::add_io_controller(Sg_Constructs* construct,
			     Fpga* fpga_elem,
			     const int mem_id,
			     CoreList* sg_list)
{
  if (DEBUG_IOCONTROLLER)
    printf("Port::add_io_controller:mem_id=%d\n",mem_id);

  // Determine the phases needed for activation
  ACSIntArray* address_times=port_timing->address_times();
  ACSIntArray* source_times=port_timing->access_times(SOURCE);
  ACSIntArray* sink_times=port_timing->access_times(SINK);
  ACSIntArray* lut_times=port_timing->access_times(SOURCE_LUT);
  int total_srctimes=source_times->population();
  int total_snktimes=sink_times->population();
  int total_luttimes=lut_times->population();
  int total_times=total_srctimes+total_snktimes+total_luttimes;

  if (DEBUG_IOCONTROLLER)
    {
      source_times->print("sources times");
      sink_times->print("sink times");
      lut_times->print("lut times");
    }


  // FIX: Should generalize for multiple types of io control models
  // Add a chip enable decoder
  // Last, -1 indicates that the output of the decoder has no priority assignment needed for
  // the resolver
  ACSIntArray* all_times=new ACSIntArray;
  all_times->copy(address_times);
  all_times->add(-1);
  if (DEBUG_IOCONTROLLER)
    all_times->print("all times");

  // Construct the decodering equations as a logical OR'ing of each time occurance
  ACSIntArray** decoder_eq=new ACSIntArray*[1];
  decoder_eq[0]=new ACSIntArray;
  for (int oloop=0;oloop<total_times;oloop++)
    for (int iloop=0;iloop<total_times;iloop++)
      if (oloop==iloop)
	decoder_eq[0]->add(1);
      else
	decoder_eq[0]->add(-1);
  if (DEBUG_IOCONTROLLER)
    decoder_eq[0]->print("ce decoder equation");

  ACSCGFPGACore* ce_decoder=construct->add_decoder(total_times,1,AL,
						   all_times,decoder_eq,sg_list);
  ce_decoder->add_comment("Memory chip enable");

  // Adjust resolver names for the inputs
  for (int loop=0;loop<total_times;loop++)
    {
      int in_pin=ce_decoder->find_hardpin(INPUT_PIN,all_times->query(loop));
      ce_decoder->replace_pintype(in_pin,INPUT_PIN_PHASE);
    }
  
  // Cleanup
  delete all_times;
  delete decoder_eq[0];
  delete []decoder_eq;

  // Connect the output directly
  int output_pin=ce_decoder->find_hardpin(OUTPUT_PIN);
  int input_pin=(fpga_elem->enable_signal_id)->query(mem_id);
  construct->connect_sg(ce_decoder,output_pin,UNASSIGNED,input_pin,
			fpga_elem->ext_signals,input_pin,UNASSIGNED,input_pin,EXT_NODE);

  
  // Add a read/write control enabler
  // FIX:Remedy for separate_rw control line case!
  ACSIntArray** enable_eq=new ACSIntArray*[1];
  enable_eq[0]=new ACSIntArray;
  for (int oloop=0;oloop<total_snktimes;oloop++)
    for (int iloop=0;iloop<total_snktimes;iloop++)
      if (oloop==iloop)
	enable_eq[0]->add(1);
      else
	enable_eq[0]->add(-1);
  if (DEBUG_IOCONTROLLER)
    enable_eq[0]->print("rw decoder equation");

  // Add a non-priority flag for the output pin
  ACSIntArray* write_times=new ACSIntArray;
  write_times->copy(sink_times);
  write_times->add(-1);
  ACSCGFPGACore* rw_decoder=construct->add_decoder(total_snktimes,1,AL,
						   write_times,enable_eq,sg_list);
  rw_decoder->add_comment("Memory chip RW enabler");

  // Adjust resolver names for the inputs
  for (int loop=0;loop<total_snktimes;loop++)
    {
      int in_pin=rw_decoder->find_hardpin(INPUT_PIN,write_times->query(loop));
      rw_decoder->replace_pintype(in_pin,INPUT_PIN_PHASE);
    }
  
  // Connect the output directly
  int rwoutput_pin=rw_decoder->find_hardpin(OUTPUT_PIN);
  int rwinput_pin=(fpga_elem->write_signal_id)->query(mem_id);
  construct->connect_sg(rw_decoder,rwoutput_pin,UNASSIGNED,rwinput_pin,
			fpga_elem->ext_signals,rwinput_pin,UNASSIGNED,rwinput_pin,EXT_NODE);

  // Cleanup
  delete write_times;
  delete enable_eq[0];
  delete []enable_eq;
}



// Return positive indicator if this port is indeed a memory
int Port::memory_test(void)
{
  if (port_type==MEMORY)
    return(1);
  else
    return(0);
}

// Toggle port type to the appropriate setting
void Port::designate_memory(void)
{
  if (port_type==UNKNOWN)
    port_type=MEMORY;
  else
    fprintf(stderr,"Caution:Port is being redesignated\n");
}
void Port::designate_sysinterconnect(void)
{
  if (port_type==UNKNOWN)
    port_type=SYSTOLIC_INTERCONNECT;
  else
    fprintf(stderr,"Caution:Port is being redesignated\n");
}
void Port::designate_reconfigconnect(void)
{
  if (port_type==UNKNOWN)
    port_type=RECONFIGURABLE_INTERCONNECT;
  else
    fprintf(stderr,"Caution:Port is being redesignated\n");
}

void Port::add_connected(const int device_count)
{
  fpga_count=device_count;
  connected_fpgas=new Fpga*[fpga_count];
}

void Port::set_connected(const int index, Fpga& fpga_elem)
{
  connected_fpgas[index]=&fpga_elem;
}

int Port::query_fpga_connections(void)
{
  return(fpga_count);
}

Fpga* Port::get_fpga_handle(const int index)
{
  return(connected_fpgas[index]);
}
