static const char file_id[] = "HWutils.cc";

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
 Version: @(#)HWutils.cc      1.0     10/20/99
***********************************************************************/
#include "HWutils.h"

#ifdef __GNUG__
#pragma implementation
#endif

//////////////////////////////////
// Standard memory warning handler
//////////////////////////////////
void out_of_memory(void)
{
  printf("Memory exhausted\n");
  exit(1);
}
void ACSCGFPGATarget::memory_monitor(void)
{
  set_new_handler(out_of_memory);
}


///////////////////////////////////////////////////////////////
// Given a star, return a pointer to the FPGA core if available
// ASSUMPTION: An ACSStar must have an ACSCorona.
///////////////////////////////////////////////////////////////
ACSCGFPGACore* ACSCGFPGATarget::fetch_fpgacore(Star* ptolemy_star)
{
  if (ptolemy_star->isA("ACSStar"))
    {
      ACSStar* acs_star=(ACSStar*) ptolemy_star;
      ACSCorona* acs_corona=(ACSCorona*) acs_star;
      if (acs_corona==NULL)
	printf("fetch_fpgacore:acs_corona is null!\n");
      ACSCore* acs_core=(ACSCore*) (acs_corona->currentCore);
      if (acs_core==NULL)
	printf("fetch_fpgacore:Not a valud ACSCore\n");
      if (acs_core->isA("ACSCGFPGACore"))
	{
	  ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) acs_core;
	  return(fpga_core);
	}
      else
	return(NULL);
    }
  else
    return(NULL);
}

/////////////////////////////////////////////////////////////////
// Given a star, return a pointer to the FPGA corona if available
// ASSUMPTION: An ACSStar must have an ACSCorona.
/////////////////////////////////////////////////////////////////
ACSCorona* ACSCGFPGATarget::fetch_corona(Star* ptolemy_star)
{
  if (ptolemy_star->isA("ACSStar"))
    {
      ACSStar* acs_star=(ACSStar*) ptolemy_star;
      ACSCorona* acs_corona=(ACSCorona*) acs_star;
      if (acs_corona==NULL)
	printf("fetch_fpgacore:acs_corona is null!\n");
      ACSCore* acs_core=(ACSCore*) acs_corona->currentCore;
      if (acs_core->isA("ACSCGFPGACore"))
	return(acs_corona);
      else
	return(NULL);
    }
  else
    return(NULL);
}

//////////////////////////////////////////////////////////////////////////
// Given an acs_id return a handle to the matching star, NULL if no match.
//////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* ACSCGFPGATarget::HWfind_star(int acs_id)
{
  if (acs_id < 0)
    return(NULL);

  //
  // Return a pointer to the smart generator with the specified acs_id
  //

  // Loop over all the smart generators
  int total_sgs=smart_generators->size();
  for (int loop=0;loop<total_sgs;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      if (fpga_core->acs_id==acs_id)
	{
	  // Star found return
	  return(fpga_core);
	}
    }

  // Return error
  return(NULL);
}

void ACSCGFPGATarget::HWset_wordcount(CoreList* sg_list)
{
  if (DEBUG_WORDCOUNT)
    printf("\n\nHWset_wordcount:started\n");

  int runaway=0;
  int done=0;
  while (!done)
    if (HWall_wordset(sg_list))
      {
	if (DEBUG_WORDCOUNT)
	  printf("HWset_wordcount: Wordcount growth complete\n");
	done=1;
      }
  else
    {
      for (int loop=0;loop<sg_list->size();loop++)
	{
	  ACSCGFPGACore* unset_sg=(ACSCGFPGACore*) sg_list->elem(loop);
	  Fpga* dirty_fpga=arch->get_fpga_handle(unset_sg->acs_device);
	  Pin* dirty_pins=unset_sg->pins;
	  if (DEBUG_WORDCOUNT)
	    printf("Examining smart generator for word count%s\n",unset_sg->comment_name());

	  // Find an unset smart generator
	  if (unset_sg->word_count==UNASSIGNED)
	    {
	      if (DEBUG_WORDCOUNT)
		printf("Smart generator %s needs to be word count updated\n",
		       unset_sg->comment_name());
		
	      // Determine connections and update BW accordingly
	      for (int pin_loop=0;pin_loop<dirty_pins->query_pincount();pin_loop++)
		{
		  if (DEBUG_WORDCOUNT)
		    printf("testing pin %d of %d, word_count=%d\n",
			   pin_loop,
			   dirty_pins->query_pincount(),
			   dirty_pins->query_wordcount(pin_loop));
		  if (dirty_pins->query_pintype(pin_loop)==INPUT_PIN)
		    {
		      Connectivity* dirty_connects=dirty_pins->query_connection(pin_loop);
		      for (int node_loop=0;node_loop < dirty_connects->node_count;node_loop++)
			{
			  int nodetype=dirty_connects->query_pintype(node_loop);
			  Pin* origin_pins=NULL;
			  ACSCGFPGACore* conn_star=NULL;
			  switch (nodetype)
			    {
			    case DATA_NODE:
			      conn_star=HWfind_star(dirty_connects->query_acsid(node_loop));
			      origin_pins=conn_star->pins;
			      break;
			    case EXT_NODE:
			      origin_pins=dirty_fpga->ext_signals;
			      break;
			    case CONSTANT_NODE:
			      origin_pins=dirty_fpga->constant_signals;
			      break;
			    case CTRL_NODE:
			      origin_pins=dirty_fpga->ctrl_signals;
			      break;
			    default:
			      if (DEBUG_WORDCOUNT)
				{
				  printf("Pin %d, node %d ",
					 pin_loop,
					 node_loop);
				  printf("is connected to unknown signal\n");
				}
			    }

			  // Fetch new BW from source
			  int origin_node=dirty_connects->query_pinid(node_loop);
			  int src_wordcount=origin_pins->query_wordcount(origin_node);
			
			  if (DEBUG_WORDCOUNT)
			    printf("remote connection: word_count=%d\n",src_wordcount);

			  // Set new BW
			  // ASSUMPTION:Star-level wordcount value is uninteresting other than being set
			  dirty_pins->set_wordcount(pin_loop,src_wordcount);
			  unset_sg->word_count=src_wordcount;

			} // for (int node_loop=0;
		    } // if (((dirty_pins->pintype(pin_loop)==INPUT_PIN) ||
		}  //for (int pin_loop=0;pin_loop<(unset_sg->pins)->pin_count;pin_loop++)
	    } // if (unset_sg->word_count)
	} // for (int loop=0;loop<sg_list->size();loop++)

      // DEBUG
      runaway++;
      if (runaway>RUNAWAY_BW)
	{
	  done=1;
	  if (DEBUG)
	    printf("HWset_wordcount:Runaway iterator!\n");
	}
    } // if (!HWset_wordcount(sg_list))
}

////////////////////////////////////////////////////////////////////////
// This will examine an internal netlist and propogate bitwidths so that 
// all sources and sinks match in terms of bit-sizes.  Limited feedback
// is handled
////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWset_bw(CoreList* sg_list)
{
  if (DEBUG_BW)
    printf("\n\nHWset_bw:started\n");

  int runaway=0;
  int done=0;
  int first_pass=1;
  int exempt=0;
  while (!done)
    if (HWall_bwset(sg_list))
      {
	if (DEBUG_BW)
	  printf("HWset_bw: BW growth complete\n");
	done=1;
      }
    else
      {
	for (int loop=0;loop<sg_list->size();loop++)
	  {
	    ACSCGFPGACore* unset_sg=(ACSCGFPGACore*) sg_list->elem(loop);
	    Fpga* dirty_fpga=arch->get_fpga_handle(unset_sg->acs_device);
	    Pin* dirty_pins=unset_sg->pins;
	    if (DEBUG_BW)
	      printf("Examining smart generator %s\n",unset_sg->comment_name());

	    // Make sure that minimum and maximum bitwidth constraints are met
	    HWset_requirements(unset_sg);

	    // Find an unset smart generator
	    if (unset_sg->bw_dirty)
	      {
		if (DEBUG_BW)
		  printf("Smart generator %s acs_id=%d needs to be BW updated\n",unset_sg->comment_name(),
			 unset_sg->acs_id);
		
		// Determine connections and update BW accordingly
		int skip_update=0;
                for (int pin_loop=0;pin_loop<dirty_pins->query_pincount();pin_loop++)
		  {
		    if (DEBUG_BW)
		      printf("testing pin %d of %d, size (%d,%d) lock=%d\n",
			     pin_loop,
			     dirty_pins->query_pincount(),
			     dirty_pins->query_majorbit(pin_loop),
			     dirty_pins->query_bitlen(pin_loop),
			     dirty_pins->query_preclock(pin_loop));
		    if ((dirty_pins->query_pinassigned(pin_loop)==ASSIGNED) &&
			(dirty_pins->query_preclock(pin_loop)==UNLOCKED) &&
			((dirty_pins->pin_classtype(pin_loop)==INPUT_PIN) ||
			 (dirty_pins->pin_classtype(pin_loop)==BIDIR_PIN)))
		      {
			if (DEBUG_BW)
			  printf("Sourced pin %d is connected and unlocked\n",
				 pin_loop);

			Connectivity* dirty_connects=dirty_pins->query_connection(pin_loop);
			for (int node_loop=0;node_loop < dirty_connects->node_count;node_loop++)
			  {
			    int nodetype=dirty_connects->query_pintype(node_loop);
			    Pin* origin_pins=NULL;
			    ACSCGFPGACore* conn_star=NULL;
			    int lock_type=UNLOCKED;
			    switch (nodetype)
			      {
			      case DATA_NODE:
				conn_star=HWfind_star(dirty_connects->query_acsid(node_loop));
				if (conn_star->bw_exempt)
				  exempt=1;
				else
				  exempt=0;
				origin_pins=conn_star->pins;
				break;
			      case EXT_NODE:
				origin_pins=dirty_fpga->ext_signals;
				lock_type=LOCKED;
				break;
			      case CONSTANT_NODE:
				origin_pins=dirty_fpga->constant_signals;
				lock_type=LOCKED;
				break;
			      case CTRL_NODE:
				origin_pins=dirty_fpga->ctrl_signals;
				lock_type=LOCKED;
				break;
			      default:
				if (DEBUG_BW)
				  {
				    printf("Pin %d, node %d ",
					   pin_loop,
					   node_loop);
				    printf("is connected to unknown signal\n");
				  }
			      }

			    // Fetch new BW from source
			    int origin_node=dirty_connects->query_pinid(node_loop);
			    int src_mbit=origin_pins->query_majorbit(origin_node);
			    int src_bitlen=origin_pins->query_bitlen(origin_node);

			    if (lock_type!=LOCKED)
			      lock_type=origin_pins->query_preclock(origin_node);
			
			if (DEBUG_BW)
			  {
			    printf("remote connection: bw (%d,%d) lock=%d\n",
				   src_mbit,src_bitlen,lock_type);
			    printf("src exempt=%d\n",exempt);
			  }

			// Set new BW
			if (!exempt)
			  {
			    if (DEBUG_BW)
			      printf("updating precision on pin %d, to (%d.%d) lock=%d\n",
				     pin_loop,src_mbit,src_bitlen,lock_type);

			    // Correct the precision
			    dirty_pins->set_precision(pin_loop,src_mbit,src_bitlen,lock_type);

			    // Change all precision-dependant factors
			    unset_sg->update_sg(lock_type,UNLOCKED);
			  }
			  } // for (int node_loop=0;
		      } // if (((dirty_pins->pintype(pin_loop)==INPUT_PIN) ||
		  }  //for (int pin_loop=0;pin_loop<(unset_sg->pins)->pin_count;pin_loop++)


		// Are all applicable connections set?
		if (!skip_update)
		  {
//		    unset_sg->update_resources(LOCKED);
//		    unset_sg->update_resources(UNLOCKED);
		    unset_sg->update_sg(UNLOCKED,UNLOCKED);
		    unset_sg->bw_dirty=0;

		    // Flag neighboring smart generators as dirty
		    HWset_dirty(unset_sg);
		  }

	      } // if (unset_sg->bw_dirty)
	  } // for (int loop=0;loop<sg_list->size();loop++)

	first_pass=0;

	// DEBUG
	runaway++;
	if (runaway>RUNAWAY_BW)
	  {
	    done=1;
	    if (DEBUG)
	      printf("HWset_bw:Runaway iterator!\n");
	  }
      } // if (!HWset_bw(sg_list))
}

void ACSCGFPGATarget::HWset_requirements(ACSCGFPGACore* unset_sg)
{
  Pin* unset_pins=unset_sg->pins;
  for (int pin_loop=0;pin_loop<unset_pins->query_pincount();pin_loop++)
    {
      int update=0;
      int src_bitlen=unset_pins->query_bitlen(pin_loop);
      int max_bit=unset_pins->query_maxbitlen(pin_loop);
      int min_bit=unset_pins->query_minbitlen(pin_loop);

      if (DEBUG_BW)
	printf("Core:%s, pin %d, bitlen=%d, min=%d, max=%d\n",
	       unset_sg->comment_name(),
	       pin_loop,
	       src_bitlen,
	       min_bit,
	       max_bit);
      
      // Check for minimum requirements
      if (src_bitlen < min_bit)
	{
	  src_bitlen=min_bit;
	  update=1;
	  if (DEBUG_BW)
	    printf("Pin %d, does not meet minimum (%d) reqs.\n",
		   pin_loop,
		   min_bit);
	}

      // Check for maximum requirements
      if (src_bitlen > max_bit)
	{
	  src_bitlen=max_bit;
	  update=1;
	  if (DEBUG_BW)
	    printf("Pin %d, exceeds maximum (%d) thres.\n",
		   pin_loop,
		   max_bit);
	}
		
      // Set new BW
      if (update)
	{
	  // Maintain current majorbit and lock type
	  int src_mbit=unset_pins->query_majorbit(pin_loop);
	  int lock_type=unset_pins->query_preclock(pin_loop);

	  // Correct the precision
	  unset_pins->set_precision(pin_loop,src_mbit,src_bitlen,lock_type);

	  // Change all precision-dependant factors
//	  unset_sg->update_resources(lock_type);
	  unset_sg->update_sg(UNLOCKED,UNLOCKED);
	}
    }
}

Pin* ACSCGFPGATarget::HWfind_origin(int nodetype,int acs_id)
{
  ACSCGFPGACore* origin_sg=NULL;
  origin_sg=HWfind_star(acs_id);
  if (origin_sg!=NULL)
    {
      Pin* origin_pins=origin_sg->pins;
      ACSIntArray* origin_onodes=origin_pins->query_onodes();

      if (DEBUG_BW)
	printf("Pin is driven by smart generator %s\n",
	       origin_sg->comment_name());	 	
      if (origin_sg->bw_dirty)
	if ((origin_sg->acs_type==SOURCE) ||
	    (origin_sg->acs_type==SINK))
	  {
	    origin_sg->bw_dirty=0;
	    return(origin_sg->pins);
	  }
	else if ((origin_sg->bw_dirty) && 
		 (origin_pins->query_pincount() == 
		  (origin_onodes->population())))
	  {
	    if (DEBUG_BW)
	      printf("This is a driving-only smart generator and is upto date\n");
	    origin_sg->bw_dirty=0;
	    return(origin_sg->pins);
	  }
	else
	  {
	    if (DEBUG_BW)
	      printf("HWset_bw: Source sg is also dirty\n");
	    return(NULL);
	  }
      else
	return(origin_sg->pins);
    }
  else
    {
      printf("ACSCGFPGATARGET::HWset_bw:Error, unable to ");
      printf("find sourcing smart generator\n");
      return(NULL);
    }
}


// For now, only test for loops that span only a single star
// FIX:Not working, this should iterate over connections looking for loops to the
// root acs_id;
CoreList* ACSCGFPGATarget::HWfind_loop(ACSCGFPGACore* starting_sg)
{
  static int root_id=starting_sg->acs_id;
  Pin* starting_pins=starting_sg->pins;
  CoreList* connected_sg=NULL;
  ACSIntArray* onodes=starting_pins->query_onodes();
  for (int onode_loop=0;onode_loop<onodes->population();onode_loop++)
    if (starting_pins->query_pinassigned(onode_loop)!=UNASSIGNED)
      {
	int pin_no=onodes->query(onode_loop);
	int src_pin=starting_pins->match_acsnode(root_id,pin_no);
	if (src_pin != -1)
	  {
	    if (connected_sg==NULL)
	      connected_sg=new CoreList;
	    connected_sg->append(starting_sg);
	  }
      }
  return(connected_sg);
}


int ACSCGFPGATarget::HWall_bwset(CoreList* sg_list)
{
  for (int loop=0;loop<sg_list->size();loop++)
    {
      ACSCGFPGACore* unset_sg=(ACSCGFPGACore*) sg_list->elem(loop);
      if (unset_sg->bw_exempt)
	unset_sg->bw_dirty=0;
      if (unset_sg->bw_dirty)
	return(0);
    }
  return(1);
}

int ACSCGFPGATarget::HWall_wordset(CoreList* sg_list)
{
  for (int loop=0;loop<sg_list->size();loop++)
    {
      ACSCGFPGACore* unset_sg=(ACSCGFPGACore*) sg_list->elem(loop);
      if (unset_sg->word_count==0)
	return(0);
    }
  return(1);
}


// FIX:What about feedback loops?
void ACSCGFPGATarget::HWset_dirty(ACSCGFPGACore* updated_sg)
{
  if (DEBUG_BW)
    printf("All downstream smart generators are now being dirtied\n");

  // Find sourcing nodes
  Pin* updated_pins=updated_sg->pins;
  ACSIntArray* onodes=updated_pins->query_onodes();
  for (int onode_loop=0;onode_loop<onodes->population();onode_loop++)
    {
      int pin_no=onodes->query(onode_loop);
      Connectivity* ocon=updated_pins->query_connection(pin_no);
      for (int oloop=0;oloop<ocon->node_count;oloop++)
	if (ocon->query_pintype(oloop)==DATA_NODE)
	  {
	    ACSCGFPGACore* dest_sg=HWfind_star(ocon->query_acsid(oloop));
	    if (dest_sg==NULL)
	      {
		if (DEBUG_BW)
		  printf("HWset_dirty: Unable to find destination sg!!\n");
		return;
	      }

	    Pin* dest_pins=dest_sg->pins;
	    ACSIntArray* dest_onodes=dest_pins->query_onodes();
	    if (updated_sg->acs_id==ocon->query_acsid(oloop))
	      {
		if (dest_sg->bw_loop==0)
		  {
		    if (DEBUG_BW)
		      {
			printf("HWset_dirty: Local feedback loop.");
			printf("Allowing one loop dependency\n");
			printf("No need to dirty downstream\n");
		      }
		    dest_sg->bw_loop=1;
		    dest_sg->bw_dirty=1;
		  }
		else
		  {
		    if (DEBUG_BW)
		      {
			printf("HWset_dirty: Local feedback loop.");
			printf("One loop already evaluated, ignoring\n");
		      }
		  }
		  
	      }
	    else if (dest_sg->bw_dirty==1)
	      {
		if (DEBUG_BW)
		  printf("HWset_dirty: Next smart generator is already dirty\n");
	      }
	    // Check for source-only smart generators
	    else if ((dest_sg->bw_dirty) && 
		     (dest_pins->query_pincount()) == (dest_onodes->population()))
	      {
		if (DEBUG_BW)
		  printf("This is a driving-only smart generator and is upto date\n");
	      }
	    else
	      {
		if (DEBUG_BW)
		  printf("Dirtying smart generator %s\n",
			 dest_sg->comment_name());
		dest_sg->bw_dirty=1;
//		HWset_dirty(dest_sg);
	      }
	  }
    }
}

////////////////////////////////////////////////////////////////////
// Find out if a bit (bit_pos) in the binary sequence representation
// of (value) is set or not.  Return code dictates the result
// FIX:What it lacks in elegance, makes up for the tenancity it
//     evidences;)
////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::bselcode_old(int value, int bit_pos)
{
  int bit_power=(int) pow(2.0,bit_pos);

  // Check for trivial answer
  if (value < bit_power)
    return(0);
  if (value == bit_power)
    return(1);

  // Constrain shaving loop
  // log2(x)=log(x)/log(2)
  double precise_bit=log(value)/log(2.0);
  int max_bit=(int) ceil(precise_bit);

  int local_val=value;
  int curr_bit=max_bit;
  for (int bit_loop=max_bit;bit_loop>bit_pos;bit_loop--)
    {
      curr_bit=bit_loop;
      int power=(int) pow(2.0,bit_loop);
      if ((local_val-power) >= 0)
	local_val-=power;
      if (local_val<=0)
	break;
    }
  
  // Now examine bit position in question
  if (curr_bit==bit_pos+1)
    {
      curr_bit--;
      int power=(int) pow(2.0,curr_bit);
      if (((local_val-power)>=0) && (curr_bit==bit_pos))
	return(1);
      else
	return(0);
    }
  return(0);
}

////////////////////////////////////////////////////////////////////////////
// Display all information regarding stars and connectivity with the galaxy.
////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWdisplay_connectivity(char* title,
					     CoreList* sg_list)
{
  printf("\n\n----------------------------------------\n");
  printf("\t\t%s\n",title);
  printf("\n----------------------------------------\n");
  
  // Loop over all the smart generators
  for (int loop=0;loop<sg_list->size();loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(loop);

      // Display star info
      int row_size=0;
      int col_size=0;
      (fpga_core->resources)->get_occupancy(row_size,col_size);
      printf("\n--------------------------------------------------\n");
      printf("SG:%s, type:",fpga_core->comment_name());

      switch (fpga_core->acs_type)
	{
	case SOURCE:
	  printf("SOURCE ");
	  break;
	case SINK:
	  printf("SINK ");
	  break;
	case BOTH:
	  printf("BOTH ");
	  break;
	case FORK:
	  printf("FORK ");
	  break;
	case IOPORT:
	  printf("IOPORT ");
	  break;
	default:
	  printf("Unknown ");
	}

      printf("Acs id=%d\n",fpga_core->acs_id);

      if (fpga_core->acs_type==IOPORT)
	{
	  ACSIntArray* device_nums=fpga_core->sg_get_privaledge();
	  int acs_indevice=device_nums->query(0);
	  int acs_outdevice=device_nums->query(1);
	  printf("Acs device=%d, Acs Indevice=%d, Acs Outdevice=%d\n",
		 fpga_core->acs_device,
		 acs_indevice,
		 acs_outdevice);
	}
      else
	printf("Acs device=%d\n",fpga_core->acs_device);
      printf("Acs delay=%d, Alg delay=%d, pipe delay=%d\n",
	     fpga_core->acs_delay,
	     fpga_core->alg_delay,
	     fpga_core->pipe_delay);
      printf("SG has %d associated delay stars\n",fpga_core->acsdelay_count);
      printf("SG will receive its data at time %d\n",fpga_core->act_input);
      printf("SG will assert its data at time %d\n",fpga_core->act_output);
      printf("SG will process %d words\n",fpga_core->word_count);
      printf("SG launch rate is %d, completing at %d\n",fpga_core->act_launch,
	     fpga_core->act_complete);
      printf("SG bitstrategy=");
      if (fpga_core->bitslice_strategy==PRESERVE_LSB)
	printf("LSB");
      else
	printf("MSB");

      HWdisplay_connects(fpga_core->pins);

      if (fpga_core->macro_query()==DARK_STAR)
	{
	  HWdisplay_connectivity("BEGIN DARK_STAR children",
				 fpga_core->child_sgs);
	  printf("\t\tEND DARK_STAR children\n");
	}

    } //   for (int loop=0;loop<sg_list->size();loop++)

  // Display fpga connectivity info
  arch->print_arch(title);
}


void ACSCGFPGATarget::HWdisplay_connects(Pin* localp)
{
  // Now display port info
  printf("\n\nConnectivity info:\n");
  if (localp != NULL)
    {
      printf("Pins:\n");
      for (int loop=0;loop<localp->query_pincount();loop++)
	{
	  printf("%s of pin type %d, bw of (%d,%d), lock(%d), netlistid(%d), pin_assigned=%d, priority=%d\n",
		 localp->query_pinname(loop),
		 localp->query_pintype(loop),
		 localp->query_majorbit(loop),
		 localp->query_bitlen(loop),
		 localp->query_preclock(loop),
		 localp->query_netlistid(loop),
		 localp->query_pinassigned(loop),
		 localp->query_pinpriority(loop));
	  Connectivity* localc=localp->query_connection(loop);
	  for (int loop=0;loop<localc->node_count;loop++)
	    {
	      printf("\tNode #%d is connected via type ",loop);
	      ACSCGFPGACore* conn_sg;
	      switch (localc->query_pintype(loop))
		{
		case DATA_NODE:
		  conn_sg=HWfind_star(localc->query_acsid(loop));
		  if (conn_sg != NULL)
		    printf("DATA_NODE to sg %s acs_id=%d and signal %d\n",
			   conn_sg->comment_name(),
			   localc->query_acsid(loop),
			   localc->query_pinsignal(loop));
		  else
		    printf("DATA_NODE to unknown sg and signal %d\n",
			   localc->query_pinsignal(loop));
		  break;
		case CTRL_NODE:
		  printf("CTRL_NODE to signal %d\n",
			 localc->query_pinsignal(loop));
		  break;
		case EXT_NODE:
		  printf("EXT_NODE to signal %d\n",
			 localc->query_pinsignal(loop));
		  break;
		case CONSTANT_NODE:
		  printf("CONSTANT_NODE to signal %d\n",
			 localc->query_pinsignal(loop));
		  break;
		default:
		  printf("Unknown\n");
		}
	    }
	}
      localp->print_pinclasses();
    }
  else
    printf("No pins\n");
}


////////////////////////////////////////////////////////////////////
// Query an integer Ptolemy parameter from the named state parameter
////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::intparam_query(const char* state_name)
{
  int val=-1;
  State* state=stateWithName(state_name);
  if (state!=NULL)
    {
      IntState* int_state=(IntState *)state;
      val=(int) (*int_state);
    }
  return(val);
}


//////////////////////////////////////////////
// Examine all devices looking for contentions
//////////////////////////////////////////////
int ACSCGFPGATarget::HWtest_contentions(void)
{
  for (int fpga_loop=0;fpga_loop<arch->fpga_count;fpga_loop++)
    {
      Fpga* fpga_device=arch->get_fpga_handle(fpga_loop);
      
      if (fpga_device->usage_state==FPGA_USED)
	{
	  // Check all child smart generators to this fpga
	  CoreList* fpga_cores=fpga_device->get_childcores_handle();
	  for (int core_loop=0;core_loop<fpga_cores->size();core_loop++)
	    {
	      ACSCGFPGACore* fpga_core=
		(ACSCGFPGACore*) fpga_cores->elem(core_loop);
	      Pin* fpga_pins=fpga_core->pins;
	      int bad_pin=HWtest_contention(fpga_pins);
	      if (bad_pin!=-1)
		{
		  sprintf(err_message,"Contention:Error, smart generator %s, pin %s is driven by multiple sources, cannot proceed\n",
			  fpga_core->comment_name(),
			  fpga_pins->query_pinname(bad_pin));
		  Error::error(*this,err_message);
		  return(0);
		}
	    }

	  // Check all external signals
	  Pin* fpga_ext_signals=fpga_device->ext_signals;
	  int bad_pin=HWtest_contention(fpga_ext_signals);
	  if (bad_pin!=-1)
	    {
	      sprintf(err_message,"Contention:Error, external pin %s on fpga %d, is driven by multiple sources, cannot proceed\n",
		      fpga_ext_signals->query_pinname(bad_pin),
		      fpga_loop);
	      Error::error(*this,err_message);
	      return(0);
	    }
	}
    }

  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////////////////
// Examine the input pins.
// If there are multiple input nodes, then a contention condition
// has occured and the code generation must terminate, or at least
// caution the user
//////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWtest_contention(Pin* fpga_pins)
{
  ACSIntArray* ipins=fpga_pins->query_inodes();
  for (int ipin_loop=0;ipin_loop<ipins->population();ipin_loop++)
    {
      int ipin=ipins->query(ipin_loop);
      Connectivity* icons=fpga_pins->query_connection(ipin);
      if (icons->node_count>1)
	return(ipin);
    }

  // Return happy condition
  // FIX:Yuck, I switched to negative for happy....
  return(-1);
}

