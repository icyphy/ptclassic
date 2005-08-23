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
 Version: @(#)HWSchedule.cc	1.7 08/02/01
***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <math.h>
// Ptolemy kernel includes
#include "Domain.h"
#include "Block.h"
#include "KnownBlock.h"
#include "GalIter.h"
#include "Error.h"
#include "EventHorizon.h"
#include "miscFuncs.h"
#include "pt_fstream.h"
#include "SimControl.h"
#include "Tokenizer.h"
#include "ProfileTimer.h"
#include "checkConnect.h"

// Ptolemy domain includes
//#include "LoopScheduler.h"
//#include "AcyLoopScheduler.h"
//#include "SDFScheduler.h"
//#include "SDFCluster.h"
//#include "CGUtilities.h"
//#include "ACSStar.h"
//#include "ACSPortHole.h"
#include "HWSchedule.h"
#include <assert.h>

#include <time.h>
//#include <sys/types.h>
//#include <sys/stat.h>                 // define stat structure

#include "acs_starconsts.h"

MEMPort::MEMPort() 
{
  total_sgs=0;
  source_stars=new SequentialList;
  sink_stars=new SequentialList;
  dataimux_star=NULL;
  dataomux_star=NULL;
  addrmux_star=NULL;
  const_stars=new SequentialList;
  addrgen_star=NULL;
  addrbuf_star=NULL;
  mem_timing=NULL;
  data_size=0;
  addr_size=0;
  addr_lo=0;
  addr_hi=0;
  portuse=UNUSED;
  bidir_data=0;
  separate_rw=0;
  read_skew=0;
  write_skew=0;

  datain_signal_id=-1;
  dataout_signal_id=-1;
  addr_signal_id=-1;
  read_signal_id=-1;
  write_signal_id=-1;
  enable_signal_id=-1;

  current_act=0;
  pt_count=0;
  mem_timing=NULL;
  sequence_overflow=0;

  datain_name=NULL;
  dataout_name=NULL;
  addr_name=NULL;
  read_name=NULL;
  write_name=NULL;
  enable_name=NULL;
}


MEMPort::~MEMPort() 
{
  delete source_stars;
  delete sink_stars;
  delete dataimux_star;
  delete dataomux_star;
  delete addrmux_star;
  delete addrbuf_star;
  delete const_stars;
  delete addrgen_star;
  delete mem_timing;

  delete port_timing;

  delete []datain_name;
  delete []dataout_name;
  delete []addr_name;
  delete []read_name;
  delete []write_name;
  delete []enable_name;

}

int MEMPort::init_pt(int seqlen)
{
  pt_count=seqlen;
  if (mem_timing!=NULL)
    {
      printf("MEMPort::init_pt:Error, multiple initialization\n");
      return(-1);
    }
  else
    mem_timing=new ACSIntArray(seqlen,UNKNOWN);

  // Return happy condition
  return(1);
}


int MEMPort::add_pt(int node_act,int node_type)
{
  mem_timing->set(node_act,node_type);

  // Return happy condition
  return(1);
}


int MEMPort::fetch_pt(int index)
{
  return(mem_timing->query(index));
}  


//////////////////////////////////////////////////////////////////////////////
// Each memory port is tasked to keep track of all source and sink stars that 
// are associated with it.  Additionally, the memory port will conduct 
// preliminary scheduling of all source stars.  
//////////////////////////////////////////////////////////////////////////////
int MEMPort::assign_srccore(ACSCGFPGACore* fpga_core)
{
  total_sgs++;
  if (portuse==UNUSED)
    {
      if (DEBUG_MEMPORT)
	printf("Port has not been used yet\n");
      
      // Port has not been used yet
      portuse=USED;
    }
  else
    {
      if (DEBUG_MEMPORT)
	printf("Port has been used, using next available clock period\n");
    }

  // Track sources
  source_stars->append((Pointer) fpga_core);

  // Assign activation time
  fpga_core->acs_addract=current_act;
  fpga_core->acs_dataact=current_act+read_skew;
      
  // Update next available time slot
  current_act++;
      
  // Return happy condition
  return(1);
}
int MEMPort::assign_snkcore(ACSCGFPGACore* fpga_core,int act)
{
  total_sgs++;
  if (portuse==UNUSED)
    {
      if (DEBUG_MEMPORT)
	printf("Port has not been used yet\n");
      
      // Port has not been used yet
      portuse=USED;
    }
  else
    {
      if (DEBUG_MEMPORT)
	printf("Port has been used, using next available clock period\n");
    }

  sink_stars->append((Pointer) fpga_core);
  
  // Assign activation time
  fpga_core->acs_addract=act-write_skew;
  fpga_core->acs_dataact=act;
      
      
  // Return happy condition
  return(1);
}


  

Pin::Pin()
{
  pin_count=0;
  major_bit=new ACSIntArray;
  vector_length=new ACSIntArray;
  min_vlength=new ACSIntArray;
  max_vlength=new ACSIntArray;
  word_lock=new ACSIntArray;
  prec_lock=new ACSIntArray;
  netlist_ids=new ACSIntArray;

  data_type=new ACSIntArray;
  pin_type=new ACSIntArray;
  pin_assigned=new ACSIntArray;
  pin_limit=new ACSIntArray;
  pin_priority=new ACSIntArray;

  connect=NULL;
  delays=NULL;
  pin_name=new StringArray;
  corona_pinname=new StringArray;
  inode_pins=NULL;
  onode_pins=NULL;
  cnode_pins=NULL;
}

Pin::~Pin() 
{
  delete major_bit;
  delete vector_length;
  delete min_vlength;
  delete max_vlength;
  delete word_lock;
  delete prec_lock;
  delete netlist_ids;

  delete data_type;
  delete pin_type;
  delete pin_assigned;
  delete pin_limit;
  delete pin_priority;

  for (int dloop=0;dloop<pin_count;dloop++)
    {
      delete connect[dloop];
      delete delays[dloop];
    }
  delete connect;
  delete delays;

  delete pin_name;
  delete corona_pinname;
  delete inode_pins;
  delete onode_pins;
  delete cnode_pins;
}


Pin& Pin::operator=(Pin& rh_pin)
{
  if (DEBUG_PIN)
    printf("Pin::operator= invoked=\n");
  
  pin_count=rh_pin.pin_count;

  major_bit->copy(rh_pin.major_bit);
  vector_length->copy(rh_pin.vector_length);
  min_vlength->copy(rh_pin.min_vlength);
  max_vlength->copy(rh_pin.max_vlength);
  word_lock->copy(rh_pin.word_lock);
  prec_lock->copy(rh_pin.prec_lock);
  netlist_ids->copy(rh_pin.netlist_ids);

  data_type->copy(rh_pin.data_type);
  pin_type->copy(rh_pin.pin_type);
  pin_assigned->copy(rh_pin.pin_assigned);
  pin_limit->copy(rh_pin.pin_limit);
  pin_priority->copy(rh_pin.pin_priority);

  connect=new Connectivity*[pin_count];
  delays=new Delay*[pin_count];
  pin_name=new StringArray;
  corona_pinname=new StringArray;
  inode_pins=new SequentialList;
  onode_pins=new SequentialList;
  cnode_pins=new SequentialList;
  for (int loop=0;loop<pin_count;loop++)
    {
      pin_name->add((rh_pin.pin_name)->get(loop));
      corona_pinname->add((rh_pin.corona_pinname)->get(loop));
      connect[loop]=rh_pin.connect[loop];
      delays[loop]=rh_pin.delays[loop];

      int ipin=(int) rh_pin.inode_pins->elem(loop+1);
      int opin=(int) rh_pin.inode_pins->elem(loop+1);
      int cpin=(int) rh_pin.inode_pins->elem(loop+1);
      inode_pins->append((Pointer) ipin);
      onode_pins->append((Pointer) opin);
      cnode_pins->append((Pointer) cpin);
    }
  return *this;
}


void Pin::update_pins()
{
  delete inode_pins;
  delete onode_pins;
  delete cnode_pins;
  
  inode_pins=classify_datapins(INPUT_PIN);
  onode_pins=classify_datapins(OUTPUT_PIN);
  cnode_pins=classify_controlpins();
}


SequentialList* Pin::classify_datapins(int find_type)
{
  SequentialList* result=new SequentialList;
  int alt_type=0;
  if ((find_type==INPUT_PIN) || (find_type==OUTPUT_PIN))
    alt_type=BIDIR_PIN;

  // Check for exceptions
  SequentialList* exceptions=new SequentialList;
  if (find_type==OUTPUT_PIN)
    {
      exceptions->append((Pointer) OUTPUT_PIN_WC);
      exceptions->append((Pointer) OUTPUT_PIN_MUX_RESULT);
      exceptions->append((Pointer) OUTPUT_PIN_MUX_SWITCHABLE);
    }
    
  
  for (int loop=0;loop<pin_count;loop++)
    if ((pin_type->query(loop)==find_type) || (pintype(loop)==alt_type))
      result->append((Pointer) loop);
    else
      if (exceptions->size()>0)
	for (int eloop=1;eloop<=exceptions->size();eloop++)
	  {
	    int exception=(int) exceptions->elem(eloop);
	    if (pin_type->query(loop)==exception)
	      result->append((Pointer) loop);
	  }

  // Cleanup
  delete exceptions;

  // Return unhappy condition
  return(result);
}


SequentialList* Pin::classify_controlpins()
{
  SequentialList* result=new SequentialList;
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("pin type=%d is",pin_type->query(loop));
      if ((pintype(loop)!=BIDIR_PIN) && 
	  (pin_type->query(loop)!=INPUT_PIN) &&
	  (pin_type->query(loop)!=OUTPUT_PIN) &&
	  (pin_type->query(loop)!=OUTPUT_PIN_MUX_RESULT) &&
	  (pin_type->query(loop)!=OUTPUT_PIN_MUX_SWITCHABLE) &&
	  (pin_type->query(loop)!=INPUT_PIN_MUX_SWITCHABLE) &&
	  (pin_type->query(loop)!=INPUT_PIN_MUX_RESULT))
	{
	  result->append((Pointer) loop);
	  if (DEBUG_PIN)
	    printf(" a contol pin\n");
	}
      else
	if (DEBUG_PIN)
	  printf(" not a contol pin\n");
    }
  // Return unhappy condition
  return(result);
}


// Add a new port definition given a port name, and port type
int Pin::add_pin(char* name, 
		 const int p_type)
{
  if (DEBUG_PIN)
    printf("Pin::add_pin(%s,%d)\n",name,p_type);

  new_pin();
  
  // Add new information
  pin_name->add(name);
  corona_pinname->add("NO_CORONA_PIN");
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(char* pname,
		 char* cpname,
		 const int p_type)
{
  if (DEBUG_PIN)
    printf("Pin::add_pin(%s,%s,%d)\n",pname,cpname,p_type);

  new_pin();
  
  // Add new information
  pin_name->add(pname);
  corona_pinname->add(cpname);
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(char* name, 
		 const int m_bit, 
		 const int v_len,
		 const int p_type)
{
  if (DEBUG_PIN)
    printf("Pin::add_pin(%s,%d,%d,%d)\n",name,m_bit,v_len,p_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  corona_pinname->add("NO_CORONA_PIN");
  major_bit->set(pin_count-1,m_bit);
  vector_length->set(pin_count-1,v_len);
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(char* name,
		 const int m_bit, 
		 const int v_len,
		 const int d_type, 
		 const int p_type)
{
  if (DEBUG_PIN)
    printf("Pin::add_pin(%s,%d,%d,%d,%d)\n",name,m_bit,v_len,d_type,p_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  corona_pinname->add("NO_CORONA_PIN");
  major_bit->set(pin_count-1,m_bit);
  vector_length->set(pin_count-1,v_len);
  pin_type->set(pin_count-1,p_type);
  data_type->set(pin_count-1,d_type);
  
  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}


// Generate a new port, and swap in old information
int Pin::new_pin()
{
  if (DEBUG_PIN)
    {
      printf("Pin::new_pin(), current pin_count=%d\n",pin_count);
      for (int loop=0;loop<pin_count;loop++)
	connect[loop]->dump();
    }

  // Develop a new temporary Pin
  int pcount=pin_count+1;

  // Add new field and assign default values to all fields
  major_bit->add(0);
  vector_length->add(0);
  min_vlength->add(0);
  max_vlength->add(INF);
  word_lock->add(UNLOCKED);
  prec_lock->add(UNLOCKED);
  netlist_ids->add(UNASSIGNED);

  data_type->add(UNASSIGNED);
  pin_type->add(UNASSIGNED);
  pin_assigned->add(UNASSIGNED);
  pin_limit->add(INF);
  pin_priority->add(NO_PRIORITY);

  Connectivity** tmp_con=new Connectivity*[pcount];
  Delay** tmp_delay=new Delay*[pcount];

  for (int loop=0;loop<pin_count;loop++)
    {
      tmp_con[loop]=connect[loop];
      tmp_delay[loop]=delays[loop];
    }
  tmp_con[pin_count]=new Connectivity;
  tmp_delay[pin_count]=new Delay;
  
  // Swap into correct instance
  pin_count=pcount;
  connect=tmp_con;
  delays=tmp_delay;

  // Return happy condition
  return(pin_count);
}


// Set a priority for ranking multiple requests for association
int Pin::set_pinpriority(const int index,
			 const int priority)
{
  if (DEBUG_PIN)
    printf("Pin::set_pinpriority(%d,%d)\n",index,priority);

  pin_priority->set(index,priority);

  // Return happy condition
  return(1);
}

// Set the maximum number of connections that can be made to this pin
int Pin::set_pinlim(const int index,
		    const int limit)
{
  if (DEBUG_PIN)
    printf("Pin::set_pinlim(%d,%d)\n",index,limit);

  pin_limit->set(index,limit);

  // Return happy condition
  return(1);
}

// Set the port type for the given port index
int Pin::set_pintype(const int index,
		     const int type)
{
  if (DEBUG_PIN)
    printf("Pin::set_pintype(%d,%d)\n",index,type);

  pin_type->set(index,type);

  // Update nodes
  update_pins();

  // Return happy condition
  return(1);
}


// Set the data type for the given port index
int Pin::set_datatype(const int index,
		      const int type)
{
  if (DEBUG_PIN)
    printf("Pin::set_datatype(%d,%d)\n",index,type);

  data_type->set(index,type);

  // Return happy condition
  return(1);
}

int Pin::set_min_vlength(const int index,
			 const int length)
{
  if (DEBUG_PIN)
    printf("Pin::set_min_vlength(%d,%d)\n",index,length);

  min_vlength->set(index,length);

  // Return happy condition
  return(1);
}

int Pin::set_max_vlength(const int index,
			 const int length)
{
  if (DEBUG_PIN)
    printf("Pin::set_max_vlength(%d,%d)\n",index,length);

  max_vlength->set(index,length);

  // Return happy condition
  return(1);
}

///////////////////////////////////////////////////////
// Set the absolute precision for the given port: index
///////////////////////////////////////////////////////
int Pin::set_precision(const int index,
		       const int m_bit,
		       const int v_len,
		       const int lock)
{
  if (DEBUG_PIN)
    printf("Pin::set_precision(%d,%d,%d,%d), old precision=%d,%d\n",
	   index,
	   m_bit,
	   v_len,
	   lock,
	   major_bit->query(index),
	   vector_length->query(index));

  major_bit->set(index,m_bit);
  vector_length->set(index,v_len);

  // Lock down this value after vectorlen was calculated
  prec_lock->set(index,lock);

  // Return happy condition
  return(1);
}


//////////////////////////////////////////////////////
// Set the maximum precision for the given port: index
// WARNING: Gaps will be spanned, this should only be
//          used for sourcing nodes!!
//////////////////////////////////////////////////////
int Pin::setmax_precision(const int index,
			  const int m_bit,
			  const int v_len,
			  const int lock)
{
  if (DEBUG_PIN)
    printf("Pin::setmax_precision(%d,%d,%d,%d), old precision=%d,%d\n",
	   index,
	   m_bit,
	   v_len,
	   lock,
	   major_bit->query(index),
	   vector_length->query(index));
  
  int prev_mbit=major_bit->query(index);
  int prev_vlen=vector_length->query(index);
  int new_mbit;
  int new_vlen;

  if (v_len==0)
    {
      printf("Pin::setmax_precision:Error, invalid! v_len==0\n");
      return(-1);
    }

  if ((prev_mbit==0) && (prev_vlen==0))
    {
      new_mbit=m_bit;
      new_vlen=v_len;
    }
  else
    {
      new_mbit=(int) max(m_bit,prev_mbit);
      int old_wordlen=prev_mbit-prev_vlen+1;
      int tmp_wordlen=m_bit-v_len+1;
      int new_wordlen=(int) min(old_wordlen,tmp_wordlen);
      new_vlen=new_mbit-new_wordlen+1;
    }
  
  // Update
  major_bit->set(index,new_mbit);
  vector_length->set(index,new_vlen);

  // Lock down this value after vectorlen was calculated
  if (lock==LOCKED)
    prec_lock->set(index,lock);

  if (DEBUG_PIN)
    printf("Pin::setmax_precision new precision=%d,%d\n",
	   major_bit->query(index),
	   vector_length->query(index));

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////////////////////////////
// Dictate for the Word Length analyzer whether, or not, the indexed pin
// can be modified (re: wordlengths)
////////////////////////////////////////////////////////////////////////
int Pin::set_wordlock(const int index,
		      const int lock_val)
{
  word_lock->set(index,lock_val);

  // Return happy condition
  return(1);
}

//////////////////////////////////////////////////////////
// Assign the netlist identifier for external associations
//////////////////////////////////////////////////////////
int Pin::set_netlistid(const int index,
		       const int netlist_id)
{
  netlist_ids->set(index,netlist_id);

  // Return happy condition
  return(1);
}

int Pin::query_pintype(const int index)
{
  return(pin_type->query(index));
}

int Pin::query_majorbit(const int index)
{
  return(major_bit->query(index));
}

int Pin::query_bitlen(const int index)
{
  return(vector_length->query(index));
}

int Pin::query_minbitlen(const int index)
{
  return(min_vlength->query(index));
}

int Pin::query_maxbitlen(const int index)
{
  return(max_vlength->query(index));
  return(0);
}

int Pin::query_wordlock(const int index)
{
  return(word_lock->query(index));
}

int Pin::query_preclock(const int index)
{
  return(prec_lock->query(index));
}

int Pin::query_netlistid(const int index)
{
  return(netlist_ids->query(index));
}

int Pin::query_pinassigned(const int index)
{
  return(pin_assigned->query(index));
}
			

char* Pin::retrieve_pinname(const int index)
{
  return(pin_name->get(index));
}

char* Pin::retrieve_cpinname(const int index)
{
  return(corona_pinname->get(index));
}

int Pin::prefix_string(const char* prfx_str)
{
  pin_name->prefix(prfx_str);
  return(1);
}


// Return the class of pin type a particular port instance is
int Pin::pintype(const int pin_no)
{
  if ((pin_type->query(pin_no) >= IPIN_MIN) && 
      (pin_type->query(pin_no) <= IPIN_MAX))
    return(INPUT_PIN);
  else if ((pin_type->query(pin_no) >= OPIN_MIN) && 
	   (pin_type->query(pin_no) <= OPIN_MAX))
    return(OUTPUT_PIN);
  else if ((pin_type->query(pin_no) >= BIDIR_MIN) && 
	   (pin_type->query(pin_no) <= BIDIR_MAX))
    return(BIDIR_PIN);
  else
    {
      if (DEBUG_PIN)
	printf("Pin::pintype:Warning, unknown pintype returned\n");
      return(UNKNOWN);
    }
}


int Pin::req_pintype(const int req_ptype)
{
  if ((req_ptype >= IPIN_MIN) && (req_ptype <= IPIN_MAX))
    return(INPUT_PIN);
  else if ((req_ptype >= OPIN_MIN) && (req_ptype <= OPIN_MAX))
    return(OUTPUT_PIN);
  else if ((req_ptype >= BIDIR_MIN) && (req_ptype <= BIDIR_MAX))
    return(BIDIR_PIN);
  else
    {
      if (DEBUG_PIN)
	printf("Pin::req_pintype:Warning, unknown pintype returned\n");
      return(UNKNOWN);
    }
}


// Return the first port number that matches the passed type
int Pin::retrieve_type(const int req_ptype)
{
  for (int loop=0;loop<pin_type->population();loop++)
    if (pin_type->query(loop)==req_ptype)
      return(loop);
  
  if (DEBUG_PIN)
    printf("Pin::retrieve_type:Warning, no matching type available\n");
  return(-1);
}


// Return the port number that matches the passed type, and the correct bit 
// value
int Pin::retrieve_type(const int req_ptype,
		       const int order)
{
  int order_count=0;
  for (int loop=0;loop<pin_type->population();loop++)
    if (pin_type->query(loop)==req_ptype)
      {
	if (order_count==order)
	  return(loop);
	else
	  order_count++;
      }
  
  if (DEBUG_PIN)
    printf("Pin::retrieve_type(order):Warning, no matching type available\n");
  return(-1);
}

void Pin::print_pins(char* title)
{
  printf("Pin::print_pins for %s\n",title);
  printf("Pin has %d instances\n",pin_count);
  for (int loop=0;loop<pin_count;loop++)
    {
      printf("pin_name(%d)=%s, pin_type=%d, Vector_length=%d, major_bit=%d\n",
	     loop,
	     pin_name->get(loop),
	     pin_type->query(loop),
	     vector_length->query(loop),
	     major_bit->query(loop));
      connect[loop]->dump();
    }
}

// Return the first UNASSIGNED node that matches the particular pintype
// Should utilize local connectivity, but for now it is an argument
// FIX:pin limit should be utilized here
int Pin::free_pintype(const int ptype)
{
  if (DEBUG_PIN)
    printf("Looking for free_pintype %d given %d pins\n",ptype,pin_count);
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("Testing pin %d, assignment state is %d, pin_limit=%d, pintype=%d\n",
	       loop,
	       pin_assigned->query(loop),
	       pin_limit->query(loop),
	       pin_type->query(loop));

      if ((pin_assigned->query(loop)==UNASSIGNED) && 
	  (pintype(loop)==INPUT_PIN))
	{
	  if (pin_type->query(loop)==ptype)
	    return(loop);
	}
      else if (pintype(loop)==OUTPUT_PIN)
	{
	  // Test for exceptions
	  if (ptype==OUTPUT_PIN_MUX_SWITCHABLE)
	    {
	      if ((pin_type->query(loop)==ptype) && 
		  (pin_assigned->query(loop)==UNASSIGNED))
		return(loop);
	    }
	  else if (pin_type->query(loop)==ptype)
	    return(loop);
	}
      else if (pintype(loop)==BIDIR_PIN)
	{
	  // Trap for input-type bidir pins
	  if (pin_type->query(loop)==BIDIR_PIN_MUX_SWITCHABLE)
	    {
	      if ((pin_assigned->query(loop)==UNASSIGNED) && 
		  ((ptype==INPUT_PIN) || (ptype==BIDIR_PIN_MUX_SWITCHABLE)))
		return(loop);
	    }
	  else if (pin_type->query(loop)==BIDIR_PIN_MUX_RESULT)
	    {
	      if ((pin_assigned->query(loop)==UNASSIGNED) && 
		  ((ptype==OUTPUT_PIN) || (ptype==BIDIR_PIN_MUX_RESULT)))
		return(loop);
	    }
	  else if (pin_type->query(loop)==ptype)
	    return(loop);
	}
    }
  if (DEBUG_PIN)
    printf("Pin::free_pintype:Warning, no matching & free type available\n");
  return(-1);
}

int Pin::connect_pin(const int pin_no,
		     const int acs_id,
		     const int dest_pin,
		     const int node_type)
{
  if (DEBUG_CONNECT)
    printf("pin_no(%d), pin_assigned(%d), drives acs(%d) via pin(%d)\n",
	   pin_no,
	   pin_assigned->query(pin_no),
	   acs_id,
	   dest_pin);

  if (pin_assigned->query(pin_no)==UNASSIGNED)
    {
      pin_assigned->set(pin_no,ASSIGNED);
      connect[pin_no]->add_node(acs_id,dest_pin,node_type);
    }
  else
    {
      if ((pintype(pin_no)==OUTPUT_PIN) ||
	  (pintype(pin_no)==BIDIR_PIN))
	connect[pin_no]->add_node(acs_id,dest_pin,node_type);
      else
	{
	  connect[pin_no]->assign_node(0,acs_id,pin_no,node_type);
	  if (DEBUG_PIN)
	    printf("Pin::connect_pin:Error? Overwriting existing connection\n");
	}
    }

  // Return happy condition
  return(1);
}

int Pin::connect_pin(const int pin_no,
		     const int acs_id,
		     const int dest_pin,
		     const int node_type,
		     const int signal)
{
  if (DEBUG_CONNECT)
    printf("(5arg connect_pin) pin_assigned for pin_no %d is %d\n",pin_no,
	   pin_assigned->query(pin_no));

  if (pin_assigned->query(pin_no)==UNASSIGNED)
    {
      int new_node=connect[pin_no]->add_node(acs_id,dest_pin,node_type);
      connect[pin_no]->set_pinsignal(new_node,signal);
      pin_assigned->set(pin_no,ASSIGNED);
    }
  else
    {
      if ((pintype(pin_no)==OUTPUT_PIN) ||
	  (pintype(pin_no)==BIDIR_PIN))
	{
	  int new_node=connect[pin_no]->add_node(acs_id,dest_pin,node_type);
	  connect[pin_no]->set_pinsignal(new_node,signal);
	}
      else
	{
	  int new_node=connect[pin_no]->add_node(acs_id,dest_pin,node_type);
	  connect[pin_no]->set_pinsignal(new_node,signal);
	  if (DEBUG_PIN)
	    printf("Pin::connect_pin:Error? Overwriting existing connection\n");
	}
    }

  // Return happy condition
  return(1);
}

int Pin::assign_pin(const int pin_no,
		    const int acs_id,
		    const int dest_node,
		    const int node_type)
{
  connect[pin_no]->assign_node(dest_node,acs_id,pin_no,node_type);

  // Return happy condition
  return(1);
}

int Pin::match_acstype(const int acsid,
		       const int find_type)
{
  int alt_type=0;
  if ((find_type==INPUT_PIN) || (find_type==OUTPUT_PIN))
    alt_type=BIDIR_PIN;

  for (int loop=0;loop<pin_count;loop++)
    if ((pin_type->query(loop)==find_type) || 
	(pin_type->query(loop)==alt_type))
      for (int cloop=0;cloop<connect[loop]->node_count;cloop++)
	if (connect[loop]->query_acsid(cloop)==acsid)
	  return(loop);
  
  // Return unhappy condition
  if (DEBUG_PIN)
    {
      printf("Pin::match_acstype:Warning, unable to find matching type\n");
      printf("acsid=%d, find_type=%d\n",acsid,find_type);
      printf("Choices were:\n");
      for (int loop=0;loop<pin_count;loop++)
	for (int cloop=0;cloop<connect[loop]->node_count;cloop++)
	  printf("pin %d, node %d, type=%d, acsid=%d\n",
		 loop,
		 cloop,
		 pin_type->query(loop),
		 connect[loop]->query_acsid(cloop));
    }
  return(-1);
}

int Pin::match_acsnode(const int acsid,
		       const int node)
{
  for (int cloop=0;cloop<connect[node]->node_count;cloop++)
    if (connect[node]->query_acsid(cloop)==acsid)
      return(cloop);

  // Return unhappy condition
  if (DEBUG_PIN)
    printf("Pin::match_acsnode:Warning, unable to find matching type\n");
  return(-1);
}


int Pin::disconnect_pin(const int pin_no,
			const int node_no)
{
  if (connect[pin_no]->node_count==1)
    pin_assigned->set(pin_no,UNASSIGNED);
  connect[pin_no]->remove_node(node_no);

  // Return happy condition
  return(1);
}


int Pin::disconnect_allpins(const int pin_no)
{
  pin_assigned->set(pin_no,UNASSIGNED);
  connect[pin_no]->remove_allnodes();
  
  // Return happy condition
  return(1);
}


/////////////////////////////////////////////////////////////////////
// Return a boolean condition if a given pin should be considered for
// mechanical growth analysis
/////////////////////////////////////////////////////////////////////
int Pin::bw_exclude()
{
  // If only source pins, then no updates are needed
  if (pin_count == onode_pins->size())
    return(1);

  // If all pins are locked then no updates are needed
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("testing pin %d, lock=%d\n",loop,prec_lock->query(loop));
      if (prec_lock->query(loop)==UNLOCKED)
	return(0);
    }
  return(1);
}

////////////////////////////////////////////////////////////
// Return the pin index, with the pin that is named ref_name
////////////////////////////////////////////////////////////
int Pin::pin_withname(const char* ref_name)
{
  for (int pin_no=0;pin_no<pin_count;pin_no++)
    {
      if (DEBUG_PIN)
	printf("phole %d = %s, comparing to %s\n",
	       pin_no,
	       retrieve_cpinname(pin_no),
	       ref_name);
	     
      if (strcmp(retrieve_cpinname(pin_no),ref_name)==0)
	return(pin_no);
    }

  if (DEBUG_PIN)
    printf("Pin::pin_withname:Warning, no pin named %s found\n",ref_name);
  return(-1);
}


///////////////////////////////////////////////////////////////////////////
// Returns the pin index, with the pin whose netlist_id matches ref_netlist
///////////////////////////////////////////////////////////////////////////
int Pin::pin_withnetlist(const int ref_netlist)
{
  for (int pin_no=0;pin_no<pin_count;pin_no++)
    {
      if (DEBUG_PIN)
	  printf("pin(%d):netlist_id=%d, comparing to %d\n",
		 pin_no,
		 netlist_ids->query(pin_no),
		 ref_netlist);
      if (netlist_ids->query(pin_no)==ref_netlist)
	return(pin_no);
    }
  
  printf("Pin::pin_withnetlist:Error, no pin is associated with netlist %d\n",
	 ref_netlist);
  return(-1);
}

///////////////////////////////////////////////////////////
// Remaps the first pin of pin_type orig_type with new_type
///////////////////////////////////////////////////////////
int Pin::reclassify_pin(const int orig_type,
			const int new_type)
{
  for (int pin_no=0;pin_no<pin_count;pin_no++)
    {
      if (DEBUG_PIN)
	printf("phole %d, type = %d, looking for %d\n",
	       pin_no,
	       pin_type->query(pin_no),
	       orig_type);
      if (pin_type->query(pin_no)==orig_type)
	set_pintype(pin_no,new_type);
    }

  // Return happy condition
  return(1);
}



Capability::Capability()
{
  domains=new StringList;
  architectures=new StringList;
  languages=new SequentialList;
  assert(domains);
  assert(architectures);
  assert(languages);
}

Capability::~Capability()
{
  delete domains;
  delete architectures;
  delete languages;
}

Capability& Capability::operator=(const Capability& rh_cap)
{
  char* tmp_name=new char[MAX_STR];
  
  if ((rh_cap.domains)->numPieces()!=0)
    for (int loop=1;loop<=(rh_cap.domains)->numPieces();loop++)
      {
	strcpy(tmp_name,retrieve_string(rh_cap.domains,loop));
	*domains << tmp_name;
	strcpy(tmp_name,retrieve_string(rh_cap.architectures,loop));
	*architectures << tmp_name;
	
	for (int copy_loop=1;copy_loop<languages->size();copy_loop++)
	  languages->append((rh_cap.languages)->elem(copy_loop));
      }

  // Cleanup
  delete []tmp_name;

  return *this;
}

char* Capability::retrieve_string(StringList* the_list,int index)
{
  StringListIter stringIter(*the_list);
  char* entry_name;
  int count=0;

  while ((entry_name=(char*) stringIter++)!=NULL)
    {
      count++;
      if (count==index)
	return(entry_name);
    }

  if (DEBUG)
    printf("Capability::retrieve_string:Warning, unable to find matching string\n");
  return(NULL);
}

int Capability::add_domain(char* domain_name)
{
  *domains << domain_name;

  // Return happy condition
  return(1);
}
int Capability::add_architecture(char* arch_name)
{
  *architectures << arch_name;

  // Return happy condition
  return(1);
}
int Capability::add_language(int language)
{
  languages->append((Pointer) language);

  // Return happy condition
  return(1);
}
int Capability::get_language(int index)
{
  if (index < languages->size())
    return((int) languages->elem(index));
  else
    {
      if (DEBUG)
	printf("Capability::get_language:Error, invalid index provided\n");
      return(-1);
    }
}


// Constants class 
Constants::Constants(void)
{
  count=0;
  storage=NULL;
  types=new ACSIntArray;
}
Constants::~Constants(void)
{
  for (int loop=0;loop<count;loop++)
    delete storage[loop];
  delete storage;
  delete types;
  count=0;
}

void Constants::add_double(double value)
{
  add(&value,CDOUBLE);
}

void Constants::add_float(float value)
{
  add(&value,CFLOAT);
}

void Constants::add_long(long value)
{
  add(&value,CLONG);
}

void Constants::add_int(int value)
{
  add(&value,CINT);
}

void Constants::add(void* value,const int type)
{
  void* new_ptr=NULL;

  switch(type)
    {
    case CINT:
      new_ptr=(void *) new int;
      memcpy(new_ptr,value,sizeof(int));
      break;
    case CLONG:
      new_ptr=(void *) new long;
      memcpy(new_ptr,value,sizeof(long));      
      break;
    case CFLOAT:
      new_ptr=(void *) new float;
      memcpy(new_ptr,value,sizeof(float));      
      break;
    case CDOUBLE:
      new_ptr=(void *) new double;
      memcpy(new_ptr,value,sizeof(double));      
      break;
    }

  add_storage(new_ptr);
  types->add(type);

  if (DEBUG_CONST)
    printf("Constants: count=%d\n",count);
}

void Constants::add_storage(void* add_ptr)
{
  void** new_ptr=new void *[count+1];
  
  for (int loop=0;loop<count;loop++)
    new_ptr[loop]=storage[loop];

  new_ptr[count]=add_ptr;
  storage=new_ptr;

  count++;
}

int Constants::get_int(int index)
{
  return(*((int *) storage[index]));
}

long Constants::get_long(int index)
{
  return(*((long *) storage[index]));
}

float Constants::get_float(int index)
{
  return(*((float *) storage[index]));
}

double Constants::get_double(int index)
{
  return(*(double *) storage[index]);
}

// Return a constant value in either string format in '' or
// in numeric format.  Particular value is selected by an index
char* Constants::query_bitstr(int index,
			      int major_bit,
			      int bitlen)
{
  int indexed_type=types->query(index);

  // Breakup the indexed value into an integer and fractional component
  double value=0.0;
  switch (indexed_type)
    {
    case CINT:
      value=(double) get_int(index);
      break;
    case CLONG:
      value=(double) get_long(index);
      break;
    case CFLOAT:
      value=(double) get_float(index);
      break;
    case CDOUBLE:
      value=get_double(index);
      break;
    }
  if (DEBUG_CONST)
    printf("Extracted constant %d w/val %f, convert to (%d,%d)\n",
	   index,
	   value,
	   major_bit,
	   bitlen);

  // Convert magnitude to binary representation
  double abs_value=(long) abs(value);
  int cur_bit=major_bit;
  int bit_ptr=bitlen-1;
  int* bit_array=new int[bitlen];
  while (bit_ptr>=0)
    {
      double power=pow(2.0,(double) cur_bit);
      if (DEBUG_CONST)
	printf("abs_value=%f, power=%f\n",abs_value,power);
      if (abs_value >= power)
	{
	  bit_array[bit_ptr]=1;
	  abs_value-=power;
	}
      else
	bit_array[bit_ptr]=0;
      cur_bit--;
      bit_ptr--;
    }

  // Convert to 2's complement if negative value
  if (value<0)
    {
      if (DEBUG_CONST)
	printf("Constants::query_str:Converting negative value\n");

      // Invert bits
      for (int loop=0;loop<bitlen;loop++)
	if (bit_array[loop]==1)
	  bit_array[loop]=0;
	else
	  bit_array[loop]=1;
      
      // Add 1
      int carry=1;
      int bit=0;
      while (carry)
	if (bit_array[bit]==1)
	  {
	    bit_array[bit]=0;
	    bit++;
	    
	    //FIX: This should set an error for insufficient bits
	    if (bit==bitlen)
	      carry=0;
	  }
	else
	  {
	    bit_array[bit]=1;
	    carry=0;
	  }
    }

  // Generate character version
  ostrstream tmp_str;
  for (int loop=bitlen-1;loop>=0;loop--)
    if (bit_array[loop]==1)
      tmp_str << "1";
    else
      tmp_str << "0";

  tmp_str << ends;
  return(tmp_str.str());
}

// Return a constant value in string format
// Particular value is selected by an index
char* Constants::query_str(int index,
			   int majorbits,
			   int bitlen)
{
  ostrstream tmp_str;
  char* tmp_valstr=new char[MAX_STR];
  char* format_str=new char[MAX_STR];
  
  int indexed_type=types->query(index);
  int queried_int=0;
  long queried_long=0;
  float queried_float=0.0;
  double queried_double=0.0;

  // If applicable
  if ((indexed_type==CFLOAT) || (indexed_type==CDOUBLE))
    {
      int msd=1;
      int lsd=2;
      if (majorbits > 1)
	msd=majorbits;

      int delta_bits=majorbits+1-bitlen;
      if (delta_bits<-2)
	lsd=-1*delta_bits;
      
      // FIX:There is probably a more clever way of doing this:
      ostrstream format;
      format << "%" << msd << "." << lsd << "f" << ends;
      strcpy(format_str,format.str());
    }

  switch (indexed_type)
    {
    case CINT:
      queried_int=get_int(index);
      sprintf(tmp_valstr,"%d",queried_int);
      break;
    case CLONG:
      queried_long=get_long(index);
      sprintf(tmp_valstr,"%ld",queried_long);
      break;
    case CFLOAT:
      queried_float=get_float(index);
      sprintf(tmp_valstr,format_str,queried_float);
      break;
    case CDOUBLE:
      queried_double=get_double(index);
      sprintf(tmp_valstr,format_str,queried_double);
      break;
    }

  tmp_str << tmp_valstr << ends;
  delete []tmp_valstr;
  delete []format_str;
  return(tmp_str.str());
}


int Constants::query_bitsize(int index)
{
  int indexed_type=types->query(index);
  long value=0;
  switch (indexed_type)
    {
    case CINT:
      value=get_int(index);
      break;
    case CLONG:
      value=get_long(index);
      break;
    case CFLOAT:
      value=0;
      break;
    case CDOUBLE:
      value=0;
      break;
    }
  
  // log2(x)=log(x)/log(2)

#ifdef PTHPPA_CFRONT
  long abs_value=(long) abs((double)value);
#else
  long abs_value=(long) abs(value);
#endif
  int bit_count=0;
  if (abs_value==0)
    bit_count=1;
  else
    bit_count=(int) ceil(log(abs_value+1.0)/log(2.0));

  if (value<0)
    bit_count++;

  // FIX: For now assume that these constants are signed
  bit_count++;

  if (DEBUG_CONST)
    printf("Converted value %ld to bit count %d\n",
	   value,
	   bit_count);
  return(bit_count);
}


Sequencer::Sequencer(void)
{
  seq_sg=NULL;
  wc_consts=new SequentialList;
  wc_mux=NULL;
  wc_add=NULL;
  //  addr_consts=new SequentialList;
  //  addr_mux=NULL;
  //  addr_add=NULL;
}

Sequencer::~Sequencer(void)
{
  delete seq_sg;
  delete wc_consts;
  delete wc_mux;
  delete wc_add;
  //delete addr_consts;
  //delete addr_mux;
  //delete addr_add;
}






Resource::Resource()
{
  row=-1;
  col=-1;
}

Resource::~Resource()
{
}

int Resource::set_occupancy(int row_arg,int col_arg)
{
  row=row_arg;
  col=col_arg;

  // Return happy condition
  return(1);
}

int Resource::get_occupancy(int row_ptr,int col_ptr)
{
  row_ptr=row;
  col_ptr=col;

  // Return happy condition
  return(1);
}




