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
 Version: @(#)Pin.cc      1.0     06/16/99
***********************************************************************/
#include "Pin.h"
Pin::Pin() 
: pin_count(0), pin_directions(NULL), connect(NULL), delays(NULL)
{
  major_bit=new ACSIntArray;
  vector_length=new ACSIntArray;
  word_count=new ACSIntArray;
  min_vlength=new ACSIntArray;
  max_vlength=new ACSIntArray;
  word_lock=new ACSIntArray;
  prec_lock=new ACSIntArray;
  netlist_ids=new ACSIntArray;

  skip_pin=new ACSIntArray;
  data_type=new ACSIntArray;
  pin_type=new ACSIntArray;
  pin_assertion=new ACSIntArray;
  pin_assigned=new ACSIntArray;
  pin_limit=new ACSIntArray;
  pin_priority=new ACSIntArray;
  phase_dependency=new ACSIntArray;

  pin_name=new StringArray;
  alt_pinname=new StringArray;
  altpin_flag=new ACSIntArray;
  corona_pinname=new StringArray;

  inode_pins=new ACSIntArray;
  onode_pins=new ACSIntArray;
  icnode_pins=new ACSIntArray;
  ocnode_pins=new ACSIntArray;
}

Pin::~Pin() 
{
  delete major_bit;
  delete vector_length;
  delete word_count;
  delete min_vlength;
  delete max_vlength;
  delete word_lock;
  delete prec_lock;
  delete netlist_ids;

  delete skip_pin;
  delete data_type;
  delete pin_type;
  delete pin_assertion;
  delete pin_assigned;
  delete pin_limit;
  delete pin_priority;
  delete phase_dependency;

  for (int dloop=0;dloop<pin_count;dloop++)
    {
      if (pin_directions!=NULL)
	delete pin_directions[dloop];
      if (connect!=NULL)
	delete connect[dloop];
      if (delays!=NULL)
	delete delays[dloop];
    }
  delete pin_directions;
  delete connect;
  delete delays;

  delete pin_name;
  delete alt_pinname;
  delete altpin_flag;
  delete corona_pinname;
  delete inode_pins;
  delete onode_pins;
  delete icnode_pins;
  delete ocnode_pins;
}


Pin& Pin::operator=(Pin& rh_pin)
{
  if (DEBUG_PIN_OPERATOR)
    printf("Pin::operator= invoked=\n");
  
  pin_count=rh_pin.pin_count;

  major_bit->copy(rh_pin.major_bit);
  vector_length->copy(rh_pin.vector_length);
  word_count->copy(rh_pin.word_count);
  min_vlength->copy(rh_pin.min_vlength);
  max_vlength->copy(rh_pin.max_vlength);
  word_lock->copy(rh_pin.word_lock);
  prec_lock->copy(rh_pin.prec_lock);
  netlist_ids->copy(rh_pin.netlist_ids);

  skip_pin->copy(rh_pin.skip_pin);
  data_type->copy(rh_pin.data_type);
  pin_type->copy(rh_pin.pin_type);
  pin_assertion->copy(rh_pin.pin_assertion);
  pin_assigned->copy(rh_pin.pin_assigned);
  pin_limit->copy(rh_pin.pin_limit);
  pin_priority->copy(rh_pin.pin_priority);
  phase_dependency->copy(rh_pin.phase_dependency);

  pin_directions=new ACSIntArray*[pin_count];
  pin_name=new StringArray;
  alt_pinname=new StringArray;
  altpin_flag=new ACSIntArray;
  corona_pinname=new StringArray;
  connect=new Connectivity*[pin_count];
  delays=new Delay*[pin_count];
  for (int loop=0;loop<pin_count;loop++)
    {
      pin_directions[loop]=rh_pin.pin_directions[loop];
      pin_name->add((rh_pin.pin_name)->get(loop));
      alt_pinname->add((rh_pin.alt_pinname)->get(loop));
      corona_pinname->add((rh_pin.corona_pinname)->get(loop));
      connect[loop]=rh_pin.connect[loop];
      delays[loop]=rh_pin.delays[loop];
    }

  altpin_flag->copy(rh_pin.altpin_flag);
  inode_pins=rh_pin.inode_pins;
  onode_pins=rh_pin.onode_pins;
  icnode_pins=rh_pin.icnode_pins;
  ocnode_pins=rh_pin.ocnode_pins;

  return *this;
}

void Pin::copy_pin(Pin* source_pins, const int index)
{
  int new_pin=add_pin(source_pins->query_pinname(index),
		      source_pins->query_majorbit(index),
		      source_pins->query_bitlen(index),
		      source_pins->query_datatype(index),
		      source_pins->query_pintype(index),
		      source_pins->query_pinassertion(index));
  set_pinpriority(new_pin,source_pins->query_pinpriority(index));
}
void Pin::copy_pins(Pin* source_pins)
{
  for (int loop=0;loop<source_pins->query_pincount();loop++)
    {
      int new_pin=add_pin(source_pins->query_pinname(loop),
			  source_pins->query_majorbit(loop),
			  source_pins->query_bitlen(loop),
			  source_pins->query_datatype(loop),
			  source_pins->query_pintype(loop),
			  source_pins->query_pinassertion(loop));
      set_pinpriority(new_pin,source_pins->query_pinpriority(loop));
    }
}
void Pin::copy_pins(Pin* source_pins, ACSIntArray* pin_list)
{
  for (int loop=0;loop<pin_list->population();loop++)
    {
      int index=pin_list->query(loop);
      int new_pin=add_pin(source_pins->query_pinname(index),
			  source_pins->query_majorbit(index),
			  source_pins->query_bitlen(index),
			  source_pins->query_datatype(index),
			  source_pins->query_pintype(index),
			  source_pins->query_pinassertion(index));
      set_pinpriority(new_pin,source_pins->query_pinpriority(index));
    }
}


void Pin::update_pins()
{
  delete inode_pins;
  delete onode_pins;
  delete icnode_pins;
  delete ocnode_pins;
  
  inode_pins=classify_datapins(INPUT_PIN);
  onode_pins=classify_datapins(OUTPUT_PIN);
  icnode_pins=classify_controlpins(INPUT_PIN);
  ocnode_pins=classify_controlpins(OUTPUT_PIN);
}


ACSIntArray* Pin::classify_datapins(const int find_type)
{
  ACSIntArray* result=new ACSIntArray;
  int alt_type=0;
  if ((find_type==INPUT_PIN) || (find_type==OUTPUT_PIN))
    alt_type=BIDIR_PIN;

  // Check for exceptions
  ACSIntArray* exceptions=new ACSIntArray;
  if (find_type==OUTPUT_PIN)
    {
      exceptions->add(OUTPUT_PIN_WC);
      exceptions->add(OUTPUT_PIN_SRC_WC);
      exceptions->add(OUTPUT_PIN_SNK_WC);
      exceptions->add(OUTPUT_PIN_DELAY_WC);
      exceptions->add(OUTPUT_PIN_MUX_RESULT);
      exceptions->add(OUTPUT_PIN_MUX_SWITCHABLE);
    }
    
  
  for (int loop=0;loop<pin_count;loop++)
    if ((query_pintype(loop)==find_type) || (pin_classtype(loop)==alt_type))
      result->add(loop);
    else
      if (exceptions->population()>0)
	for (int eloop=0;eloop<exceptions->population();eloop++)
	  {
	    int exception=exceptions->query(eloop);
	    if (query_pintype(loop)==exception)
	      result->add(loop);
	  }

  // Cleanup
  delete exceptions;

  // Return unhappy condition
  return(result);
}


ACSIntArray* Pin::classify_controlpins(const int type)
{
  ACSIntArray* result=new ACSIntArray;
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("pin type=%d is",query_pintype(loop));
      if ((pin_classtype(loop)!=BIDIR_PIN) && 
	  (query_pintype(loop)!=INPUT_PIN) &&
	  (query_pintype(loop)!=OUTPUT_PIN) &&
	  (query_pintype(loop)!=OUTPUT_PIN_MUX_RESULT) &&
	  (query_pintype(loop)!=OUTPUT_PIN_MUX_SWITCHABLE) &&
	  (query_pintype(loop)!=INPUT_PIN_MUX_SWITCHABLE) &&
	  (query_pintype(loop)!=INPUT_PIN_MUX_RESULT))
	{
	  if (DEBUG_PIN)
	    printf(" a contol pin\n");
	  if ((pin_type->query(loop) >= IPIN_MIN) && (pin_type->query(loop) <= IPIN_MAX) 
	      && (type==INPUT_PIN))
	    result->add(loop);
	  if ((pin_type->query(loop) >= OPIN_MIN) && (pin_type->query(loop) <= OPIN_MAX) 
	      && (type==OUTPUT_PIN))
	    result->add(loop);
	}
      else
	if (DEBUG_PIN)
	  printf(" not a contol pin\n");
    }
  // Return unhappy condition
  return(result);
}


// Add a new port definition given a port name, and port type
int Pin::add_pin(const char* name, 
		 const int p_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d)\n",name,p_type);

  new_pin();
  
  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add("NO_CORONA_PIN");
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* name, 
		 const int p_type,
		 const char a_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d,%d)\n",name,p_type,a_type);

  new_pin();
  
  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add("NO_CORONA_PIN");
  pin_type->set(pin_count-1,p_type);
  pin_assertion->set(pin_count-1,a_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* pname,
		 const char* cpname,
		 const int p_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%s,%d)\n",pname,cpname,p_type);

  new_pin();
  
  // Add new information
  pin_name->add(pname);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add(cpname);
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* name, 
		 const int m_bit, 
		 const int v_len,
		 const int p_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d,%d,%d)\n",name,m_bit,v_len,p_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add("NO_CORONA_PIN");
  major_bit->set(pin_count-1,m_bit);
  vector_length->set(pin_count-1,v_len);
  pin_type->set(pin_count-1,p_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* name, 
		 const int m_bit, 
		 const int v_len,
		 const int p_type,
		 const char a_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d,%d,%d,%d)\n",name,m_bit,v_len,p_type,a_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add("NO_CORONA_PIN");
  major_bit->set(pin_count-1,m_bit);
  vector_length->set(pin_count-1,v_len);
  pin_type->set(pin_count-1,p_type);
  pin_assertion->set(pin_count-1,a_type);

  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* name,
		 const int m_bit, 
		 const int v_len,
		 const int d_type, 
		 const int p_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d,%d,%d,%d)\n",name,m_bit,v_len,d_type,p_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
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
int Pin::add_pin(const char* name,
		 const int m_bit, 
		 const int v_len,
		 const int d_type, 
		 const int p_type,
		 const char a_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%d,%d,%d,%d,%d)\n",name,m_bit,v_len,d_type,p_type,a_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  alt_pinname->add("NO_ALTERNATE_PIN_NAME");
  altpin_flag->add(0);
  corona_pinname->add("NO_CORONA_PIN");
  major_bit->set(pin_count-1,m_bit);
  vector_length->set(pin_count-1,v_len);
  pin_type->set(pin_count-1,p_type);
  pin_assertion->set(pin_count-1,a_type);
  data_type->set(pin_count-1,d_type);
  
  // Update nodes
  update_pins();
  
  // Return happy condition
  return(pin_count-1);
}
int Pin::add_pin(const char* name,
		 const char* alt_name,
		 const int m_bit, 
		 const int v_len,
		 const int d_type, 
		 const int p_type)
{
  if (DEBUG_PIN_ADD)
    printf("Pin::add_pin(%s,%s,%d,%d,%d,%d)\n",name,alt_name,
	   m_bit,v_len,d_type,p_type);
  
  new_pin();

  // Add new information
  pin_name->add(name);
  alt_pinname->add(alt_name);
  altpin_flag->add(1);
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
	{
	  printf("Pin %d connections:\n",loop);
	  connect[loop]->dump();
	}
      print_pinclasses();
    }

  // Develop a new temporary Pin
  int pcount=pin_count+1;

  // Add new field and assign default values to all fields
  major_bit->add(0);
  vector_length->add(0);
  word_count->add(0);
  min_vlength->add(0);
  max_vlength->add(INF);
  word_lock->add(UNLOCKED);
  prec_lock->add(UNLOCKED);
  netlist_ids->add(UNASSIGNED);

  skip_pin->add(0);
  data_type->add(UNASSIGNED);
  pin_type->add(UNASSIGNED);
  pin_assertion->add(UNASSIGNED);
  pin_assigned->add(UNASSIGNED);
  pin_limit->add(INF);
  pin_priority->add(NO_PRIORITY);
  phase_dependency->add(UNASSIGNED);
  
  ACSIntArray** tmp_pindir=new ACSIntArray*[pcount];
  Connectivity** tmp_con=new Connectivity*[pcount];
  Delay** tmp_delay=new Delay*[pcount];

  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("Pin::new_pin() reassigning connectivity %d\n",loop);
      tmp_pindir[loop]=pin_directions[loop];
      tmp_con[loop]=connect[loop];
      tmp_delay[loop]=delays[loop];
    }
  // Cleanup old lists
  delete pin_directions;
  delete connect;
  delete delays;

  tmp_pindir[pin_count]=new ACSIntArray;
  tmp_con[pin_count]=new Connectivity;
  tmp_delay[pin_count]=new Delay;
  
  // Swap into correct instance
  pin_count=pcount;
  pin_directions=tmp_pindir;
  connect=tmp_con;
  delays=tmp_delay;

  if (DEBUG_PIN)
    {
      printf("Pin::new_pin() complete, current pin_count=%d\n",pin_count);
      for (int loop=0;loop<pin_count;loop++)
	{
	  printf("Pin %d connections:\n",loop);
	  connect[loop]->dump();
	}
      print_pinclasses();
    }
  // Return happy condition
  return(pin_count);
}

int Pin::change_pinname(const int index,
			const char* new_name)
{
  if (DEBUG_PIN)
    printf("Pin::change_pinname(%d,%s)\n",index,new_name);

  pin_name->set(index,new_name);

  // Return happy condition
  return(1);
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

// Set a particular control pin's phase dependency on either an INPUT or OUTPUT_PIN
int Pin::set_phasedependency(const int index,
			     const int dependency)
{
  if (DEBUG_PIN)
    printf("Pin::set_phasedependency(%d,%d)\n",index,dependency);

  phase_dependency->set(index,dependency);

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

// Set the pin assertion level
int Pin::set_pinassertion(const int index,
			  const int assertion_level)
{
  if (DEBUG_PIN)
    printf("Pin::set_pinassertion(%d,%d)\n",index,assertion_level);

  pin_assertion->set(index,assertion_level);

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////////////
// Set the directionality on a per-bit basis for the pin
// FIX:Obviously, some coding is needed here;)
////////////////////////////////////////////////////////
int Pin::set_directionality(const int pin_no,
			    const int bit,
			    const int direction)
{

  // Return happy condition
  return(1);
}

int Pin::set_skip(const int index, const int state)
{
  if (DEBUG_PIN)
    printf("Pin::set_skip(%d,%d)\n",index,state);

  skip_pin->set(index,state);

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

int Pin::set_wordcount(const int index,
		       const int count)
{
  if (DEBUG_PIN)
    printf("Pin::set_wordcount(%d,%d)\n",index,count);
  
  word_count->set(index,count);

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
  if (DEBUG_PIN_PRECISION)
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


// Directly set the precision lock status on an individual pin
int Pin::set_preclock(const int index,
		      const int lock)
{
  // Lock down this value
  prec_lock->set(index,lock);

  // Return happy condition
  return(1);
}

///////////////////////////////////////////////////////////////
// Match the precision between a reference pin and a target pin
///////////////////////////////////////////////////////////////
int Pin::match_precision(const int ref_pin,
			 const int target_pin)
{
  int mbit=query_majorbit(ref_pin);
  int bitl=query_bitlen(ref_pin);
  int lock=query_preclock(ref_pin);
  set_precision(target_pin,mbit,bitl,lock);

  // Return happy condition
  return(1);
}
int Pin::match_precision(Pin* src_pin,
			 const int ref_pin,
			 const int target_pin)
{
  int mbit=src_pin->query_majorbit(ref_pin);
  int bitl=src_pin->query_bitlen(ref_pin);
  int lock=src_pin->query_preclock(ref_pin);
  set_precision(target_pin,mbit,bitl,lock);

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
  if (DEBUG_PIN_PRECISION)
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

  if (DEBUG_PIN_PRECISION)
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

///////////////////////////////
// Lock all pin precisions down
///////////////////////////////
int Pin::set_alllocks(void)
{
  for (int loop=0;loop<pin_count;loop++)
    word_lock->set(loop,LOCKED);

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


/////////////////////////////////
// Accessors to protected members
/////////////////////////////////
int Pin::query_pincount(void)
{
  return(pin_count);
}

int Pin::query_majorbit(const int index)
{
  return(major_bit->query(index));
}

int Pin::query_bitlen(const int index)
{
  return(vector_length->query(index));
}

int Pin::query_wordcount(const int index)
{
  return(word_count->query(index));
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

int Pin::query_skip(const int index)
{
  return(skip_pin->query(index));
}

int Pin::query_datatype(const int index)
{
  return(data_type->query(index));
}

int Pin::query_pintype(const int index)
{
  return(pin_type->query(index));
}

int Pin::query_pinassertion(const int index)
{
  return(pin_assertion->query(index));
}

int Pin::query_pinassigned(const int index)
{
  return(pin_assigned->query(index));
}

int Pin::query_pinlimit(const int index)
{
  return(pin_limit->query(index));
}

int Pin::query_pinpriority(const int index)
{
  return(pin_priority->query(index));
}

int Pin::query_phasedependency(const int index)
{
  return(phase_dependency->query(index));
}

char* Pin::query_pinname(const int index)
{
  return(pin_name->get(index));
}

char* Pin::query_altpinname(const int index)
{
  return(alt_pinname->get(index));
}

int Pin::query_altpinflag(const int index)
{
  return(altpin_flag->get(index));
}

char* Pin::query_cpinname(const int index)
{
  return(corona_pinname->get(index));
}

Connectivity* Pin::query_connection(const int index)
{
  if ((index > pin_count) || (index < 0))
    {
      printf("Error:invalid access to connectivity handle %d of %d\n",index,pin_count);
      return(NULL);
    }
  return(connect[index]);
}

Delay* Pin::query_delay(const int index)
{
  if ((index > pin_count) || (index < 0))
    {
      printf("Error:invalid access to delay handle %d of %d\n",index,pin_count);
      return(NULL);
    }
  return(delays[index]);
}

ACSIntArray* Pin::query_inodes(void)
{
  return(inode_pins);
}
ACSIntArray* Pin::query_onodes(void)
{
  return(onode_pins);
}
ACSIntArray* Pin::query_icnodes(void)
{
  return(icnode_pins);
}
ACSIntArray* Pin::query_ocnodes(void)
{
  return(ocnode_pins);
}




// Return the class of pin type a particular port instance is
int Pin::pin_classtype(const int pin_no)
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
	printf("Pin::pin_classtype:Warning, unknown pintype returned\n");
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


// Return the port number that matches the passed type, and the correct priority
int Pin::retrieve_type(const int req_ptype,
		       const int priority)
{
  for (int loop=0;loop<pin_type->population();loop++)
    if (pin_type->query(loop)==req_ptype)
      {
	if (priority==pin_priority->query(loop))
	  return(loop);
      }
  
  if (DEBUG_PIN)
    printf("Pin::retrieve_type(order):Warning, no matching type available\n");
  return(-1);
}

void Pin::print_pins(const char* title)
{
  printf("Pin::print_pins for %s\n",title);
  printf("Pin has %d instances\n",pin_count);
  for (int loop=0;loop<pin_count;loop++)
    {
      printf("pin_name(%d)=%s, pin_type=%d, pin_assertion=%d, vector_length=%d, major_bit=%d, word_count=%d\n",
	     loop,
	     pin_name->get(loop),
	     pin_type->query(loop),
	     pin_assertion->query(loop),
	     vector_length->query(loop),
	     major_bit->query(loop),
	     word_count->query(loop));
      connect[loop]->dump();
    }
}

void Pin::print_pinclasses(void)
{
  printf("Pin::print_pinclasses are\n");
  printf("inodes are : ");
  for (int loop=0;loop<inode_pins->population();loop++)
    printf("%d ",inode_pins->query(loop));
  printf("\nonodes are : ");
  for (int loop=0;loop<onode_pins->population();loop++)
    printf("%d ",onode_pins->query(loop));
  printf("\nicnodes are : ");
  for (int loop=0;loop<icnode_pins->population();loop++)
    printf("%d ",icnode_pins->query(loop));
  printf("\nocnodes are : ");
  for (int loop=0;loop<ocnode_pins->population();loop++)
    printf("%d ",ocnode_pins->query(loop));
  printf("\n");
}

// Return the first UNASSIGNED node that matches the particular pintype
// Should utilize local connectivity, but for now it is an argument
// FIX:pin limit should be utilized here
int Pin::free_pintype(const int ptype)
{
  return(free_pintype(ptype,-1));
}
int Pin::free_pintype(const int ptype,
		      const int priority)
{
  if (DEBUG_PIN)
    printf("Looking for free_pintype %d, priority %d given %d pins\n",
	   ptype,
	   priority,
	   pin_count);
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN)
	printf("Testing pin %d, assignment state is %d, priority=%d, pin_limit=%d, pintype=%d\n",
	       loop,
	       pin_assigned->query(loop),
	       pin_priority->query(loop),
	       pin_limit->query(loop),
	       pin_type->query(loop));

      if ((pin_assigned->query(loop)==UNASSIGNED) &&
	  (pin_priority->query(loop)==priority))
	{
	  switch (pin_classtype(loop))
	    {
	    case INPUT_PIN:
	      if (pin_type->query(loop)==ptype)
		return(loop);
	      break;
	    case OUTPUT_PIN:
	      // Test for exceptions
	      if (ptype==OUTPUT_PIN_MUX_SWITCHABLE)
		{
		  if (pin_type->query(loop)==ptype)
		    return(loop);
		}
	      else if (pin_type->query(loop)==ptype)
		return(loop);
	      break;
	    case BIDIR_PIN:
	      // Trap for input-type bidir pins
	      if (pin_type->query(loop)==BIDIR_PIN_MUX_SWITCHABLE)
		{
		  if ((ptype==INPUT_PIN) || (ptype==BIDIR_PIN_MUX_SWITCHABLE))
		    return(loop);
		}
	      else if (pin_type->query(loop)==BIDIR_PIN_MUX_RESULT)
		{
		  if ((ptype==OUTPUT_PIN) || (ptype==BIDIR_PIN_MUX_RESULT))
		    return(loop);
		}
	      break;
	    default:
	      if (pin_type->query(loop)==ptype)
		return(loop);
	    }
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
  if (DEBUG_PIN_CONNECT)
    printf("pin_no(%d), pin_assigned(%d), drives acs(%d) via pin(%d)\n",
	   pin_no,
	   pin_assigned->query(pin_no),
	   acs_id,
	   dest_pin);

  if (pin_assigned->query(pin_no)==UNASSIGNED)
    {
      if (DEBUG_PIN_CONNECT)
	printf("pin is unassigned\n");
      pin_assigned->set(pin_no,ASSIGNED);
      connect[pin_no]->add_node(acs_id,dest_pin,node_type);
    }
  else
    {
      if ((pin_classtype(pin_no)==OUTPUT_PIN) || (pin_classtype(pin_no)==BIDIR_PIN))
	connect[pin_no]->add_node(acs_id,dest_pin,node_type);
      else
	{
	  connect[pin_no]->assign_node(0,acs_id,pin_no,node_type);
	  if (DEBUG_PIN_CONNECT)
	    printf("Pin::connect_pin:Error? Overwriting existing connection\n");
	}
    }

  if (DEBUG_PIN_CONNECT)
    print_pins("after connect_pin");

  // Return happy condition
  return(1);
}

int Pin::connect_pin(const int pin_no,
		     const int acs_id,
		     const int dest_pin,
		     const int node_type,
		     const int signal)
{
  if (DEBUG_PIN_CONNECT)
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
      if ((pin_classtype(pin_no)==OUTPUT_PIN) ||
	  (pin_classtype(pin_no)==BIDIR_PIN))
	{
	  int new_node=connect[pin_no]->add_node(acs_id,dest_pin,node_type);
	  connect[pin_no]->set_pinsignal(new_node,signal);
	}
      else
	{
	  int new_node=connect[pin_no]->add_node(acs_id,dest_pin,node_type);
	  connect[pin_no]->set_pinsignal(new_node,signal);
	  if (DEBUG_PIN_CONNECT)
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
	{
	  if (DEBUG_PIN_MATCH)
	    printf("match_acstype, pin=%d, ref_acsid=%d. connect[%d]_acsid=%d\n",
		   loop,acsid,cloop,connect[loop]->query_acsid(cloop));
	  if (connect[loop]->query_acsid(cloop)==acsid)
	    return(loop);
	}
  
  // Return unhappy condition
  if (DEBUG_PIN_MATCH)
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
    {
      if (DEBUG_PIN_MATCH)
	printf("match_acsnode, pin=%d, ref_acsid=%d.  connect[%d]_acsid=%d\n",
	       node,acsid,cloop,connect[node]->query_acsid(cloop));
      if (connect[node]->query_acsid(cloop)==acsid)
	return(cloop);
    }

  // Return unhappy condition
  if (DEBUG_PIN_MATCH)
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

int Pin::disconnect_pin_to(const int ref_id,
			   const int src_pin)
{
  Connectivity* onodes=query_connection(src_pin);
  for (int onode_loop=0;onode_loop<onodes->node_count;onode_loop++)
    if (onodes->query_acsid(onode_loop)==ref_id)
      disconnect_pin(src_pin,onode_loop);

  // Return happy condition
  return(1);
}

int Pin::disconnect_pin_from(const int ref_id,
			     const int dest_pin)
{
  Connectivity* inodes=query_connection(dest_pin);
  if (inodes->query_acsid(0)==ref_id)
    disconnect_pin(dest_pin,0);

  // Return happy condition
  return(1);
}

int Pin::disconnect_all_pinsto(const int ref_id)
{
  int a_disconnection=0;
  for (int loop=0;loop<inode_pins->population();loop++)
    {
      int ipin=inode_pins->query(loop);
      Connectivity* inodes=query_connection(ipin);
      if (inodes->query_acsid(0)==ref_id)
	{
	  disconnect_pin(ipin,0);
	  a_disconnection=1;
	}
    }
  for (int loop=0;loop<onode_pins->population();loop++)
    {
      int opin=onode_pins->query(loop);
      Connectivity* onodes=query_connection(opin);
      for (int onode_loop=0;onode_loop<onodes->node_count;onode_loop++)
	if (onodes->query_acsid(onode_loop)==ref_id)
	  {
	    disconnect_pin(opin,onode_loop);
	    a_disconnection=1;
	  }
    }

  // Return disconnection status
  return(a_disconnection);
}

int Pin::disconnect_all_ipins(void)
{
  for (int loop=0;loop<inode_pins->population();loop++)
    {
      int ipin=inode_pins->query(loop);
      pin_assigned->set(ipin,UNASSIGNED);
      connect[ipin]->remove_allnodes();
    }

  // Return happy condition
  return(1);
}

int Pin::disconnect_all_opins(void)
{
  for (int loop=0;loop<onode_pins->population();loop++)
    {
      int opin=onode_pins->query(loop);
      pin_assigned->set(opin,UNASSIGNED);
      connect[opin]->remove_allnodes();
    }

  // Return happy condition
  return(1);
}

int Pin::disconnect_allpin_nodes(const int pin_no)
{
  pin_assigned->set(pin_no,UNASSIGNED);
  connect[pin_no]->remove_allnodes();
  
  // Return happy condition
  return(1);
}

int Pin::disconnect_allpins(void)
{
  for (int loop=0;loop<pin_count;loop++)
    {
      pin_assigned->set(loop,UNASSIGNED);
      connect[loop]->remove_allnodes();
    }

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
  if (pin_count == onode_pins->population())
    return(1);

  // If all pins are locked then no updates are needed
  for (int loop=0;loop<pin_count;loop++)
    {
      if (DEBUG_PIN_PRECISION)
	printf("testing pin %d, lock=%d\n",loop,query_preclock(loop));
      if (query_preclock(loop)==UNLOCKED)
	return(0);
    }
  return(1);
}

////////////////////////////////////////////////////////////
// Return the pin index, with the pin that is named ref_name
////////////////////////////////////////////////////////////
int Pin::pin_withname(const char* ref_name)
{
  int ref_length=strlen(ref_name);

  for (int pin_no=0;pin_no<pin_count;pin_no++)
    {
      if (DEBUG_PIN)
	printf("candidate phole %d = %s, looking for %s, reflength=%d\n",
	       pin_no,
	       query_cpinname(pin_no),
	       ref_name,
	       ref_length);
	     
//      if (strncmp(query_cpinname(pin_no),ref_name,ref_length)==0)
      if (strcmp(query_cpinname(pin_no),ref_name)==0)
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
int Pin::pin_withnetlist(const int ref_netlist, const int inst)
{
  int pin_inst=0;
  for (int pin_no=0;pin_no<pin_count;pin_no++)
    {
      if (DEBUG_PIN)
	  printf("pin(%d):netlist_id=%d, comparing to %d\n",
		 pin_no,
		 netlist_ids->query(pin_no),
		 ref_netlist);
      if (netlist_ids->query(pin_no)==ref_netlist)
	{
	  pin_inst++;
	  if (pin_inst==inst)
	    return(pin_no);
	}
    }
  
  printf("Pin::pin_withnetlist:Error, no pin is associated with netlist %d\n",
	 ref_netlist);
  return(-1);
}

///////////////////////////////////////////////////////
// Return the total number of pins with this netlist id
///////////////////////////////////////////////////////
int Pin::pins_withnetlist(const int ref_netlist)
{
  int net_count=0;
  for (int pin_no=0;pin_no<pin_count;pin_no++)
    if (netlist_ids->query(pin_no)==ref_netlist)
      net_count++;
  return(net_count);
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
////////////////////////////////////////////////////////////////////
// Local methods for determining minimum and maximum of two integers
// FIX:Collisions with Fix::max require the need for a local version
//     rather than using the builtin sys/ddi.h versions.
////////////////////////////////////////////////////////////////////
int Pin::max(const int arg1, const int arg2)
{
  if (arg1 >= arg2)
    return(arg1);
  else
    return(arg2);
}
int Pin::min(const int arg1, const int arg2)
{
  if (arg1 > arg2)
    return(arg2);
  else
    return(arg1);
}

