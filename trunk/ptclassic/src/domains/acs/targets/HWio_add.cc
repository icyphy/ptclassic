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
	  construct->set_targetdevice(fpga_elem,fpga_loop);
	  HWio_add_sequencer(fpga_elem,fpga_loop);
	}
    }

  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////////////
// Look for implicit fpga interconnects and make them explicit
//////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWsegment_alg(void)
{
  int sg_count=smart_generators->size();
  for (int sg_loop=0;sg_loop<sg_count;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sg_loop);
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
		  ACSIntArray* the_route=arch->route_solutions[0];
		  int prev_device=UNASSIGNED;
		  int curr_device=UNASSIGNED;
		  int next_device=UNASSIGNED;
//		  the_route->print("route solution");

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
		      construct->set_targetdevice(arch->get_fpga_handle(curr_device),
									curr_device);

		      // Preserve data type integrity across partitions
		      int sign_convention=fpga_core->sign_convention;
		      int bitslice_strategy=fpga_core->bitslice_strategy;

		      // Add an ioport
		      // FIX: Should be variable in terms of port type
		      ioport_core=construct->add_ioport(sign_convention,smart_generators);
		      ioport_core->bitslice_strategy=bitslice_strategy;
		      ioport_core->acs_outdevice=next_device;
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
			  construct->set_targetdevice(arch->get_fpga_handle(curr_device),
						      curr_device);
			  ACSCGFPGACore* buffer_core=construct->
			    add_buffer(sign_convention,smart_generators);
			  buffer_core->add_comment("Inter-fpga buffer (output)");
			  buffer_core->bitslice_strategy=bitslice_strategy;
			  
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
			}
		      if ((ports_needed > 1) && (rloop != ports_needed-1) 
			  || (dest_core->acs_type != BOTH))
			{
			  construct->set_targetdevice(arch->get_fpga_handle(next_device),next_device);
			  ACSCGFPGACore* buffer_core=construct->
			    add_buffer(sign_convention,smart_generators);
			  buffer_core->add_comment("Inter-fpga buffer (input)");
			  buffer_core->bitslice_strategy=bitslice_strategy;
			  
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
			}
		      else
			{
			  // Prepare for next insertion (if applicable)
			  base_core=ioport_core;
			}
		    }
		}
	    }
	}
    }
  
  // Return happy condition
  return(1);
}

int ACSCGFPGATarget::HWreturn_fpgadevice(ACSCGFPGACore* fpga_core)
{
  int fpga_device=UNASSIGNED;
  if (fpga_core->acs_type==BOTH)
    fpga_device=fpga_core->acs_device;
  else
    {
      // Need a handle to the fpga, not the memory
      // ASSUMPTION:All routes to this memory must go through controller fpga
      MemPort* mem_port=arch->get_memory_handle(fpga_core->acs_device);
      fpga_device=mem_port->controller_fpga;
    }
  return(fpga_device);
}


/////////////////////////////////////////////////////////
// Add all memory-related, support components in hardware
/////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWio_add()
{
  /////////////////////////////////////////////////////////////////
  // Examine each memory port
  // Any port that supports multiple outputs need muxing and piping
  /////////////////////////////////////////////////////////////////
  for (int mem_loop=0;mem_loop<arch->mem_count;mem_loop++)
    {
      if (DEBUG_IOADD)
	printf("HWio_add:Examining memory %d\n",mem_loop);

      MemPort* mem_port=arch->get_memory_handle(mem_loop);
      if (mem_port->portuse==MEMORY_USED)
	{
	  if (DEBUG_IOADD)
	    printf("HWio_Add:This memory is being used\n");

	  /////////////////////////////////
	  // Determine the controlling fpga
	  /////////////////////////////////
	  int controller_fpga=mem_port->controller_fpga;
	  Fpga* fpga_elem=arch->get_fpga_handle(controller_fpga);
	  construct->set_targetdevice(fpga_elem,controller_fpga);
	  Sequencer* sequencer=fpga_elem->sequencer;

	  /////////////////////////
	  // Determine memory usage
	  /////////////////////////
	  int src_sgs=(mem_port->source_stars)->size();
	  int snk_sgs=(mem_port->sink_stars)->size();
  
	  ////////////////////////
	  // Add address generator
	  ////////////////////////
	  HWio_add_addressgen(fpga_elem,controller_fpga,
			      mem_port,mem_loop);

	  ////////////////////////////////////////////
	  // Add data routers and connect to algorithm
	  ////////////////////////////////////////////
	  HWio_add_dataio(fpga_elem,controller_fpga,
			  mem_port,mem_loop);


	  ///////////////////////////////////
	  // Add source counter for sequencer
	  ///////////////////////////////////
	  if (src_sgs)
	    // The '1' factor is needed since the word counter includes the zero
	    HWio_add_wordcounter(fpga_elem,controller_fpga,
				 sequencer->src_consts,
				 vector_length-1,
				 sequencer->src_mux,"Source Counter MUX",INPUT_PIN_SRC_MUX,
				 sequencer->src_add,"Source Counter Feedback Adder",
				 OUTPUT_PIN_SRC_WC,
				 INPUT_PIN_SRC_CE);

	  /////////////////////////////////
	  // Add sink counter for sequencer
	  /////////////////////////////////
	  if (snk_sgs)
	    {
//	      printf("total_latency=%d\n",mem_port->total_latency);
	      // Need to align latency with ring counter, subtract 1 to allow for state transition
	      // Delay should hit zero, one cycle before Phase0 of the ring counter
	      int factor=mem_port->total_latency % seqlen;
	      int latency=0;
	      if (factor==0)
		latency=mem_port->total_latency;
	      else
		latency=mem_port->total_latency - (mem_port->total_latency % seqlen);
//	      printf("latency=%d, seqlen=%d\n",latency,seqlen);

	      // Add an initial delay counter
	      // The '2' factor is needed to accomodate the address generator setup time
	      HWio_add_wordcounter(fpga_elem,controller_fpga,
				   sequencer->delay_consts,
				   latency,
				   sequencer->delay_mux,"Delay Counter MUX",INPUT_PIN_DELAY_MUX,
				   sequencer->delay_add,"Delay Counter Feedback Adder",
				   OUTPUT_PIN_DELAY_WC,
				   INPUT_PIN_DELAY_CE);


	      // Add a counter for sinks
	      // The '1' factor is needed since the word counter includes the zero
	      HWio_add_wordcounter(fpga_elem,controller_fpga,
				   sequencer->snk_consts,
				   vector_length-1,
				   sequencer->snk_mux,"Sink Counter MUX",INPUT_PIN_SNK_MUX,
				   sequencer->snk_add,"Sink Counter Feedback Adder",
				   OUTPUT_PIN_SNK_WC,
				   INPUT_PIN_SNK_CE);
	    }
	}
    }

  //////////////////////////////////////////////////////////////////////////
  // Now examine all smart generators, in search of device partitions
  // If they do exist, then remap these smart generators to their respective
  // external node
  //////////////////////////////////////////////////////////////////////////
  for (int sg_loop=0;sg_loop<smart_generators->size();sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sg_loop);
      if (fpga_core->acs_type==IOPORT)
	HWremap_ioport(fpga_core);
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
void ACSCGFPGATarget::HWio_add_sequencer(Fpga* fpga_elem, const int fpga_id)
{
  fpga_elem->sequencer=new Sequencer;
  Sequencer* sequencer=fpga_elem->sequencer;
  sequencer->seq_sg=construct->add_sg("ACS","UniSeq","CGFPGA",
				      BOTH,UNSIGNED,smart_generators);
}


///////////////////////////////////////////////////////////////////////////////
// Add smart generators to function as an address generator for a given memory,
// that will be resident in a specific fpga
///////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_addressgen(Fpga* fpga_elem,
					  const int fpga_id,
					  MemPort* mem_port,
					  const int mem_id)
{
  if (DEBUG_IOADD)
    printf("Adding address generator for memory %d, in fpga %d\n",mem_id,fpga_id);

  	
  // Retrieve the starting addresses used by each star
  // on this memory port as well as their node activation times
  SequentialList* source_sgs=mem_port->source_stars;
  SequentialList* sink_sgs=mem_port->sink_stars;
  int src_sgs=source_sgs->size();
  int snk_sgs=sink_sgs->size();
  mem_port->port_timing=new Port_Timing(mem_port->read_skew,
					mem_port->write_skew);

  if (DEBUG_IOADD)
    printf("This memory has %d sources and %d sinks associated\n",
	   src_sgs,snk_sgs);

  Port_Timing* port_timing=mem_port->port_timing;

  // Identify all source elements and source execution times
  for (int sg_loop=1;sg_loop<=src_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) source_sgs->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;
      port_timing->add_read(fpga_core->address_start,
			    fpga_core->address_step,
			    fpga_core->act_input,
			    fpga_core->act_output,
			    fpga_pins->query_majorbit(0),
			    fpga_pins->query_bitlen(0),
			    sg_loop);
      port_timing->add_wordcount(vector_length);
      
      if (DEBUG_IOADD)
	printf("added src for addr %d, time %d\n",
	       fpga_core->address_start,
	       fpga_core->act_input);
    }

  // Identify all sink elements and sink execution times
  for (int sg_loop=1;sg_loop<=snk_sgs;sg_loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore *) sink_sgs->elem(sg_loop);
      Pin* fpga_pins=fpga_core->pins;
      
      // Adjust the node activation time for sink stars to 
      // reflect architecture delays
      int new_nodeact=fpga_core->act_output;
      int new_address=fpga_core->address_start;

      // If memory is shared as a source, then the source will be
      // activated before any sinks are scheduled.  Need to adjust
      // the address of the sink, so that the proper address will be
      // determined once the memory is ready to be used as a sink
      if (src_sgs)
	{
	  float fsetback=floor(mem_port->total_latency/seqlen);
	  int address_setback=(int) fsetback;
	  /*
	  printf("latency=%d, seqlen=%d, setback=%d, fsetback=%f\n",
		 mem_port->total_latency,
		 seqlen,
		 address_setback,
		 fsetback);
	  printf("Since there are sources as well as sinks, sink address %d will be moved to %d\n",new_address,new_address-address_setback);
	  */
	  new_address-=address_setback;
	}

      if (new_nodeact >= seqlen)
	{
	  // If the write goes past the sequence length, then "jimmy" the 
	  // update sequence
	  // ASSUMPTION:Inputs cannot be beyond the end of the sequence 
	  //            length, only outputs
//	  int skipamt=(int) floor(new_nodeact/seqlen);
	  new_nodeact=new_nodeact % seqlen;
//	  new_address-=skipamt;
	}
      port_timing->add_write(new_address,
			     fpga_core->address_step,
			     UNASSIGNED,
			     new_nodeact,
			     fpga_pins->query_majorbit(0),
			     fpga_pins->query_bitlen(0),
			     sg_loop);
      port_timing->add_wordcount(vector_length);
      
      if (DEBUG_IOADD)
	printf("added sink for addr %d, time %d\n",
	       new_address,
	       new_nodeact);
    }
  
  // Sort the addresses based on execution time and calculate the 
  // update sequence
  port_timing->sort_times();
  port_timing->calc_updseq();

  // Keep track of these constant stars
  ACSCGFPGACore* const_core=NULL;
  
  // Convert the update sequence to hardware constants
  int const_count=src_sgs+snk_sgs+1;
  
  for (int sg_loop=0;sg_loop<const_count;sg_loop++)
    {
      if (sg_loop==0)
	{
	  ACSIntArray* address_start=port_timing->address_start;
	  int address=address_start->query(0);
	  const_core=construct->add_const(address,
					  SIGNED,
					  smart_generators);
	  const_core->add_comment("Starting address");
	}
      else
	{
	  ACSIntArray* update_sequence=port_timing->update_sequence;
	  int address=update_sequence->query(sg_loop-1);
	  const_core=construct->add_const(address,
					  SIGNED,
					  smart_generators);
	  const_core->add_comment("Address delta",sg_loop);
	}
      SequentialList* const_stars=mem_port->const_stars;
      const_stars->append((Pointer) const_core);
    }

  // Adjust bitwidths on constants
  HWbalance_bw(mem_port->const_stars);
  
  // Add MUXs in order to route the constants to an adder
  if (const_count > 1)
    {
      mem_port->addrmux_star=construct->add_mux(const_count,
				      const_count,
				      UNSIGNED,
				      smart_generators);
      ACSCGFPGACore* addrmux_star=mem_port->addrmux_star;
      addrmux_star->add_comment("Address generator MUX");
      addrmux_star->bitslice_strategy=PRESERVE_LSB;
      
      Pin* addrmux_pins=addrmux_star->pins;
      int mux_ctrl=addrmux_pins->retrieve_type(INPUT_PIN_CTRL,0);
      addrmux_pins->set_pintype(mux_ctrl,INPUT_PIN_ADDRMUX);
      
      if (DEBUG_IOADD)
	printf("Added address generator mux %s for port %d\n",
	       (mem_port->addrmux_star)->name(),
	       mem_id);
    }

  // Add the address generating adder and inform the memory port
  mem_port->addrgen_star=construct->add_adder(SIGNED,
					      smart_generators);
  ACSCGFPGACore* addrgen_star=mem_port->addrgen_star;
  addrgen_star->bitslice_strategy=PRESERVE_LSB;
  addrgen_star->add_comment("Address generator, feedback adder");

  // Fix the output size of the adder in order to represent the total span
  // of addresses needed
  // FIX:Some liberty taken on the pin numbers for the adder!!!
  int max_address=port_timing->max_address(vector_length);
  int maxaddr_bits=HWbit_sizer(max_address);
  Pin* addrgen_pins=addrgen_star->pins;
  addrgen_pins->set_precision(1,maxaddr_bits-1,maxaddr_bits,LOCKED);
  addrgen_pins->set_precision(2,maxaddr_bits-1,maxaddr_bits,LOCKED);
//  printf("address generator should span %d bits\n",maxaddr_bits);

  
  if (DEBUG_IOADD)
    printf("Added address generator adder %s for port %d\n",
	   addrgen_star->name(),
	   mem_id);
  
  // Connect constants to the MUX and the MUX to the adder
  if (const_count > 1)
    {
      construct->connect_sg(mem_port->const_stars,
			    NULL,NULL,OUTPUT_PIN,
			    mem_port->addrmux_star,
			    UNASSIGNED,UNASSIGNED,
			    INPUT_PIN_MUX_SWITCHABLE,
			    DATA_NODE);
      construct->connect_sg(mem_port->addrmux_star,
			    UNASSIGNED,UNASSIGNED,OUTPUT_PIN_MUX_RESULT,
			    mem_port->addrgen_star,
			    UNASSIGNED,UNASSIGNED,INPUT_PIN,
			    DATA_NODE);
    }
  else
    construct->connect_sg(mem_port->const_stars,NULL,NULL,
			  mem_port->addrgen_star,
			  UNASSIGNED,UNASSIGNED,
			  DATA_NODE);
  
  // Add a buffer for adder feedback inform the memory port
  mem_port->addrbuf_star=construct->add_buffer(UNSIGNED,
					       smart_generators);
  ACSCGFPGACore* addrbuf_star=mem_port->addrbuf_star;
  addrbuf_star->add_comment("Address generator feedback buffer");
  
  // Connect the output of the adder to the input of the buffer
  construct->connect_sg(mem_port->addrgen_star,
			UNASSIGNED,UNASSIGNED,OUTPUT_PIN,
			mem_port->addrbuf_star,
			UNASSIGNED,UNASSIGNED,INPUT_PIN,
			DATA_NODE);
	
  // Connect the feedback loop for the adder
  construct->connect_sg(mem_port->addrgen_star,
			UNASSIGNED,UNASSIGNED,OUTPUT_PIN,
			mem_port->addrgen_star,
			UNASSIGNED,UNASSIGNED,INPUT_PIN,
			DATA_NODE);

  // Connect the output of the adder (via a buffer) to the address 
  // generator
  construct->connect_sg(mem_port->addrbuf_star,
			UNASSIGNED,UNASSIGNED,
			(fpga_elem->addr_signal_id)->get(mem_id),
			fpga_elem->ext_signals,
		        (fpga_elem->addr_signal_id)->get(mem_id),UNASSIGNED,
			(fpga_elem->addr_signal_id)->get(mem_id),
			EXT_NODE);
  
  // Set buffer output to match address bits
  int addr_size=mem_port->addr_size;
  Pin* addrbuf_pins=(mem_port->addrbuf_star)->pins;
  addrbuf_pins->set_precision(1,addr_size-1,addr_size,LOCKED);

  
  // Remap pin types for linkage to sequencer
  Pin* addr_pins=(mem_port->addrgen_star)->pins;
  int address_clear=addr_pins->retrieve_type(INPUT_PIN_CLR);
  addr_pins->set_pintype(address_clear,INPUT_PIN_ADDRCLR);
  int address_enable=addr_pins->retrieve_type(INPUT_PIN_CE);
  addr_pins->set_pintype(address_enable,INPUT_PIN_ADDRCE);
}


// Works for Constants classes with only a single constant!
// FIX:Need to trap, or better yet generalize
void ACSCGFPGATarget::HWbalance_bw(SequentialList* smart_generators)
{
  // Determine max bitwidth of the group
  int max_bw=-1;
  for (int sg_loop=1;sg_loop<=smart_generators->size();sg_loop++)
    {
      ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) 
	smart_generators->elem(sg_loop);
      int curr_bw=(smart_generator->sg_constants)->query_bitsize(0);
      if (curr_bw > max_bw)
	max_bw=curr_bw;

      if (DEBUG_BW)
	printf("HWbalance_bw, sg %s has bw of %d\n",
	       smart_generator->comment_name(),
	       curr_bw);
    }

  // Now set all precisions to match bit widths
  for (int sg_loop=1;sg_loop<=smart_generators->size();sg_loop++)
    {
      ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) 
	smart_generators->elem(sg_loop);
      (smart_generator->pins)->set_precision(0,max_bw-1,max_bw,LOCKED);
    }
}


///////////////////////////////////////////////////////////////
// Make the connections from a (shared) memory to the algorithm
///////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_dataio(Fpga* fpga_elem,
				      const int controller_fpga,
				      MemPort* mem_port,
				      const int mem_id)
{
  // Determine source and sink pointers
  SequentialList* source_sgs=mem_port->source_stars;
  SequentialList* sink_sgs=mem_port->sink_stars;
  int src_count=source_sgs->size();
  int snk_count=sink_sgs->size();

  int sources_reconnected=FALSE;
  int sinks_reconnected=FALSE;
	  
  // Need to trap for bidirectional or split memory signals
  if ((snk_count > 1) || (src_count > 1) || (snk_count+src_count > 1))
    {
      if (DEBUG_IOADD)
	printf("Memory %d has multiple connections (%d src, %d snk)\n",
	       mem_id,
	       src_count,
	       snk_count);

      // Does this architecture provide a bidirectional memory model
      // or separate data baths?
      if (mem_port->bidir_data)
	{
	  if (snk_count+src_count > 1)
	    {
	      HWio_add_bidirmem(fpga_elem,controller_fpga,
				mem_port,mem_id);
	      sources_reconnected=TRUE;
	      sinks_reconnected=TRUE;
	    }
	}
      else
	{
	  if (src_count > 1)
	    {
	      HWio_add_dualmem(fpga_elem,controller_fpga,
			       mem_port,mem_id,
			       SOURCE);
	      sources_reconnected=TRUE;
	    }
	  if (snk_count > 1)
	    {
	      HWio_add_dualmem(fpga_elem,controller_fpga,
			       mem_port,mem_id,
			       SINK);
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
	  ACSCGFPGACore* alg_core=(ACSCGFPGACore *) source_sgs->elem(1);
	  type=INPUT_PIN;
	  data_sigid=(fpga_elem->datain_signal_id)->get(mem_id);
	  HWio_add_reconnect(alg_core,NULL,fpga_elem->ext_signals,
			     type,data_sigid,EXT_NODE,
			     mem_port);
	}
      if ((!sinks_reconnected) && (snk_count != 0))
	{
	  ACSCGFPGACore* alg_core=(ACSCGFPGACore *) sink_sgs->elem(1);
	  type=OUTPUT_PIN;
	  data_sigid=(fpga_elem->dataout_signal_id)->get(mem_id);
	  HWio_add_reconnect(alg_core,NULL,
			     fpga_elem->ext_signals,
			     type,data_sigid,EXT_NODE,
			     mem_port);
	}
    }
}


//////////////////////////////////////////////////////////
// Hook up the data paths to a single bidirectional memory
//////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_bidirmem(Fpga* fpga_elem, const int fpga_id, 
					MemPort* mem_port, const int mem_id)
{
  SequentialList* source_stars=mem_port->source_stars;
  SequentialList* sink_stars=mem_port->sink_stars;
  int src_stars=source_stars->size();
  int snk_stars=sink_stars->size();

  // Hook up sourcing smart generators to this MUX
  ACSCGFPGACore* mux_sg;
  mux_sg=mem_port->dataimux_star=construct->add_mux(src_stars+snk_stars,
						    snk_stars,
						    SIGNED,
						    smart_generators);
  mux_sg->add_comment("Bidirectional MUX for memory port",mem_id);
  mux_sg->bitslice_strategy=PRESERVE_LSB;

  // Exempt these bit widths as they are set by the architecture model
  mux_sg->bw_exempt=1;

  for (int sg_loop=1;sg_loop<=src_stars+snk_stars;sg_loop++)
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
			 mem_port);
      
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
				    mem_port->data_size-1,
				    mem_port->data_size,
				    LOCKED);
	    
      construct->connect_sg(mux_sg,
			    src_pin,UNASSIGNED,
			    (fpga_elem->datain_signal_id)->get(mem_id),
			    fpga_elem->ext_signals,
			    (fpga_elem->datain_signal_id)->get(mem_id),UNASSIGNED,
			    (fpga_elem->datain_signal_id)->get(mem_id),
			    EXT_NODE);
    }

}


/////////////////////////////////////////////////
// Hook up the data paths to a dual ported memory
/////////////////////////////////////////////////
void ACSCGFPGATarget::HWio_add_dualmem(Fpga* fpga_elem, 
				       const int fpga_id,
				       MemPort* mem_port,
				       const int mem_id,
				       const int port_type)
{
  SequentialList* io_stars=NULL;

  int sg_count=0;
  int mempin_type;
  int muxswitchable_type;
  int muxresult_type;
  int data_signal;
  ACSCGFPGACore* mux_sg=NULL;
  if (port_type==SOURCE)
    {
      io_stars=mem_port->source_stars;
      sg_count=io_stars->size();
      mem_port->dataimux_star=mux_sg=construct->add_mux(sg_count,
							0,
							SIGNED,
							smart_generators);
      mux_sg->add_comment("Input MUX from memory",mem_id);
      mux_sg->bitslice_strategy=PRESERVE_LSB;
      mempin_type=INPUT_PIN_MEMORY;
      muxswitchable_type=OUTPUT_PIN_MUX_SWITCHABLE;
      muxresult_type=INPUT_PIN_MUX_RESULT;
      data_signal=(fpga_elem->datain_signal_id)->get(mem_id);
    }
  else
    {
      io_stars=mem_port->sink_stars;
      sg_count=io_stars->size();
      mem_port->dataomux_star=mux_sg=construct->add_mux(sg_count,
							sg_count,
							SIGNED,
							smart_generators);
      mux_sg->add_comment("Output MUX from memory",mem_id);
      mux_sg->bitslice_strategy=PRESERVE_LSB;
      mempin_type=OUTPUT_PIN_MEMORY;
      muxswitchable_type=INPUT_PIN_MUX_SWITCHABLE;
      muxresult_type=OUTPUT_PIN_MUX_RESULT;
      data_signal=(fpga_elem->dataout_signal_id)->get(mem_id);
    }

  // Exempt these bit widths as they are set by the 
  // architecture model
  mux_sg->bw_exempt=1;


  // Mux connections are to be made on a first needed basis
  // as determined by the port timing data activation schedule
  for (int sg_loop=1;sg_loop<=sg_count;sg_loop++)
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
			   io_core);
      else
	HWio_reconnect_snk(io_core,
			   mux_sg);
    }
      
  // Hook up mux output to the ram path
  // FIX:The free_pintype may not be needed as it would trap as an
  //     OUTPUT_PIN during a search.  Ok for now...
  Pin* mux_pins=mux_sg->pins;
  int src_pin=mux_pins->free_pintype(muxresult_type);
  mux_pins->set_precision(src_pin,
			  mem_port->data_size-1,
			  mem_port->data_size,
			  LOCKED);
  construct->connect_sg(mux_sg,
			src_pin,UNASSIGNED,data_signal,
			fpga_elem->ext_signals,
			data_signal,UNASSIGNED,data_signal,
			EXT_NODE);
}



void ACSCGFPGATarget::HWio_reconnect_src(ACSCGFPGACore* mux_core,
					 ACSCGFPGACore* src_core)
{
  // Reassign all connections from the Source star to the MUX
  // ASSUMPTION:Source stars only have a single output pin

  Pin* src_pins=src_core->pins;
  Connectivity* src_connect0=src_pins->query_connection(0);
  if (DEBUG_IOADD)
    printf("source sg %s has %d node connections to be remapped\n",
	   src_core->comment_name(),
	   src_connect0->node_count);

  // Determine the MUX pin in question
  // NOTE:Only each MUX pin only represents one source, all connections
  //      use the same pin on the MUX
  Pin* mux_pins=mux_core->pins;
  int mux_pin=mux_pins->free_pintype(OUTPUT_PIN_MUX_SWITCHABLE);

  for (int con_loop=0;con_loop<src_connect0->node_count;con_loop++)
    {
      int alg_id=src_connect0->query_acsid(con_loop);
      ACSCGFPGACore* alg_core=HWfind_star(alg_id);
      if (DEBUG_IOADD)
	printf("Looking for connection to id %d, sg %s\n",
	       alg_id,
	       alg_core->comment_name());
      
      // Make new connection
      Pin* alg_pins=alg_core->pins;
      int alg_pin=alg_pins->match_acstype(src_core->acs_id,INPUT_PIN);
      int alg_node=alg_pins->match_acsnode(src_core->acs_id,alg_pin);

      if (DEBUG_IOADD)
	printf("alg_pin=%d, alg_node=%d\n",
	       alg_pin,
	       alg_node);

      // Connect the MUX to the algorithm
      construct->connect_sg(mux_core,mux_pin,UNASSIGNED,
			    alg_core,alg_pin,alg_node,
			    DATA_NODE);

      if (DEBUG_IOADD)
	{
	  printf("Source %s drove %s (pin %d)\n",
		 src_core->comment_name(),
		 alg_core->comment_name(),
		 alg_pin);
	  printf("%s is now driven by %s\n",
		 alg_core->comment_name(),
		 mux_core->comment_name());
	}
    }    
  // Remove old connection on the Source/Sink stars
  src_pins->disconnect_allpins();
}

void ACSCGFPGATarget::HWio_reconnect_snk(ACSCGFPGACore* snk_core,
					 ACSCGFPGACore* mux_core)
{
  // Reassign all connections from the Sink star to the MUX
  // ASSUMPTION:Sink stars only have a single input pin

  Pin* snk_pins=snk_core->pins;
  Connectivity* snk_connect0=snk_pins->query_connection(0);
  if (DEBUG_IOADD)
    printf("sg %s has %d node connections to be remapped\n",
	   snk_core->comment_name(),
	   snk_connect0->node_count);
  
  int alg_id=snk_connect0->query_acsid(0);
  ACSCGFPGACore* alg_core=HWfind_star(alg_id);
  if (DEBUG_IOADD)
    printf("Looking for connection to id %d, sg %s\n",
	   alg_id,
	   alg_core->comment_name());
  
  // Make new connection
  Pin* alg_pins=alg_core->pins;
  int alg_pin=alg_pins->match_acstype(snk_core->acs_id,OUTPUT_PIN);
  int alg_node=alg_pins->match_acsnode(snk_core->acs_id,alg_pin);
  if (DEBUG_IOADD)
    printf("alg_pin=%d, alg_node=%d\n",
	   alg_pin,
	   alg_node);
  
  // Determine the MUX pin that was connected and transfer BW
  // Only do this once, since there is only one mux_pin in
  // question here
  Pin* mux_pins=mux_core->pins;
  int mux_pin=mux_pins->free_pintype(INPUT_PIN_MUX_SWITCHABLE);
  construct->connect_sg(alg_core,alg_pin,alg_node,
			mux_core,mux_pin,UNASSIGNED,
			DATA_NODE);
  if (DEBUG_IOADD)
    {
      printf("Snk %s was drivin by %s (pin %d)\n",
	     snk_core->comment_name(),
	     alg_core->comment_name(),
	     alg_pin);
      printf("%s is now drives by %s\n",
	     alg_core->comment_name(),
	     mux_core->comment_name());
    }    
  // Remove old connection on the Source/Sink stars
  snk_pins->disconnect_allpins();
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
					 MemPort* mem_port)
{
//  printf("HWio_add_reconnect invoked....it is OBSOLETE!!!\n");

  // Reassign all connections from the Source/Sink star to the MUX
  // ASSUMPTION:Source stars only have single output ports
  // fpga_core is only a pointer to the dummy source star, need 
  // to connect to the next star

  Pin* alg_pins=alg_core->pins;

  Connectivity* alg_connect0=alg_pins->query_connection(0);
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
				  mem_port->data_size-1,
				  mem_port->data_size,
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
  alg_pins->disconnect_allpins();
}

///////////////////////////////////////
// Add a word counter (fixed countdown)
///////////////////////////////////////
void ACSCGFPGATarget::HWio_add_wordcounter(Fpga* fpga_elem, const int fpga_id,
					   SequentialList*& wc_consts,
					   const int count_value,
					   ACSCGFPGACore*& wc_mux, const char* mux_comment,
					   const int mux_ctrl_type,
					   ACSCGFPGACore*& wc_add, const char* add_comment,
					   const int count_type, const int add_ctrl_type)
{
  // Allocate a pointer to the sequencer for this controlling fpga

  // Generate the initial count value as HW constant 
  ACSCGFPGACore* wcconst_core;
  wcconst_core=construct->add_const(-count_value,SIGNED,smart_generators);
  wcconst_core->add_comment("Initial countdown value for word counter");
  wc_consts->append((Pointer) wcconst_core);

  // Obtain bit size from the word count constants
  Constants* consts=wcconst_core->sg_constants;
  int wcbits=consts->query_bitsize(0);
  if (DEBUG_IOADD)
    printf("IOADD:vector_length=%d, wcbits=%d\n",vector_length,wcbits);

  // Bow genereate the increment value as HW constants
  wcconst_core=construct->add_const((int) 1,SIGNED,smart_generators);
  wcconst_core->add_comment("Countup value of one for word counter");
  wc_consts->append((Pointer) wcconst_core);
  
  // Adjust bitwidths on constants
  HWbalance_bw(wc_consts);

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
void ACSCGFPGATarget::HWremap_ioport(ACSCGFPGACore* io_core)
{
  if (DEBUG_IOREMAP)
    printf("\n\nRemapping IO core %s\n",io_core->comment_name());

  construct->set_targetdevice(arch->get_fpga_handle(io_core->acs_device),
			      io_core->acs_device);

  // Remap input pins
  // ASSUMPTION: Hopefully there will only be one
  Pin* io_pins=io_core->pins;
  ACSIntArray* ipins=io_pins->query_inodes();
  int ipin_count=ipins->population();
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
      ACSIntArray* fpga_ext_signals=curr_fpga->ifpga_signal_id;
      construct->set_targetdevice(curr_fpga,src_core->acs_device);
      
      // For now there is only one set of port pins
      // FIX:Generalize this please!
      int io_pin=fpga_ext_signals->query(io_core->acs_outdevice);
      int io_node=curr_fpga_pins->match_acsnode(src_core->acs_id,
						io_pin);

      // Given a direction, reconfigure the external signal from bidirectional to
      // the appropriate data direction.
      // FIX: Will need to revisit if bidirectional MUX employed!!!
      Pin* fpga_ext_pins=curr_fpga->ext_signals;
      fpga_ext_pins->set_pintype(io_pin,OUTPUT_PIN);

      if (DEBUG_IOREMAP)
	printf("adding input pin port timing %d, %d\n",src_core->acs_device,
	       src_core->act_output);
      curr_fpga->add_timing(io_core->acs_outdevice,src_core->act_output,0);

      construct->connect_sg(src_core,src_pin,src_node,io_pin,
			    curr_fpga_pins,io_pin,io_node,io_pin,EXT_NODE);
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
      int dest_pin=dest_pins->match_acstype(io_core->acs_id,
					    INPUT_PIN);
      int dest_node=dest_pins->match_acsnode(io_core->acs_id,
					     dest_pin);
      // Determine and set which device we are operating in
      // Since it is an input pin the acs_device field is correct
      Fpga* curr_fpga=arch->get_fpga_handle(dest_core->acs_device);
      Pin* curr_fpga_pins=curr_fpga->ext_signals;
      ACSIntArray* fpga_ext_signals=curr_fpga->ifpga_signal_id;
      construct->set_targetdevice(curr_fpga,dest_core->acs_device);
      
      // For now there is only one set of port pins
      // FIX:Generalize this please!
      int io_pin=fpga_ext_signals->query(io_core->acs_device);
      int io_node=curr_fpga_pins->match_acsnode(io_core->acs_id,
						io_pin);
      
      // Given a direction, reconfigure the external signal from bidirectional to
      // the appropriate data direction.
      // FIX: Will need to revisit if bidirectional MUX employed!!!
      Pin* fpga_ext_pins=curr_fpga->ext_signals;
      fpga_ext_pins->set_pintype(io_pin,INPUT_PIN);

      if (DEBUG_IOREMAP)
	printf("adding output pin port timing %d, %d\n",dest_core->acs_device,
	       dest_core->act_output);
      curr_fpga->add_timing(io_core->acs_device,0,dest_core->act_output);
      
      construct->connect_sg(curr_fpga_pins,io_pin,io_node,io_pin,
			    dest_core,dest_pin,dest_node,io_pin,EXT_NODE);
    }
  
  // Remove old connections on the ioport
  io_pins->disconnect_allpins();
}

