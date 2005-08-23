static const char file_id[] = "HWio_add.cc";

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
 Date of creation: 10/20/99
 Version: @(#)HWio_add.cc      1.0     10/20/99
***********************************************************************/
#include "HWio_add.h"

#ifdef __GNUG__
#pragma implementation
#endif

ACSIntArray* ACSCGFPGATarget::HWpack_bits(ACSIntArray* io_list, const int type, CoreList* sg_list)
{
  ACSIntArray* unneeded_cores=new ACSIntArray;
  for (int loop=0;loop<io_list->population();loop++)
    {
      int io_instance=io_list->query(loop);
      if (DEBUG_PACK)
	printf("HWpack_bits:Compressing port type %d, number %d\n",type,io_instance);

      Port* io_port=NULL;
      if (type==MEMORY)
	{
	  // For memories, pack sources and sinks separately
	  io_port=arch->get_memory_handle(io_instance);
	  if (io_port->sources_packed)
	    HWadd_packer(io_port,MEMORY,SOURCE,loop,sg_list,unneeded_cores);
	  if (io_port->sinks_packed)
	    HWadd_packer(io_port,MEMORY,SINK,loop,sg_list,unneeded_cores);

	  CoreList* source_cores=io_port->source_cores;
	  CoreList* sink_cores=io_port->sink_cores;
	  io_port->total_sgs=source_cores->size()+sink_cores->size();
	}
      else if (type==SYSTOLIC)
	{
	  io_port=arch->get_systolic_handle(io_instance);
	  if (io_port->sources_packed)
	    {
	      CoreList* io_cores=io_port->io_cores;
	      HWadd_packer(io_port,SYSTOLIC,SOURCE,loop,sg_list,unneeded_cores);

	      io_cores=io_port->io_cores;
	      HWadd_packer(io_port,SYSTOLIC,SINK,loop,sg_list,unneeded_cores);
	    }
	}
      else
	fprintf(stderr,"HWpack_bits::Error, unknown io type handle passed\n");
    }
  
  // Pass purge list back to parent
  return(unneeded_cores);
}

void ACSCGFPGATarget::HWadd_packer(Port* io_port,
				   const int type,
				   const int dir_type,
				   const int io_instance,
				   CoreList* sg_list,
				   ACSIntArray* unneeded_cores)
{
  // Determine the list of cores that we should be concerned about
  CoreList* core_list=NULL;
  if (dir_type==SOURCE)
    core_list=io_port->source_cores;
  else
    core_list=io_port->sink_cores;
  if (type==SYSTOLIC)
    core_list=io_port->io_cores;

  // Identify the source/sink to be preserved
  // Memories are trivial, for systolics the device must be obtained via
  // the privaledged settings that describe the devices that are spanned
  ACSCGFPGACore* preserved_core=NULL;
  if (type==MEMORY)
    preserved_core=(ACSCGFPGACore*) core_list->elem(0);
  else
    {
      CoreList* io_cores=io_port->io_cores;
      preserved_core=(ACSCGFPGACore*) io_cores->elem(0);
    }

  // Determine the parent Fpga for this io packing session
  Fpga* target_fpga=NULL;
  if (type==MEMORY)
    target_fpga=arch->get_fpga_handle(preserved_core->acs_device);
  else
    {
      int device_no=-1;
      ACSIntArray* devices=preserved_core->sg_get_privaledge();
      if (dir_type==SOURCE)
	device_no=devices->query(1);
      else 
	device_no=devices->query(0);
      target_fpga=arch->get_fpga_handle(device_no);
    }
  construct->set_targetdevice(target_fpga);
  if (DEBUG_PACK)
    printf("preserved_core is %s out of %d cores, acs_device=%d\n",
	   preserved_core->comment_name(),
	   core_list->size(),
	   preserved_core->acs_device);

  ACSCGFPGACore* packer_core=NULL;
  if (dir_type==SOURCE)
    packer_core=construct->add_unpacker(core_list->size(),smart_generators);
  else
    packer_core=construct->add_packer(core_list->size(),smart_generators);
  
  if (dir_type==SOURCE)
    {
      if (type==MEMORY)
	packer_core->add_comment("Source unpacker for memory",io_instance);
      else if (type==SYSTOLIC)
	packer_core->add_comment("Source unpacker for systolic",io_instance);
    }
  else
    {
      if (type==MEMORY)
	packer_core->add_comment("Sink packer for memory",io_instance);
      else if (type==SYSTOLIC)
	packer_core->add_comment("Sink packer for systolic",io_instance);
    }
  
  packer_core->bitslice_strategy=PRESERVE_LSB;
	      
  // Exempt these bit widths as they are set by the architecture model
  packer_core->bw_exempt=1;


  // Insert the packer between the algorithm cores and the Source/Sink cores
  for (int loop=0;loop<core_list->size();loop++)
    {
      ACSCGFPGACore* core=(ACSCGFPGACore*) core_list->elem(loop);
      if (dir_type==SOURCE)
	HWio_reconnect_src(packer_core,core,OUTPUT_PIN);
      else
	HWio_reconnect_snk(core,packer_core,INPUT_PIN);
	
    }
  packer_core->update_sg(LOCKED,UNLOCKED);

  // Remove any unnecessary cores
  // NOTE:Element zero IS the preserved one, all others are unnecessary
  for (int loop=1;loop<core_list->size();loop++)
    {
      ACSCGFPGACore* core=(ACSCGFPGACore*) core_list->elem(loop);
      unneeded_cores->add(core->acs_id);
      if (DEBUG_PACK)
	printf("core %s id=%d unneeded, added to list for removal\n",
	       core->comment_name(),
	       core->acs_id);
    }

  // Only one source/sink represents the packing of multiple sources/sinks
  // FIX:Assumptions taken on pin numbers!
  if (dir_type==SOURCE)
    {
      Pin* packer_pins=packer_core->pins;
      int mbit=packer_pins->query_majorbit(0);
      int bitlen=packer_pins->query_bitlen(0);
      preserved_core->set_precision(1,mbit,bitlen,LOCKED);

      construct->connect_sg(preserved_core,packer_core);

      packer_core->act_input=preserved_core->act_output;
      packer_core->act_output=packer_core->act_input;
      packer_core->act_launch=preserved_core->act_launch;
      packer_core->word_count=preserved_core->word_count;
      packer_core->act_complete=preserved_core->act_complete;
    }
  else
    {
      Pin* packer_pins=packer_core->pins;
      int mbit=packer_pins->query_majorbit((packer_pins->query_pincount())-1);
      int bitlen=packer_pins->query_bitlen((packer_pins->query_pincount())-1);
      preserved_core->set_precision(0,mbit,bitlen,LOCKED);

      construct->connect_sg(packer_core,preserved_core);

      packer_core->act_input=preserved_core->act_input;
      packer_core->act_output=preserved_core->act_input;
      packer_core->act_launch=preserved_core->act_launch;
      packer_core->word_count=preserved_core->word_count;
      packer_core->act_complete=preserved_core->act_complete;
    }

  // Reduce the port's list of io resources
  if ((type==MEMORY) || (dir_type==SINK))
    {
      delete core_list;
      core_list=new CoreList;
      core_list->append(preserved_core);
    }
}


////////////////////////////////////////////////////
// Master routine for adding all supporting hardware
////////////////////////////////////////////////////
int ACSCGFPGATarget::HWadd_support()
{
  /////////////////////////////////////////////
  // Examine each fpga
  // If used, then a master sequencer is needed
  /////////////////////////////////////////////
  for (int fpga_loop=0;fpga_loop<arch->fpga_count;fpga_loop++)
    {
      if (DEBUG_IOADD)
	printf("HWio_add:Examing fpga %d\n",fpga_loop);

      Fpga* fpga_elem=arch->get_fpga_handle(fpga_loop);
      if (fpga_elem->usage_state==FPGA_USED)
	{
	  if (DEBUG_IOADD)
	    printf("HWio_add:Adding a sequencer for fpga %d\n",fpga_loop);

	  ////////////////
	  // Add sequencer
	  ////////////////
	  construct->set_targetdevice(fpga_elem);
	  HWio_add_sequencer(fpga_elem);
	}
    }

  // Return happy condition
  return(1);
}


////////////////////////////////////////////////////////////////
// Look for implicit fpga interconnects and make them explicit
// NOTE:Memories are currently not considered as a routing means
////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWsegment_alg(CoreList* sg_list)
{
  int sg_count=sg_list->size();
  for (int sg_loop=0;sg_loop<sg_count;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;
      int src_device=HWreturn_fpgadevice(fpga_core);
      ACSIntArray* opins=fpga_pins->query_onodes();
      for (int opin_loop=0;opin_loop<opins->population();opin_loop++)
	{
	  int opin=opins->query(opin_loop);
	  int major_bit=fpga_pins->query_majorbit(opin);
	  int bitlen=fpga_pins->query_bitlen(opin);

	  Connectivity* onodes=fpga_pins->query_connection(opin);
	  for (int onode_loop=0;onode_loop<onodes->node_count;onode_loop++)
	    {
	      int dest_id=onodes->query_acsid(onode_loop);
	      ACSCGFPGACore* dest_core=HWfind_star(dest_id);
	      int dest_device=HWreturn_fpgadevice(dest_core);
	      // Check for partition
	      if (DEBUG_PARTITION)
		printf("Looking for segmentation between %s and %s\n",
		       fpga_core->comment_name(),
		       dest_core->comment_name());
	      if (src_device!=dest_device)
		{
		  if (DEBUG_PARTITION)
		    printf("Segmenting between src %s (device %d) and dest %s (device %d)\n",
			   fpga_core->comment_name(),
			   src_device,
			   dest_core->comment_name(),
			   dest_device);

		  // FIX:Reduce this section to utilize a route class
		  //     As well as a single solution assumption!
		  arch->find_routes(src_device,dest_device);
		  int shortest_route=-1;
		  int worst_cost=999;
		  for (int rloop=0;rloop<arch->total_solutions;rloop++)
		    {
		      ACSIntArray* a_solution=arch->route_solutions[rloop];
		      if (a_solution->population() < worst_cost)
			{
			  worst_cost=a_solution->population();
			  shortest_route=rloop;
			}
		    }
		  ACSIntArray* the_route=arch->route_solutions[shortest_route];
		  int prev_device=UNASSIGNED;
		  int curr_device=UNASSIGNED;
		  int next_device=UNASSIGNED;
		  
		  if (DEBUG_PARTITION)
		    the_route->print("route solution");

		  // Temp holder for incremental insertions
		  ACSCGFPGACore* base_core=NULL;
		  
		  // Temp holder for the current ioport core
		  ACSCGFPGACore* ioport_core=NULL;

		  int ports_needed=the_route->population()-1;
		  
		  for (int rloop=0;rloop<ports_needed;rloop++)
		    {
		      int input_pin=UNASSIGNED;
		      int output_pin=UNASSIGNED;
		      int output_node=UNASSIGNED;

		      // Keep track of who goes to where
		      if (rloop!=0)
			{
			  prev_device=curr_device;
			  output_pin=1;
			  output_node=UNASSIGNED;
			}
		      else
			{
			  base_core=fpga_core;
			  output_pin=opin;
			  output_node=onode_loop;
			  input_pin=onodes->query_pinid(onode_loop);
			}

		      curr_device=the_route->get(rloop);
		      if (rloop!=the_route->population()-1)
			next_device=the_route->get(rloop+1);
		      if (DEBUG_PARTITION)
			printf("Working from %d to %d going to %d (rloop=%d)\n",
			       prev_device,curr_device,next_device,rloop);
		      
		      // Set the constructor to the current device
		      construct->set_targetdevice(arch->get_fpga_handle(curr_device));

		      // Preserve data type integrity across partitions
		      int sign_convention=dest_core->sign_convention;
//		      int bitslice_strategy=dest_core->bitslice_strategy;
		      int bitslice_strategy=PRESERVE_LSB;

		      // Add an ioport
		      // FIX: Should be variable in terms of port type
		      ioport_core=construct->add_ioport(sign_convention,sg_list);
		      ioport_core->bitslice_strategy=bitslice_strategy;
		      ioport_core->acs_device=arch->determine_systolic_handle(curr_device,next_device);
		      Port* sys_port=arch->get_systolic_handle(ioport_core->acs_device);
		      sys_port->activate_port();
		      sys_port->assign_iocore(ioport_core);

		      // Set up scheduling info
		      // NOTE:Architecture delays need to be accounted for, since the distinction is lost in
		      //      the schedule from wordlength as to the need for algorithmic delay
		      ioport_core->act_input=base_core->act_output;
		      ioport_core->act_output=ioport_core->act_input+ioport_core->acs_delay;
		      ioport_core->act_launch=base_core->act_launch;
		      ioport_core->word_count=base_core->word_count;
		      ioport_core->act_complete=ioport_core->act_output+(ioport_core->word_count*ioport_core->act_launch);

		      // Set source and sink device via privaledged core access
		      ioport_core->sg_set_privaledge(0,curr_device);
		      ioport_core->sg_set_privaledge(1,next_device);
		      if (DEBUG_PARTITION)
			printf("ioport_core type = %d\n",ioport_core->acs_type);

		      // Input node is zero since there can only be one 
		      // input...hopefully;)
		      int input_node=0;
		      if (DEBUG_PARTITION)
			printf("Inserting %s between %s and %s\n",
			       ioport_core->comment_name(),
			       base_core->comment_name(),
			       dest_core->comment_name());
		      construct->insert_sg(base_core,
					   ioport_core,
					   dest_core,
					   output_pin,
					   output_node,
					   DATA_NODE,
					   input_pin,
					   input_node,
					   onodes->query_pintype(onode_loop));

		      // Set precisions on the ioport_core
		      Pin* io_pins=ioport_core->pins;
		      int io_in=io_pins->retrieve_type(INPUT_PIN);
		      int io_out=io_pins->retrieve_type(OUTPUT_PIN);
		      io_pins->set_precision(io_in,major_bit,bitlen,LOCKED);
		      io_pins->set_precision(io_out,major_bit,bitlen,LOCKED);

		      // Watch for port-to-port connections. 
		      // Adding a buffer between ports should ensure that
		      // there won't be any issues with EXT to EXT node connections.
		      // Buffer is not needed for last port to alg connection
		      if (base_core->acs_type != BOTH)
			{
			  construct->set_targetdevice(arch->get_fpga_handle(curr_device));
			  ACSCGFPGACore* buffer_core=construct->add_buffer(sign_convention,sg_list);
			  buffer_core->add_comment("Inter-fpga buffer (output)");
			  buffer_core->bitslice_strategy=bitslice_strategy;

			  // Insert timing information to inhibit pipe alignment
			  buffer_core->act_input=base_core->act_output;
			  buffer_core->act_output=ioport_core->act_input;
			  buffer_core->act_launch=base_core->act_launch;
			  buffer_core->act_complete=base_core->act_complete;
			  buffer_core->word_count=base_core->word_count;
			  
			  // FIX: Liberties taken on buffer core pins
			  if (DEBUG_PARTITION)
			    printf("Inserting %s between %s and %s\n",
				   buffer_core->comment_name(),
				   ioport_core->comment_name(),
				   dest_core->comment_name());
			  construct->insert_sg(base_core,
					       buffer_core,
					       ioport_core,
					       output_pin,
					       output_node,
					       DATA_NODE,
					       io_in,
					       0,
					       onodes->query_pintype(onode_loop));
			  
			  sys_port->assign_srccore(buffer_core);
			}
		      else
			sys_port->assign_srccore(base_core);

		      if ((ports_needed > 1) && (rloop != ports_needed-1) 
			  || (dest_core->acs_type != BOTH))
			{
			  construct->set_targetdevice(arch->get_fpga_handle(next_device));
			  ACSCGFPGACore* buffer_core=construct->
			    add_buffer(sign_convention,sg_list);
			  buffer_core->add_comment("Inter-fpga buffer (input)");
			  buffer_core->bitslice_strategy=bitslice_strategy;
			  
			  // Insert timing information to inhibit pipe alignment
			  buffer_core->act_input=ioport_core->act_output;
			  buffer_core->act_output=ioport_core->act_output;
			  buffer_core->act_launch=ioport_core->act_launch;
			  buffer_core->act_complete=ioport_core->act_complete;
			  buffer_core->word_count=ioport_core->word_count;
			  
			  // FIX: Liberties taken on buffer core pins
			  if (DEBUG_PARTITION)
			    printf("Inserting %s between %s and %s\n",
				   buffer_core->comment_name(),
				   ioport_core->comment_name(),
				   dest_core->comment_name());
			  construct->insert_sg(ioport_core,
					       buffer_core,
					       dest_core,
					       io_out,
					       0,
					       DATA_NODE,
					       input_pin,
					       input_node,
					       onodes->query_pintype(onode_loop));
			  base_core=buffer_core;

			  // Notify that this fpga will be used
			  // It may not have already been flagged by an algorithmic smart generator
			  Fpga* fpga_elem=arch->get_fpga_handle(next_device);
			  fpga_elem->usage_state=FPGA_USED;

			  sys_port->assign_snkcore(buffer_core);
			}
		      else
			{
			  // Prepare for next insertion (if applicable)
			  base_core=ioport_core;
			  sys_port->assign_snkcore(dest_core);
			}
		    }
		}
	    }
	}
    }
  
  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////////////////////////////////
// This method will return the fpga device number closely associated with the core
//////////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWreturn_fpgadevice(ACSCGFPGACore* fpga_core)
{
  int fpga_device=UNASSIGNED;
  if (fpga_core->acs_type==BOTH)
    fpga_device=fpga_core->acs_device;
  else
    // Need a handle to the fpga, not the memory
    fpga_device=arch->mem_to_fpga(fpga_core->acs_device);

  return(fpga_device);
}


/////////////////////////////////////////////////////////
// Add all memory-related, support components in hardware
// FIX: Contain this within the Arch and children classes...PLEASE!
/////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWio_add(CoreList* sg_list)
{
  /////////////////////////////////////////////////////////////////
  // Examine each memory port
  // Any port that supports multiple outputs need muxing and piping
  // FIX:Collapse down to their respective classes....please!;)
  /////////////////////////////////////////////////////////////////
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      if (DEBUG_IOADD)
	printf("HWio_add:Examining memory %d\n",mem_loop);

      Port* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->port_active())
	{
	  if (DEBUG_IOADD)
	    printf("HWio_Add:This memory is being used\n");

	  ///////////////////////////////
	  // Determine the connected fpga
	  // FIX:Only one allowed for now
	  ///////////////////////////////
//	  int connected_fpgas=mem_port->query_fpga_connections();
	  for (int fpga_loop=0;fpga_loop<arch->fpga_count;fpga_loop++)
	    {
	      Fpga* fpga_elem=arch->get_fpga_handle(fpga_loop);
	      if (fpga_elem->query_memconnect(mem_loop)==MEMORY_AVAILABLE)
		{
		  construct->set_targetdevice(fpga_elem);

		  ////////////////////////
		  // Add address generator
		  ////////////////////////
		  if (mem_port->memory_test())
		    mem_port->add_memorysupport(construct,fpga_elem,mem_loop,sg_list);
//		  HWio_add_addressgen(fpga_elem,mem_port,mem_loop);

		  ////////////////////////////////////////////
		  // Add data routers and connect to algorithm
		  ////////////////////////////////////////////
		  HWio_add_dataio(fpga_elem,mem_port,mem_loop);
		  
		  ////////////////////
		  // Add io controller
		  ////////////////////
		  mem_port->add_io_controller(construct,fpga_elem,mem_loop,sg_list);
		}
	    }
	}
    }

  //////////////////////////////////////////////////////////////////////////
  // Now examine each systolic interconnect, in search of device partitions
  // If they do exist, then remap these smart generators to their respective
  // external node
  //////////////////////////////////////////////////////////////////////////
  int sys_count=arch->systolic_count;
  for (int loop=0;loop<sys_count;loop++)
    {
      Port* io_port=arch->get_systolic_handle(loop);
      if (io_port->port_active())
	{
	  if (DEBUG_IOADD)
	    printf("io port %d is active\n",loop);

	  // Should only be one port in this list!!
	  CoreList* io_ports=io_port->io_cores;
	  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) io_ports->elem(0);
	  HWremap_ioport(fpga_core,sg_list);
	}
    }
  
  //
  // Now propagate bit widths
  //
  // FIX:Reactivate once separated into a smart generator!
  HWset_bw(smart_generators);
  
  // Return happy condition
  return(1);
}


/////////////////////////////////////////////
// Add a smart generator for the io sequencer
/////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_sequencer(Fpga* fpga_elem)
{
  fpga_elem->sequencer=new Sequencer;
  Sequencer* sequencer=fpga_elem->sequencer;
  sequencer->seq_sg=construct->add_sg("ACS","UniSeq","CGFPGA",
				      BOTH,UNSIGNED,smart_generators);
}


///////////////////////////////////////////////////////////////////////////////
// Add smart generators to function as an address generator for a given memory,
// that will be resident in a specific fpga
// FIX:Should revisit once multirate algorithms are being generated
///////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_addressgen(Fpga* fpga_elem,
					  Port* mem_port,
					  const int mem_id)
{
  if (DEBUG_IOADD)
    printf("Adding address generator for memory %d, in fpga %d\n",mem_id,
	   fpga_elem->retrieve_acsdevice());

  	
  // Retrieve the starting addresses used by each star
  // on this memory port as well as their node activation times
  CoreList* source_sgs=mem_port->source_cores;
  CoreList* sink_sgs=mem_port->sink_cores;
  int src_sgs=source_sgs->size();
  int snk_sgs=sink_sgs->size();
  mem_port->port_timing=new Port_Timing(mem_port->read_skew,
					mem_port->write_skew);

  if (DEBUG_IOADD)
    printf("This memory has %d sources and %d sinks associated\n",src_sgs,snk_sgs);

  // Allocate storage for timing information
  Port_Timing* port_timing=mem_port->port_timing;

  // If automatic addressing is needed, then generate temporary storage for free addresses
  // NOTE:Will always start at address zero and work up, address stepping will be one
  int free_address=0;
  int step_factor=1;

  // Identify all source elements and source execution times
  for (int sg_loop=0;sg_loop<src_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) source_sgs->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;

      if (automatic_addressing)
	{
	  fpga_core->address_start=free_address;
	  fpga_core->address_step=step_factor;
//	  free_address+=vector_length;
	  free_address+=fpga_core->word_count;
	  if (DEBUG_IOADD)
	    printf("automatic addressing for memory %d, source %s assigned to %d\n",
		   mem_id,
		   fpga_core->comment_name(),
		   fpga_core->address_start);
	}

      port_timing->add_read(fpga_core->address_start,
			    fpga_core->address_step,
			    fpga_core->act_launch,
//			    vector_length,
			    fpga_core->word_count,
			    fpga_core->act_input,
			    fpga_core->act_output,
			    fpga_pins->query_majorbit(0),
			    fpga_pins->query_bitlen(0),
			    sg_loop);
      
      if (DEBUG_IOADD)
	printf("added src for addr %d, time %d\n",
	       fpga_core->address_start,
	       fpga_core->act_input);
    }

  // Identify all sink elements and sink execution times
  for (int sg_loop=0;sg_loop<snk_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) sink_sgs->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;
      
      // Adjust the node activation time for sink stars to 
      // reflect architecture delays
      int new_nodeact=fpga_core->act_output;
      int new_address=fpga_core->address_start;

      if (automatic_addressing)
	{
	  new_address=free_address;
	  fpga_core->address_start=free_address;
	  fpga_core->address_step=step_factor;
//	  free_address+=vector_length;
	  free_address+=fpga_core->word_count;
	  if (DEBUG_IOADD)
	    printf("automatic addressing for memory %d, sink %s assigned to %d\n",
		   mem_id,
		   fpga_core->comment_name(),
		   fpga_core->address_start);
	}


      // If memory is shared as a source, then the source will be
      // activated before any sinks are scheduled.  Need to adjust
      // the address of the sink, so that the proper address will be
      // determined once the memory is ready to be used as a sink
      if (src_sgs)
	{
	  float fsetback=floor(mem_port->total_latency/seqlen);
	  int address_setback=(int) fsetback;

	  if (DEBUG_IOADD)
	    {
	      printf("latency=%d, seqlen=%d, setback=%d, fsetback=%f\n",
		     mem_port->total_latency,
		     seqlen,
		     address_setback,
		     fsetback);
	      printf("Since there are sources as well as sinks, ");
	      printf("sink address %d will be moved to %d\n",
		     new_address,new_address-address_setback);
	    }
	  new_address-=address_setback;
	}

      port_timing->add_write(new_address,
			     fpga_core->address_step,
			     fpga_core->act_launch,
//			     vector_length,
			     fpga_core->word_count,
			     UNASSIGNED,
			     new_nodeact,
			     fpga_pins->query_majorbit(0),
			     fpga_pins->query_bitlen(0),
			     sg_loop);
      
      if (DEBUG_IOADD)
	printf("added sink for addr %d, time %d\n",
	       new_address,
	       new_nodeact);
    }
  
  // Sort the addresses based on execution time and calculate the 
  // update sequence
  port_timing->sort_times();
  port_timing->calc_updseq();

  // FIX:addr_signal_id should be encapsulated with the Address class?
  //     Should the construct instance be generated by Address or shared?
  construct->set_targetdevice(fpga_elem);
  mem_port->genhw_address(construct,
			  src_sgs+snk_sgs,
			  vector_length,
			  fpga_elem->ext_signals,
			  (fpga_elem->addr_signal_id)->get(mem_id),
			  port_timing,
			  smart_generators);
}




///////////////////////////////////////////////////////////////
// Make the connections from a (shared) memory to the algorithm
///////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_dataio(Fpga* fpga_elem,
				      Port* io_port,
				      const int io_id)
{
  // Determine source and sink pointers
  CoreList* source_sgs=io_port->source_cores;
  CoreList* sink_sgs=io_port->sink_cores;
  CoreList* lut_sgs=io_port->lut_cores;
  int src_count=source_sgs->size();
  int snk_count=sink_sgs->size();
  int lut_count=lut_sgs->size();

  if (DEBUG_IOADD)
    printf("IoPort %d, src_count=%d, snk_count=%d, lut_count=%d\n",
	   io_id,
	   src_count,
	   snk_count,
	   lut_count);
  
  int sources_reconnected=FALSE;
  int sinks_reconnected=FALSE;


  // Currently we cannot support a memory being both a LUT and used for algorithmic data storage
  if (lut_count > 0)
    {
      if ((src_count > 0) || (snk_count > 0))
	{
	  fprintf(stderr,"ERROR:Memory cannot support both LUT and Src and/or Snk capability!\n");
	  exit(1);
	}
      if (lut_count > 1)
	{
	  fprintf(stderr,"ERROR:Memory cannot multiple LUTs!\n");
	  exit(1);
	}

      if (DEBUG_IOADD)
	printf("IoPort %d is used as a LUT\n",io_id);
      ACSCGFPGACore* alg_core=(ACSCGFPGACore *) lut_sgs->elem(0);
      int data_sigid=(fpga_elem->datain_signal_id)->get(io_id);
      HWio_add_reconnect(alg_core,NULL,fpga_elem->ext_signals,
			 INPUT_PIN,data_sigid,EXT_NODE,
			 io_port);
    }
	  
  // Need to trap for bidirectional or split io signals
  if ((snk_count > 1) || (src_count > 1) || (snk_count+src_count > 1))
    {
      if (DEBUG_IOADD)
	printf("IoPort %d has multiple connections (%d src, %d snk)\n",
	       io_id,
	       src_count,
	       snk_count);

      // Does this architecture provide a bidirectional ioport model
      // or separate data baths?
      if (io_port->bidir_data)
	{
	  if (snk_count+src_count > 1)
	    {
	      HWio_add_bidirio(fpga_elem,io_port,io_id);
	      sources_reconnected=TRUE;
	      sinks_reconnected=TRUE;
	    }
	}
      else
	{
	  if (src_count > 1)
	    {
	      HWio_add_dualio(fpga_elem,io_port,io_id,SOURCE);
	      sources_reconnected=TRUE;
	    }
	  if (snk_count > 1)
	    {
	      HWio_add_dualio(fpga_elem,io_port,io_id,SINK);
	      sinks_reconnected=TRUE;
	    }
	}
    } // 	if ((mux_cond1) || (mux_cond2))
  
  if (!sources_reconnected || !sinks_reconnected)
    {
      //
      // Hook up the ram ports directly...if there is something to hookup
      //
      int type;
      int data_sigid=-1;
      if ((!sources_reconnected) && (src_count != 0))
	{
	  // A bit redundant, the source star should be associated with the
	  // controlling fpga
	  ACSCGFPGACore* alg_core=(ACSCGFPGACore *) source_sgs->elem(0);
	  type=INPUT_PIN;
	  data_sigid=(fpga_elem->datain_signal_id)->get(io_id);
	  HWio_add_reconnect(alg_core,NULL,fpga_elem->ext_signals,
			     type,data_sigid,EXT_NODE,
			     io_port);
	}
      if ((!sinks_reconnected) && (snk_count != 0))
	{
	  ACSCGFPGACore* alg_core=(ACSCGFPGACore *) sink_sgs->elem(0);
	  type=OUTPUT_PIN;
	  data_sigid=(fpga_elem->dataout_signal_id)->get(io_id);
	  HWio_add_reconnect(alg_core,NULL,
			     fpga_elem->ext_signals,
			     type,data_sigid,EXT_NODE,
			     io_port);
	}
    }
}


//////////////////////////////////////////////////////////
// Hook up the data paths to a single bidirectional ioport
//////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_bidirio(Fpga* fpga_elem, 
				       Port* io_port, const int io_id)
{
  CoreList* source_stars=io_port->source_cores;
  CoreList* sink_stars=io_port->sink_cores;
  int src_stars=source_stars->size();
  int snk_stars=sink_stars->size();

  // Hook up sourcing smart generators to this MUX
  ACSCGFPGACore* mux_sg;
  mux_sg=io_port->dataimux_star=construct->add_mux(src_stars+snk_stars,
						   snk_stars,
						   SIGNED,
						   smart_generators);
  mux_sg->add_comment("Bidirectional MUX for ioport",io_id);
  mux_sg->bitslice_strategy=PRESERVE_LSB;

  // Exempt these bit widths as they are set by the architecture model
  mux_sg->bw_exempt=1;

  for (int sg_loop=0;sg_loop<src_stars+snk_stars;sg_loop++)
    {
      ACSCGFPGACore* alg_core=NULL;
      int mempin_type;
      int muxpin_type;

      // Determine information on the Source/Sink star
      if (sg_loop < src_stars)
	{
	  alg_core=(ACSCGFPGACore *) source_stars->elem(sg_loop);
	  mempin_type=INPUT_PIN_MEMORY;
	  muxpin_type=OUTPUT_PIN_MUX_SWITCHABLE;
	}
      else
	{
	  alg_core=(ACSCGFPGACore *) sink_stars->elem(sg_loop-src_stars);
	  mempin_type=OUTPUT_PIN_MEMORY;
	  muxpin_type=INPUT_PIN_MUX_SWITCHABLE;
	}
      
      if (DEBUG_IOADD)
	printf("Connect mux port %d to star id %d\n",
	       sg_loop,
	       alg_core->acs_id);
      
      // Swap ALL the Source/Sink connections with the MUX switchable pins
      HWio_add_reconnect(alg_core,
			 mux_sg,
			 mux_sg->pins,
			 mempin_type,
			 muxpin_type,
			 DATA_NODE,
			 io_port);
      
      // Hook up mux output to the ram path
      // FIX:The free_pintype may not be needed as it would trap as an
      //     OUTPUT_PIN during a search.  Ok for now...
      int outmux_pintype=UNASSIGNED;
      if ((src_stars > 0) && (snk_stars > 0))
	outmux_pintype=BIDIR_PIN_MUX_RESULT;
      if (snk_stars==0)
	outmux_pintype=OUTPUT_PIN_MUX_RESULT;
      if (src_stars==0)
	outmux_pintype=INPUT_PIN_MUX_RESULT;

      int src_pin=(mux_sg->pins)->free_pintype(outmux_pintype);
      (mux_sg->pins)->set_precision(src_pin,
				    io_port->data_size-1,
				    io_port->data_size,
				    LOCKED);
	    
      construct->connect_sg(mux_sg,
			    src_pin,UNASSIGNED,
			    (fpga_elem->datain_signal_id)->get(io_id),
			    fpga_elem->ext_signals,
			    (fpga_elem->datain_signal_id)->get(io_id),UNASSIGNED,
			    (fpga_elem->datain_signal_id)->get(io_id),
			    EXT_NODE);
    }

}


/////////////////////////////////////////////////
// Hook up the data paths to a dual ported ioport
/////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_dualio(Fpga* fpga_elem, 
				      Port* io_port,
				      const int io_id,
				      const int port_type)
{
  CoreList* io_stars=NULL;
  Port_Timing* io_timing=io_port->port_timing;

  int sg_count=0;
  int mempin_type;
  int muxswitchable_type;
  int muxresult_type;
  int data_signal;
  ACSCGFPGACore* mux_sg=NULL;
  if (port_type==SOURCE)
    {
      io_stars=io_port->source_cores;
      sg_count=io_stars->size();
      io_port->dataimux_star=mux_sg=construct->add_mux(sg_count,
						       0,
						       SIGNED,
						       smart_generators);
      mux_sg->add_comment("Input MUX from ioport",io_id);
      mux_sg->bitslice_strategy=PRESERVE_LSB;
      mempin_type=INPUT_PIN_MEMORY;
      muxswitchable_type=OUTPUT_PIN_MUX_SWITCHABLE;
      muxresult_type=INPUT_PIN_MUX_RESULT;
      data_signal=(fpga_elem->datain_signal_id)->get(io_id);
    }
  else
    {
      io_stars=io_port->sink_cores;
      sg_count=io_stars->size();
      io_port->dataomux_star=mux_sg=construct->add_mux(sg_count,
						       sg_count,
						       SIGNED,
						       smart_generators);
      mux_sg->add_comment("Output MUX from memory",io_id);
      mux_sg->bitslice_strategy=PRESERVE_LSB;
      mempin_type=OUTPUT_PIN_MEMORY;
      muxswitchable_type=INPUT_PIN_MUX_SWITCHABLE;
      muxresult_type=OUTPUT_PIN_MUX_RESULT;
      data_signal=(fpga_elem->dataout_signal_id)->get(io_id);
    }
  
  // Add mux ctrl signals
  int ctrl_sigs=(int) floor(log(sg_count)/log(2));
  ACSIntArray* decoder_times=io_timing->access_times(port_type);
  int sensy_sigs=decoder_times->population();
  if (DEBUG_IOADD)
    {
      printf("MUXCTRLs:ctrl_sigs=%d, sensy_sigs=%d\n",ctrl_sigs,sensy_sigs);
      decoder_times->print("sensy times");
    }

  ACSIntArray** decoder_eq=new ACSIntArray*[ctrl_sigs];
  for (int ctrl_loop=0;ctrl_loop<ctrl_sigs;ctrl_loop++)
    {
      // Add output priorities for connecting with mux ctrls
      decoder_times->add(ctrl_loop);

      // Add the decoding equations
      decoder_eq[ctrl_loop]=new ACSIntArray;

      for (int sense_loop=0;sense_loop<sensy_sigs;sense_loop++)
	if (bselcode(sense_loop,ctrl_loop)==1)
	  decoder_eq[ctrl_loop]->add(1);
	else
	  decoder_eq[ctrl_loop]->add(-1);
    }
  ACSCGFPGACore* mux_decoder=construct->add_decoder(sensy_sigs,ctrl_sigs,
						    decoder_times,decoder_eq,smart_generators);

  for (int loop=0;loop<ctrl_sigs;loop++)
    {
      int out_pin=mux_decoder->find_hardpin(OUTPUT_PIN,loop);
      int in_pin=mux_sg->find_hardpin(INPUT_PIN_CTRL,loop);
      construct->connect_sg(mux_decoder,out_pin,UNASSIGNED,
			    mux_sg,in_pin,UNASSIGNED,CTRL_NODE);
    }

  for (int loop=0;loop<sensy_sigs;loop++)
    {
      int in_pin=mux_decoder->find_hardpin(INPUT_PIN,decoder_times->query(loop));
      mux_decoder->replace_pintype(in_pin,INPUT_PIN_PHASE);
    }

  // Cleanup
/*
  delete decoder_times;
  for (int loop=0;loop<ctrl_sigs;loop++)
    delete decoder_eq[loop];
  delete []decoder_eq;
  */


  // Exempt these bit widths as they are set by the 
  // architecture model
  mux_sg->bw_exempt=1;

  // Mux connections are to be made on a first needed basis
  // as determined by the port timing data activation schedule
  for (int sg_loop=0;sg_loop<sg_count;sg_loop++)
    {
      // Determine information on the Source/Sink star
      ACSCGFPGACore* io_core=(ACSCGFPGACore *) io_stars->elem(sg_loop);
      
      if (DEBUG_IOADD)
	printf("\n\nConnect mux port %d to star id %d (%s)\n",
	       sg_loop,
	       io_core->acs_id,
	       io_core->comment_name());
      
      // Swap ALL the Source/Sink connections with the MUX switchable pins
      if (port_type==SOURCE)
	HWio_reconnect_src(mux_sg,
			   io_core,
			   OUTPUT_PIN_MUX_SWITCHABLE);
      else
	HWio_reconnect_snk(io_core,
			   mux_sg,
			   INPUT_PIN_MUX_SWITCHABLE);
    }
      
  // Hook up mux output to the ram path
  // FIX:The free_pintype may not be needed as it would trap as an
  //     OUTPUT_PIN during a search.  Ok for now...
  Pin* mux_pins=mux_sg->pins;
  int src_pin=mux_pins->free_pintype(muxresult_type);
  mux_pins->set_precision(src_pin,
			  io_port->data_size-1,
			  io_port->data_size,
			  LOCKED);
  construct->connect_sg(mux_sg,
			src_pin,UNASSIGNED,data_signal,
			fpga_elem->ext_signals,
			data_signal,UNASSIGNED,data_signal,
			EXT_NODE);
}



void ACSCGFPGATarget::HWio_reconnect_src(ACSCGFPGACore* mux_core,
					 ACSCGFPGACore* src_core,
					 const int mux_type)
{
  // Reassign all connections from the Source star to the MUX
  // ASSUMPTION:Source stars only have a single output pin, therefore connection 0 is valid
  //            However ioports are connection 1
  // FIX: Break up this dependency!!!
  Pin* src_pins=src_core->pins;
  Connectivity* src_connect0=NULL;
  int src_pin=0;
  if (src_core->acs_type==SOURCE)
    src_connect0=src_pins->query_connection(0);
  else
    {
      src_connect0=src_pins->query_connection(1);
      src_pin=1;
    }

  // Since the Source's connections are to be disconnected, we need only to
  // retrieve the acs_ids of the cores that it is connected to
  if (DEBUG_IOADD)
    printf("HWio_reconnect_src::Source sg %s has %d node connections to be remapped\n",
	   src_core->comment_name(),
	   src_connect0->node_count);

  ACSIntArray* src_connections=new ACSIntArray;
  for (int con_loop=0;con_loop<src_connect0->node_count;con_loop++)
    src_connections->add(src_connect0->query_acsid(con_loop));

  if (src_core->acs_type != IOPORT)
    src_pins->disconnect_allpins();

  // Determine the MUX pin in question
  // NOTE:Only each MUX pin only represents one source, all connections
  //      use the same pin on the MUX
  Pin* mux_pins=mux_core->pins;
  int mux_pin=mux_pins->free_pintype(mux_type);
  if (DEBUG_IOADD)
    printf("HWio_reconnect_src::free pin=%d\n",mux_pin);

  for (int con_loop=0;con_loop<src_connections->population();con_loop++)
    {
      int alg_id=src_connections->query(con_loop);
      ACSCGFPGACore* alg_core=HWfind_star(alg_id);
      if (DEBUG_IOADD)
	printf("HWio_reconnect_src::Looking for connection to id %d, sg %s\n",
	       alg_id,
	       alg_core->comment_name());
      
      // Make new connection
      Pin* alg_pins=alg_core->pins;
      int alg_pin=alg_pins->match_acstype(src_core->acs_id,INPUT_PIN);
      int alg_node=alg_pins->match_acsnode(src_core->acs_id,alg_pin);

      if (DEBUG_IOADD)
	printf("HWio_reconnect_src::alg_pin=%d, alg_node=%d\n",
	       alg_pin,
	       alg_node);

      // Break the old
      src_pins->disconnect_all_opins();

      // Connect the MUX to the algorithm
      construct->connect_sg(mux_core,mux_pin,UNASSIGNED,
			    alg_core,alg_pin,alg_node,
			    DATA_NODE);

      // Update precisions
      int major_bit=alg_pins->query_majorbit(alg_pin);
      int bitlength=alg_pins->query_bitlen(alg_pin);
      mux_pins->set_precision(mux_pin,major_bit,bitlength,LOCKED);

      if (DEBUG_IOADD)
	{
	  printf("HWio_reconnect_src::Source %s drove %s (pin %d)\n",
		 src_core->comment_name(),
		 alg_core->comment_name(),
		 alg_pin);
	  printf("HWio_reconnect_src::%s is now driven by %s\n",
		 alg_core->comment_name(),
		 mux_core->comment_name());
	}
    }    

  // Cleanup
  delete src_connections;
}

void ACSCGFPGATarget::HWio_reconnect_snk(ACSCGFPGACore* snk_core,
					 ACSCGFPGACore* mux_core,
					 const int mux_type)
{
  // Reassign all connections from the Sink star to the MUX
  // ASSUMPTION:Sink stars only have a single input pin

  Pin* snk_pins=snk_core->pins;
  Connectivity* snk_connect0=snk_pins->query_connection(0);
  if (DEBUG_IOADD)
    printf("HWio_reconnect_snk::sg %s has %d node connections to be remapped\n",
	   snk_core->comment_name(),
	   snk_connect0->node_count);
  
  int alg_id=snk_connect0->query_acsid(0);
  ACSCGFPGACore* alg_core=HWfind_star(alg_id);
  if (DEBUG_IOADD)
    printf("HWio_reconnect_snk::Looking for connection to id %d, sg %s\n",
	   alg_id,
	   alg_core->comment_name());
  
  // Make new connection
  Pin* alg_pins=alg_core->pins;
  int alg_pin=alg_pins->match_acstype(snk_core->acs_id,OUTPUT_PIN);
  int alg_node=alg_pins->match_acsnode(snk_core->acs_id,alg_pin);
  if (DEBUG_IOADD)
    printf("HWio_reconnect_snk::alg_pin=%d, alg_node=%d\n",
	   alg_pin,
	   alg_node);

  // Break the old
  snk_pins->disconnect_all_ipins();
  
  // Determine the MUX pin that was connected and transfer BW
  // Only do this once, since there is only one mux_pin in
  // question here
  Pin* mux_pins=mux_core->pins;
  int mux_pin=mux_pins->free_pintype(mux_type);
  if (DEBUG_IOADD)
    printf("HWio_reconnect_snk::free pin=%d\n",mux_pin);

  construct->connect_sg(alg_core,alg_pin,alg_node,
			mux_core,mux_pin,UNASSIGNED,
			DATA_NODE);

  // Update precisions
  int major_bit=alg_pins->query_majorbit(alg_pin);
  int bitlength=alg_pins->query_bitlen(alg_pin);
  mux_pins->set_precision(mux_pin,major_bit,bitlength,LOCKED);

  if (DEBUG_IOADD)
    {
      printf("HWio_reconnect_snk::Snk %s was drivin by %s (pin %d)\n",
	     snk_core->comment_name(),
	     alg_core->comment_name(),
	     alg_pin);
      printf("HWio_reconnect_snk::%s is now drives by %s\n",
	     alg_core->comment_name(),
	     mux_core->comment_name());
    }    
}



////////////////////////////////////////////////////////////////
// OBSOLETE!!!
////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_reconnect(ACSCGFPGACore* alg_core,
					 ACSCGFPGACore* new_core,
					 Pin* new_pins,
					 int alg_type,
					 int dest_type,
					 int node_type,
					 Port* io_port)
{
//  printf("HWio_add_reconnect invoked....it is OBSOLETE!!!\n");

  // Reassign all connections from the Source/Sink star to the MUX
  // ASSUMPTION:Source stars only have single output ports
  // fpga_core is only a pointer to the dummy source star, need 
  // to connect to the next star

  Pin* alg_pins=alg_core->pins;

  int io_pin=-1;
  if (alg_type==INPUT_PIN)
    io_pin=alg_core->find_hardpin(OUTPUT_PIN);
  else
    io_pin=alg_core->find_hardpin(INPUT_PIN);
  Connectivity* alg_connect0=alg_pins->query_connection(io_pin);
  if (DEBUG_IOADD)
    printf("sg %s has %d node connections to be remapped\n",
	   alg_core->comment_name(),
	   alg_connect0->node_count);

  for (int con_loop=0;con_loop<alg_connect0->node_count;con_loop++)
    {
      int dest_id=alg_connect0->query_acsid(con_loop);
      ACSCGFPGACore* data_core=HWfind_star(dest_id);
      if (DEBUG_IOADD)
	printf("Looking for connection to id %d, star %s\n",
	       dest_id,
	       data_core->comment_name());
      
      // Make new connection
      Pin* data_pins=data_core->pins;
      int data_pin=-1;
      int data_node=-1;
      if (new_core!=NULL)
	{
	  data_pin=data_pins->match_acstype(alg_core->acs_id,INPUT_PIN);
	  data_node=data_pins->match_acsnode(alg_core->acs_id,data_pin);
	}
      else
	{
	  data_pin=data_pins->match_acstype(alg_core->acs_id,alg_type);
	  data_node=data_pins->match_acsnode(alg_core->acs_id,data_pin);
	}
      if (DEBUG_IOADD)
	printf("data_pin=%d, data_node=%d\n",
	       data_pin,
	       data_node);

      // Determine the MUX pin that was connected and transfer BW
      // Only do this once, since there is only one mux_pin in
      // question here
      if (new_core!=NULL)
	{
	  // FIX:This pin search should be more specific
	  int new_pin=new_pins->retrieve_type(alg_type);
	  construct->connect_sg(data_core,data_pin,data_node,dest_type,
				new_core,new_pin,UNASSIGNED,dest_type,
				node_type);
	}
      else
	{
	  // Hooking up directly to a memory port
	  if (DEBUG_IOADD)
	    printf("%s, pin %d, node %d connect to node_type=%d dest_type=%d\n",
		   data_core->comment_name(),data_pin,data_node,
		   node_type,dest_type);
	  construct->connect_sg(data_core,data_pin,data_node,dest_type,
				new_pins,dest_type,UNASSIGNED,dest_type,
				node_type);
	}
      
      // FIX: This only prevents the external signals from being re-precisioned
      //      This method should be revisited from a global perspective
      if ((node_type==DATA_NODE) && (new_core==NULL))
	{
	  int new_pin=new_pins->match_acstype(data_core->acs_id,alg_type);
	  new_pins->set_precision(new_pin,
				  io_port->data_size-1,
				  io_port->data_size,
				  LOCKED);
	}
      
      if (DEBUG_IOADD)
	{
	  printf("Source %s drove %s (node %d)\n",
		 alg_core->comment_name(),
		 data_core->comment_name(),
		 data_pin);
	  if (node_type == DATA_NODE)
	    printf("%s is now driven by %s\n",
		   data_core->comment_name(),
		   new_core->comment_name());
	  else
	    printf("%s is now connected to external pin\n",
		   data_core->comment_name());
	}
    }    
  
  // Remove old connection on the Source/Sink stars
  if (alg_type==INPUT_PIN)
    alg_pins->disconnect_all_opins();
  else
    alg_pins->disconnect_all_ipins();

  // If a LUT then reassociate the input connection
  if (alg_core->acs_type==SOURCE_LUT)
    HWio_remaplut(alg_core);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Remap the input to the lookup table.  Break the connection to the dummy lut core, and provide resolver
// clues for hookup to the address generators
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_remaplut(ACSCGFPGACore* lut_core)
{
  Pin* lut_pins=lut_core->pins;
  int io_pin=lut_core->find_hardpin(INPUT_PIN);
  Connectivity* lut_connect0=lut_pins->query_connection(io_pin);
  if (DEBUG_IOADD)
    printf("lut %s : remapping input\n",lut_core->comment_name());

  // Find the source of the LUT and ensure that the correct address bits are preserved
  int src_id=lut_connect0->query_acsid(0);
  ACSCGFPGACore* src_core=HWfind_star(src_id);
  src_core->bitslice_strategy=PRESERVE_LSB;
  if (DEBUG_IOADD)
    printf("Looking for connection to id %d, star %s\n",
	   src_id,
	   src_core->comment_name());
      
  // Alter the pintype for the resolver, assign the priority to the lut core acs id for uniqueness
  Pin* src_pins=src_core->pins;
  int src_pin=src_pins->match_acstype(lut_core->acs_id,OUTPUT_PIN);

  // These connections are now irrelevant
  lut_pins->disconnect_all_ipins();
  src_pins->disconnect_all_opins();

  // Now remap
  src_pins->set_pintype(src_pin,OUTPUT_PIN_LUTINDEX);
  src_pins->set_pinpriority(src_pin,lut_core->acs_id);

}

///////////////////////////////////////
// Add a word counter (fixed countdown)
// OBSOLETE!
///////////////////////////////////////
void ACSCGFPGATarget::HWio_add_wordcounter(Fpga* fpga_elem,
					   CoreList*& wc_consts,
					   const int count_value,
					   ACSCGFPGACore*& wc_mux, const char* mux_comment,
					   const int mux_ctrl_type,
					   ACSCGFPGACore*& wc_add, const char* add_comment,
					   const int count_type, const int add_ctrl_type)
{
  // Generate the initial count value as HW constant 
  ACSCGFPGACore* wcconst_core;
  wcconst_core=construct->add_const(-count_value,SIGNED,smart_generators);
  wcconst_core->add_comment("Initial countdown value for word counter");
  wc_consts->append(wcconst_core);

  // Obtain bit size from the word count constants
  Constants* consts=wcconst_core->sg_constants;
  int wcbits=consts->query_bitsize(0,-1);
  if (DEBUG_IOADD)
    printf("IOADD:vector_length=%d, wcbits=%d\n",vector_length,wcbits);

  // Now genereate the increment value as HW constants
  wcconst_core=construct->add_const((int) 1,SIGNED,smart_generators);
  wcconst_core->add_comment("Countup value of one for word counter");
  wc_consts->append(wcconst_core);
  
  // Adjust bitwidths on constants
  construct->balance_bw(wc_consts);

  // Add MUXs in order to route the constants to an adder
  wc_mux=construct->add_mux(2,2,SIGNED,smart_generators);
  wc_mux->add_comment(mux_comment);
  wc_mux->bitslice_strategy=PRESERVE_LSB;
  Pin* wcmux_pins=wc_mux->pins;
  int wcmux_ctrlpin=wcmux_pins->retrieve_type(INPUT_PIN_CTRL,0);
  wcmux_pins->set_pintype(wcmux_ctrlpin,mux_ctrl_type);
  if (DEBUG_IOADD)
    printf("Added word counter mux\n");
  
  // Add the address generating adder and inform the memory port
  wc_add=construct->add_adder(SIGNED,smart_generators);
  wc_add->bitslice_strategy=PRESERVE_LSB;
  wc_add->add_comment(add_comment);

  // Connect constants to the MUX and the MUX to the adder
  construct->connect_sg(wc_consts,
			NULL,NULL,OUTPUT_PIN,
			wc_mux,
			UNASSIGNED,UNASSIGNED,INPUT_PIN_MUX_SWITCHABLE,
			DATA_NODE);
  construct->connect_sg(wc_mux,
			UNASSIGNED,UNASSIGNED,OUTPUT_PIN_MUX_RESULT,
			wc_add,
			UNASSIGNED,UNASSIGNED,INPUT_PIN,
			DATA_NODE);
  
  // Connect the feedback loop on the adder
  construct->connect_sg(wc_add,UNASSIGNED,UNASSIGNED,
			wc_add,UNASSIGNED,UNASSIGNED,
			DATA_NODE);
  
  // Reassign data output to predefined word counter output type
  Pin* wc_pins=wc_add->pins;
  int port_id=wc_pins->retrieve_type(OUTPUT_PIN);
  wc_pins->set_pintype(port_id,count_type);
  
  // Fix output width to match total sequence count
  wc_pins->set_precision(port_id,wcbits,wcbits+1,LOCKED);
  wc_pins->set_alllocks();
  if (DEBUG_IOADD)
    printf("IO_ADD:Fixed word counter to %d bits\n",wcbits+1);
  
  // Reassign chip enable to predefined word counter output type
  port_id=wc_pins->retrieve_type(INPUT_PIN_CE);
  wc_pins->set_pintype(port_id,add_ctrl_type);
}


//////////////////////////////////////////////////////////////////////////
// Remove the connection to the io_core smart generator.
// Take the links and assign them to the true io port on the fpga's
// external pins
// FIX: The remapping procedure should be reduced to a method that handles
//      both input and output pins, rather than the explicit coding here
//////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWremap_ioport(ACSCGFPGACore* io_core,
				     CoreList* sg_list)
{
  if (DEBUG_IOREMAP)
    printf("\n\nRemapping IO core %s\n",io_core->comment_name());

  // Remap input pins
  // ASSUMPTION: Hopefully there will only be one
  Pin* io_pins=io_core->pins;
  ACSIntArray* ipins=io_pins->query_inodes();
  int ipin_count=ipins->population();
  int src_mbit=0;
  int src_bitlen=0;
  for (int ipin_loop=0;ipin_loop<ipin_count;ipin_loop++)
    {
      int ipin=ipins->query(ipin_loop);
      Connectivity* icons=io_pins->query_connection(ipin);

      // ASSUMPTION: Only one input node!!
      int acs_id=icons->query_acsid(0);
      ACSCGFPGACore* src_core=HWfind_star(acs_id);
      Pin* src_pins=src_core->pins;
      if (DEBUG_IOREMAP)
	printf("IO core is driven by %s\n",src_core->comment_name());

      // Determine what we know about the source's connection
      int src_pin=src_pins->match_acstype(io_core->acs_id,OUTPUT_PIN);
      int src_node=src_pins->match_acsnode(io_core->acs_id,src_pin);

      // Determine and set which device we are operating in
      // Since it is an input pin the acs_device field is correct
      Fpga* curr_fpga=arch->get_fpga_handle(src_core->acs_device);
      Pin* curr_fpga_pins=curr_fpga->ext_signals;
      ACSIntArray* ext_signal_ptrs=curr_fpga->ifpga_signal_id;
      construct->set_targetdevice(curr_fpga);
      
      // For now there is only one set of port pins
      // FIX:Generalize this please, dont rely on acs_outdevice as a route pointer!
      ACSIntArray* device_nums=io_core->sg_get_privaledge();
      int acs_outdevice=device_nums->query(1);
      int io_pin=ext_signal_ptrs->query(acs_outdevice);
      int io_node=curr_fpga_pins->match_acsnode(src_core->acs_id,io_pin);

      // Given a direction, reconfigure the external signal from bidirectional to
      // the appropriate data direction.
      // FIX: Will need to revisit if bidirectional MUX employed!!!
      Pin* fpga_ext_pins=curr_fpga->ext_signals;
      fpga_ext_pins->set_pintype(io_pin,OUTPUT_PIN);

      if (DEBUG_IOREMAP)
	printf("adding input pin port timing acs_device=%d, acs_outdevice=%d, output_time=%d\n",
	       src_core->acs_device,
	       acs_outdevice,
	       src_core->act_output);
      curr_fpga->add_timing(acs_outdevice,src_core->act_output,0);

      construct->connect_sg(src_core,src_pin,src_node,io_pin,
			    curr_fpga_pins,io_pin,io_node,io_pin,EXT_NODE);

      // Update the major bit precision
      // NOTE:External pin bitwidth is not changed as this must match the architecture descriptor
      //      correct bitwidth information will be realized via a buffer to the output
      src_mbit=src_pins->query_majorbit(src_pin);
      src_bitlen=src_pins->query_bitlen(src_pin);
      fpga_ext_pins->set_precision(io_pin,src_mbit,fpga_ext_pins->query_bitlen(io_pin),LOCKED);
    }

  // Remap output pins
  ACSIntArray* opins=io_pins->query_onodes();
  int opin_count=opins->population();
  for (int opin_loop=0;opin_loop<opin_count;opin_loop++)
    {
      int opin=opins->query(opin_loop);
      Connectivity* ocons=io_pins->query_connection(opin);

      // ASSUMPTION: Only one input node!!
      int acs_id=ocons->query_acsid(0);
      ACSCGFPGACore* dest_core=HWfind_star(acs_id);
      Pin* dest_pins=dest_core->pins;
      if (DEBUG_IOREMAP)
	printf("IO core drives %s\n",dest_core->comment_name());

      // Determine what we know about the source's connection
      int dest_pin=dest_pins->match_acstype(io_core->acs_id,INPUT_PIN);
      int dest_node=dest_pins->match_acsnode(io_core->acs_id,dest_pin);

      // Determine and set which device we are operating in
      // Since it is an input pin the acs_device field is correct
      Fpga* curr_fpga=arch->get_fpga_handle(dest_core->acs_device);
      Pin* curr_fpga_pins=curr_fpga->ext_signals;
      ACSIntArray* ext_signal_ptrs=curr_fpga->ifpga_signal_id;
      construct->set_targetdevice(curr_fpga);
      
      // For now there is only one set of port pins
      // FIX:Generalize this please, dont rely on acs_outdevice as a route pointer!
      ACSIntArray* device_nums=io_core->sg_get_privaledge();
      int acs_indevice=device_nums->query(0);
      int io_pin=ext_signal_ptrs->query(acs_indevice);
      int io_node=curr_fpga_pins->match_acsnode(io_core->acs_id,io_pin);
      
      // Given a direction, reconfigure the external signal from bidirectional to
      // the appropriate data direction.
      // FIX: Will need to revisit if bidirectional MUX employed!!!
      Pin* fpga_ext_pins=curr_fpga->ext_signals;
      fpga_ext_pins->set_pintype(io_pin,INPUT_PIN);

      if (DEBUG_IOREMAP)
	printf("adding output pin port timing acs_device=%d, acs_indevice=%d, output_time=%d\n",
	       dest_core->acs_device,
	       acs_indevice,
	       dest_core->act_output);
      curr_fpga->add_timing(acs_indevice,0,dest_core->act_output);
      
      // Update the major bit precision
      // Buffer any under-driven bitwidth connections
      int ext_bitlen=fpga_ext_pins->query_bitlen(io_pin);
      curr_fpga_pins->set_precision(io_pin,src_mbit,ext_bitlen,LOCKED);
      if (src_bitlen < ext_bitlen)
	{
	  int sign_convention=io_core->sign_convention;
	  int bitslice_strategy=io_core->bitslice_strategy;
	  ACSCGFPGACore* buffer_core=construct->add_buffer(sign_convention,sg_list);
	  Pin* buffer_pins=buffer_core->pins;
	  buffer_core->add_comment("Post inter-fpga buffer (output)");
	  buffer_core->bitslice_strategy=bitslice_strategy;
	  
	  if (DEBUG_PARTITION)
	    printf("Inserting post inter-fpga buffer %s to %s\n",
		   buffer_core->comment_name(),
		   dest_core->comment_name());
	  construct->connect_sg(curr_fpga_pins,io_pin,io_node,io_pin,
				buffer_core,UNASSIGNED,UNASSIGNED,io_pin,EXT_NODE);
	  construct->connect_sg(buffer_core,UNASSIGNED,UNASSIGNED,
				dest_core,dest_pin,dest_node,DATA_NODE);

	  // Update precisions
	  // FIX: Liberties taken on buffer core pins
	  buffer_pins->set_precision(1,src_mbit,src_bitlen,LOCKED);
	}
      else
	construct->connect_sg(curr_fpga_pins,io_pin,io_node,io_pin,
			      dest_core,dest_pin,dest_node,io_pin,EXT_NODE);
    }
  
  // Remove old connections on the ioport
  io_pins->disconnect_allpins();
}


