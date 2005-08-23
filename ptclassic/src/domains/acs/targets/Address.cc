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
 Version: @(#)Address.cc      1.0     04/13/00
***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Address.h"

Address::Address()
: addrmux_star(NULL), const_stars(NULL), addrgen_star(NULL), addrbuf_star(NULL), 
  addr_size(0), addr_lo(0), addr_hi(0)
{
  const_stars=new CoreList;
}


Address::~Address() 
{
  delete addrmux_star;
  delete const_stars;
  delete addrbuf_star;
  delete addrgen_star;
}

int Address::set_address(const int addr_bits,
			 const int low_address,
			 const int high_address)
{
  addr_size=addr_bits;
  addr_lo=low_address;
  addr_hi=high_address;

  // Return happy condition
  return(1);
}

// FIX: This method should be further reduced into a white/dark star
void Address::genhw_address(Sg_Constructs* construct,
			    const int port_hits,
			    const int max_launch,
			    Pin* ext_signals,
			    const int address_signal,
			    Port_Timing* port_timing,
			    CoreList* sg_list)
{
  ACSIntArray* address_start=port_timing->get_startingaddress();
  ACSIntArray* address_step=port_timing->get_stepaddress();
  ACSIntArray* address_rate=port_timing->get_addressrate();
  ACSIntArray* vector_length=port_timing->get_veclen();
  int max_address=0;
  for (int addr_loop=0;addr_loop<address_start->population();addr_loop++)
    {
      int step_address=address_step->query(addr_loop);
      int vlength=vector_length->query(addr_loop);

      int starting_address=address_start->query(addr_loop);

      // Calculate ending address in order to anticipate maximum bits needed
      int ending_address=starting_address+(vlength*step_address*max_launch);
      if (ending_address > max_address)
	max_address=ending_address;
    }

  // Add MUXs in order to route the address generators to the address bus
  if (port_hits > 1)
    {
      addrmux_star=construct->add_mux(port_hits,port_hits,UNSIGNED,sg_list);
      addrmux_star->add_comment("Address generator MUX");
      addrmux_star->bitslice_strategy=PRESERVE_LSB;

      // Add mux ctrl signals
      int ctrl_sigs=(int) ceil(log(port_hits)/log(2));
      ACSIntArray* decoder_times=new ACSIntArray;
      int sensy_sigs=port_timing->count;
      for (int loop=0;loop<sensy_sigs;loop++)
	decoder_times->add(port_timing->get_addresstime(loop));
      if (DEBUG_GENADDRESS)
	{
	  printf("MUXCTRLs:port_hits=%d, ctrl_sigs=%d, sensy_sigs=%d\n",port_hits,ctrl_sigs,sensy_sigs);
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
	    {
	      if (DEBUG_GENADDRESS)
		printf("bselcode(%d,%d)=%d\n",sense_loop,ctrl_loop,bselcode(sense_loop,ctrl_loop));
	      if (bselcode(sense_loop,ctrl_loop)==1)
		decoder_eq[ctrl_loop]->add(1);
	      else
		decoder_eq[ctrl_loop]->add(-1);
	    }
	  if (DEBUG_GENADDRESS)
	    {
	      printf("ctrl_loop=%d\n",ctrl_loop);
	      decoder_eq[ctrl_loop]->print("decode eq");
	    }
	}

      ACSCGFPGACore* mux_decoder=construct->add_decoder(sensy_sigs,ctrl_sigs,
							decoder_times,decoder_eq,sg_list);
      
      for (int loop=0;loop<ctrl_sigs;loop++)
	{
	  int out_pin=mux_decoder->find_hardpin(OUTPUT_PIN,loop);
	  int in_pin=addrmux_star->find_hardpin(INPUT_PIN_CTRL,loop);
	  construct->connect_sg(mux_decoder,out_pin,UNASSIGNED,
				addrmux_star,in_pin,UNASSIGNED,CTRL_NODE);
	}
      
      for (int loop=0;loop<sensy_sigs;loop++)
	{
	  int in_pin=mux_decoder->find_hardpin(INPUT_PIN,decoder_times->query(loop));
	  mux_decoder->replace_pintype(in_pin,INPUT_PIN_PHASE);
	}
      
      // Connect the output of the MUX to the buffer and then the address bus
      addrbuf_star=construct->add_buffer(UNSIGNED,sg_list);
      addrbuf_star->bitslice_strategy=PRESERVE_LSB;
      addrbuf_star->add_comment("Address MUX buffer");
      construct->connect_sg(addrmux_star,UNASSIGNED,UNASSIGNED,OUTPUT_PIN_MUX_RESULT,
			    addrbuf_star,UNASSIGNED,UNASSIGNED,INPUT_PIN,
			    DATA_NODE);
      construct->connect_sg(addrbuf_star,UNASSIGNED,UNASSIGNED,address_signal,
			    ext_signals,address_signal,UNASSIGNED,address_signal,
			    EXT_NODE);
      // Cleanup
      delete decoder_times;
      for (int loop=0;loop<ctrl_sigs;loop++)
	delete decoder_eq[loop];
      delete []decoder_eq;
      
      if (DEBUG_GENADDRESS)
	printf("Added address generator mux %s\n",addrmux_star->name());
    }

  // Add an address counter for each source and sink
  ACSIntArray* address_list=new ACSIntArray(2,0);
  for (int addr_loop=0;addr_loop<address_start->population();addr_loop++)
    {
      int step_address=address_step->query(addr_loop);
      int vlength=vector_length->query(addr_loop);

      // Rollback starting address to account for initial step, but only for
      // launch rates greater than 1!
      int starting_address=address_start->query(addr_loop);
      if (address_rate->query(addr_loop)>1)
	starting_address-=step_address;

      // Calculate ending address in order to anticipate maximum bits needed
//      int ending_address=starting_address+(vlength*step_address);
      int ending_address=starting_address+(vlength*step_address*max_launch);
     
      address_list->set(0,starting_address);
      ACSCGFPGACore* addrgen_core=construct->add_counter(ending_address,
							 0,
							 address_list,
							 sg_list);
/* 
      ACSCGFPGACore* addrgen_core=construct->add_counter(starting_address,
							 step_address,
							 ending_address,
							 sg_list);
							 */
      
      if (DEBUG_GENADDRESS)
	printf("addrgen_core %s, set for start=%d, step=%d, end=%d, vlen=%d\n",
	       addrgen_core->comment_name(),
	       starting_address,step_address,ending_address,vlength);

      // Connect the addrgen to the mux or directly to the address bus
      if (port_hits > 1)
	construct->connect_sg(addrgen_core,
			      addrmux_star,INPUT_PIN_MUX_SWITCHABLE);
      else
	{
	  // Buffer this connection to the address bus if direct...not sure of the dependencies
	  addrbuf_star=construct->add_buffer(UNSIGNED,sg_list);
	  addrbuf_star->add_comment("Address buffer");
	  construct->connect_sg(addrgen_core,UNASSIGNED,UNASSIGNED,OUTPUT_PIN,
				addrbuf_star,UNASSIGNED,UNASSIGNED,INPUT_PIN,
				DATA_NODE);
	  construct->connect_sg(addrbuf_star,UNASSIGNED,UNASSIGNED,address_signal,
				ext_signals,address_signal,UNASSIGNED,address_signal,
				EXT_NODE);
	}
      
      // ASSUMPTION:OR pin numbers wont change
      ACSCGFPGACore* orce_core=construct->add_sg("ACS","OR","CGFPGA",
						 BOTH,UNSIGNED,sg_list);
      construct->connect_sg(orce_core,
			    addrgen_core,INPUT_PIN_CE);
      Pin* orce_pins=orce_core->pins;

      int addresstime=port_timing->get_addresstime(addr_loop);

      orce_pins->set_pintype(0,INPUT_PIN_PREPHASE);
      orce_pins->set_pinpriority(0,addresstime);
      orce_pins->set_precision(0,0,1,LOCKED);
      orce_pins->set_pintype(1,INPUT_PIN_PHASE_START);
      orce_pins->set_pinpriority(1,addresstime);
      orce_pins->set_precision(1,0,1,LOCKED);
      orce_pins->set_precision(2,0,1,LOCKED);
      
      // Remap pin types for linkage to the sequencer
      Pin* addr_pins=addrgen_core->pins;
      int go_pin=addr_pins->retrieve_type(INPUT_PIN_EXTCTRL);
      addr_pins->set_pintype(go_pin,INPUT_PIN_PHASE_START);
      addr_pins->set_pinpriority(go_pin,addresstime);

      int address_enable=addr_pins->retrieve_type(INPUT_PIN_CE);
      addr_pins->set_pintype(address_enable,INPUT_PIN_PHASE);
      addr_pins->set_pinpriority(address_enable,addresstime);

      // Resets address generator to zero
      // Will default to global reset via resolver
//      int address_reset=addr_pins->retrieve_type(INPUT_PIN_RESET);
//      addr_pins->set_pintype(address_reset,INPUT_PIN_SEQRESET);

      // Frees up address generator for operation
      int address_clear=addr_pins->retrieve_type(INPUT_PIN_CLR);
      addr_pins->set_pintype(address_clear,INPUT_PIN_ADDRCLR);
//      addr_pins->set_pintype(address_clear,INPUT_PIN_RESET);
    }

  // Cleanup
  delete address_list;
}

// FIX: This method should be further reduced into a white/dark star
void Address::genhw_lutaddress(Sg_Constructs* construct,
			       const int lut_acsid,
			       const int index_mbit,
			       const int index_bitlen,
			       const int max_launch,
			       Pin* ext_signals,
			       const int address_signal,
			       Port_Timing* port_timing,
			       CoreList* sg_list)
{
  ACSIntArray* address_start=port_timing->get_startingaddress();
  ACSIntArray* address_step=port_timing->get_stepaddress();
  ACSIntArray* address_rate=port_timing->get_addressrate();
  ACSIntArray* vector_length=port_timing->get_veclen();

  int max_address=0;
  for (int addr_loop=0;addr_loop<address_start->population();addr_loop++)
    {
      int step_address=address_step->query(addr_loop);
      int vlength=vector_length->query(addr_loop);

      int starting_address=address_start->query(addr_loop);

      // Calculate ending address in order to anticipate maximum bits needed
      int ending_address=starting_address+(vlength*step_address*max_launch);
      if (ending_address > max_address)
	max_address=ending_address;
    }
  if (DEBUG_GENADDRESS)
    printf("max_address=%d\n",max_address);

  // Add an address counter for each source and sink
  // FIX: embedding the lut_acsid upfront rather than as argument, not sure which is best way
  ACSIntArray* address_list=new ACSIntArray;
  address_list->add(lut_acsid);
  address_list->add(index_mbit);
  address_list->add(index_bitlen);
  for (int addr_loop=0;addr_loop<address_start->population();addr_loop++)
    {
      int step_address=address_step->query(addr_loop);

      // Rollback starting address to account for initial step, but only for
      // launch rates greater than 1!
      int starting_address=address_start->query(addr_loop);
      if (address_rate->query(addr_loop)>1)
	starting_address-=step_address;

      address_list->add(starting_address);
    }
     
  ACSCGFPGACore* addrgen_core=construct->add_counter(max_address,
						     1,
						     address_list,
						     sg_list);
      
  if (DEBUG_GENADDRESS)
    printf("addrgen_core %s\n",addrgen_core->comment_name());
  address_list->print("core addresses");

  // Connect the addrgen directly to the address bus
  // Buffer this connection to the address bus if direct...not sure of the dependencies
  addrbuf_star=construct->add_buffer(UNSIGNED,sg_list);
  addrbuf_star->add_comment("Address buffer");
  construct->connect_sg(addrgen_core,UNASSIGNED,UNASSIGNED,OUTPUT_PIN,
			addrbuf_star,UNASSIGNED,UNASSIGNED,INPUT_PIN,
			DATA_NODE);
  construct->connect_sg(addrbuf_star,UNASSIGNED,UNASSIGNED,address_signal,
			ext_signals,address_signal,UNASSIGNED,address_signal,
			EXT_NODE);
      
  // NOTE:All times are identical!
  int addresstime=port_timing->get_addresstime(0);
  

  // ASSUMPTION:OR pin numbers wont change
  ACSCGFPGACore* orce_core=construct->add_sg("ACS","OR","CGFPGA",
					     BOTH,UNSIGNED,sg_list);
  construct->connect_sg(orce_core,
			addrgen_core,INPUT_PIN_CE);
  Pin* orce_pins=orce_core->pins;
  
  orce_pins->set_pintype(0,INPUT_PIN_PREPHASE);
  orce_pins->set_pinpriority(0,addresstime);
  orce_pins->set_precision(0,0,1,LOCKED);
  orce_pins->set_pintype(1,INPUT_PIN_PHASE_START);
  orce_pins->set_pinpriority(1,addresstime);
  orce_pins->set_precision(1,0,1,LOCKED);
  orce_pins->set_precision(2,0,1,LOCKED);
  
  // Remap pin types for linkage to the sequencer
  Pin* addr_pins=addrgen_core->pins;
  int go_pin=addr_pins->retrieve_type(INPUT_PIN_EXTCTRL);
  addr_pins->set_pintype(go_pin,INPUT_PIN_PHASE_START);
  addr_pins->set_pinpriority(go_pin,addresstime);

/*  
  int address_enable=addr_pins->retrieve_type(INPUT_PIN_CE);
  addr_pins->set_pintype(address_enable,INPUT_PIN_PHASE);
  addr_pins->set_pinpriority(address_enable,addresstime);
  */

  // Resets address generator to zero
  // ASSUMPTION:OR pin numbers wont change
  ACSCGFPGACore* reset_core=construct->add_sg("ACS","OR","CGFPGA",
					      BOTH,UNSIGNED,sg_list);
  construct->connect_sg(reset_core,
			addrgen_core,INPUT_PIN_RESET);
  Pin* reset_pins=reset_core->pins;
  
  reset_pins->set_pintype(0,INPUT_PIN_RESET);
  reset_pins->set_pintype(1,INPUT_PIN_EXTCTRL);
  reset_pins->set_precision(0,0,1,LOCKED);
  reset_pins->set_precision(1,0,1,LOCKED);
  reset_pins->set_precision(2,0,1,LOCKED);
//      int address_reset=addr_pins->retrieve_type(INPUT_PIN_RESET);
//      addr_pins->set_pintype(address_reset,INPUT_PIN_SEQRESET);

  // Frees up address generator for operation
  int address_clear=addr_pins->retrieve_type(INPUT_PIN_CLR);
  addr_pins->set_pintype(address_clear,INPUT_PIN_ADDRCLR);
//      addr_pins->set_pintype(address_clear,INPUT_PIN_RESET);

  // Cleanup
  delete address_list;

}
