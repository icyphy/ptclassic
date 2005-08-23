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
 Version: @(#)Port_Timing.cc      1.0     06/16/99
***********************************************************************/
#include "Port_Timing.h"

Port_Timing::Port_Timing()
{
  count=0;
  free_muxline=1;
  address_start=new ACSIntArray;
  address_step=new ACSIntArray;
  address_activation=new ACSIntArray;
  address_rate=new ACSIntArray;
  vector_length=new ACSIntArray;
  major_bit=new ACSIntArray;
  bitlen=new ACSIntArray;
  data_activation=new ACSIntArray;
  update_sequence=new ACSIntArray;
  mem_type=new ACSIntArray;
  mem_id=new ACSIntArray;
  mux_line=new ACSIntArray;
  read_skew=0;
  write_skew=0;
}
Port_Timing::Port_Timing(const int rskew,
			 const int wskew)
{
  count=0;
  free_muxline=1;
  address_start=new ACSIntArray;
  address_step=new ACSIntArray;
  address_activation=new ACSIntArray;
  address_rate=new ACSIntArray;
  vector_length=new ACSIntArray;
  major_bit=new ACSIntArray;
  bitlen=new ACSIntArray;
  data_activation=new ACSIntArray;
  update_sequence=new ACSIntArray;
  mem_type=new ACSIntArray;
  mem_id=new ACSIntArray;
  mux_line=new ACSIntArray;
  read_skew=rskew;
  write_skew=wskew;
}



Port_Timing::~Port_Timing()
{
  delete address_start;
  delete address_step;
  delete address_activation;
  delete address_rate;
  delete vector_length;
  delete major_bit;
  delete bitlen;
  delete data_activation;
  delete update_sequence;
  delete mem_type;
  delete mem_id;
  delete mux_line;
}


///////////////////////////////////////////////////////////////////////////////
// Add a new read-from-memory entry
///////////////////////////////////////////////////////////////////////////////
int Port_Timing::add_read(const int address,
			  const int address_step,
			  const int address_rate,
			  const int vect_length,
			  const int addr_time,
			  const int data_time,
			  const int mbit,
			  const int blen,
			  const int id)
{
  int addr_t=0;
  int data_t=0;
  
  adjust_times(addr_time,data_time,addr_t,data_t,SOURCE);

  add_entry(address,address_step,address_rate,vect_length,addr_t,data_t,mbit,blen,SOURCE,id);

  // Return happy condition
  return(1);
}

///////////////////////////////////////////////////////////////////////////////
// Add a new read-from-memory LUT entry
///////////////////////////////////////////////////////////////////////////////
int Port_Timing::add_lutread(const int address,
			     const int address_step,
			     const int address_rate,
			     const int vect_length,
			     const int addr_time,
			     const int data_time,
			     const int mbit,
			     const int blen,
			     const int id)
{
  int addr_t=0;
  int data_t=0;
  
  adjust_times(addr_time,data_time,addr_t,data_t,SOURCE_LUT);

  add_entry(address,address_step,address_rate,vect_length,addr_t,data_t,mbit,blen,SOURCE_LUT,id);

  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////////////////
// Add a new write-to-memory entry
///////////////////////////////////////////////////////////////////////////////
int Port_Timing::add_write(const int address,
			   const int address_step,
			   const int address_rate,
			   const int vect_length,
			   const int addr_time,
			   const int data_time,
			   const int mbit,
			   const int blen,
			   const int id)
{
  int addr_t=0;
  int data_t=0;

  adjust_times(addr_time,data_time,addr_t,data_t,SINK);

  add_entry(address,address_step,address_rate,vect_length,addr_t,data_t,mbit,blen,SINK,id);

  // Return happy condition
  return(1);
}


int Port_Timing::adjust_times(const int addr_time,
			      const int data_time,
			      int& addr_t,
			      int& data_t,
			      int type)
{
  int skew=0;
  if (type==SOURCE)
    skew=read_skew;
  else
    skew=write_skew;

  if (addr_time==UNASSIGNED)
    {
      addr_t=data_time-skew;
      data_t=data_time;
    }
  else
    addr_t=addr_time;

  if (data_time==UNASSIGNED)
    {
      addr_t=addr_time;
      data_t=addr_time+skew;
    }
  else
    data_t=data_time;

  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////////////////
// Add a new timing entry
///////////////////////////////////////////////////////////////////////////////
int Port_Timing::add_entry(const int new_sa,
			   const int new_as,
			   const int new_ar,
			   const int new_vl,
			   const int new_aa,
			   const int new_da,
			   const int mbit,
			   const int blen,
			   const int new_type,
			   const int new_id)
{
  count++;

  address_start->add(new_sa);
  address_step->add(new_as);
  address_rate->add(new_ar);
  vector_length->add(new_vl);
  address_activation->add(new_aa);
  major_bit->add(mbit);
  bitlen->add(blen);
  data_activation->add(new_da);
  mem_type->add(new_type);
  mem_id->add(new_id);
  mux_line->add(free_muxline++);  // WARNING:Implies constants are ordered!

  // Return happy condition
  return(1);
}


// Sort the Port_Timing class attributes based on address_activation times
// (increasing order)
int Port_Timing::sort_times(void)
{
  if (DEBUG_TIMING)
    {
      printf("Unsorted address:\n");
      for (int loop=0;loop<count;loop++)
	printf("starting address(%d)=%d, address_act=%d, vlen=%d, data_act=%d, mem_type=%d, mem_id=%d\n",
	       loop,
	       address_start->query(loop),
	       address_activation->query(loop),
	       vector_length->query(loop),
	       data_activation->query(loop),
	       mem_type->query(loop),
	       mem_id->query(loop));
    }

  ACSIntArray* new_order=address_activation->sort_lh();

  if (DEBUG_TIMING)
    {
      printf("The new order\n");
      for (int loop=0;loop<new_order->population();loop++)
	printf("%d ",new_order->query(loop));
      printf("\n");
    }

  address_start->reorder(new_order);
  address_step->reorder(new_order);
  address_rate->reorder(new_order);
  vector_length->reorder(new_order);
  address_activation->reorder(new_order);
  major_bit->reorder(new_order);
  bitlen->reorder(new_order);
  data_activation->reorder(new_order);
  mem_type->reorder(new_order);
  mem_id->reorder(new_order);
  mux_line->reorder(new_order);
  delete new_order;

  if (DEBUG_TIMING)
    {
      printf("Addresses after:\n");
      for (int loop=0;loop<count;loop++)
	printf("starting address(%d)=%d, addr_act=%d, data_act=%d, mem_type=%d, mem_id=%d\n",
	       loop,
	       address_start->query(loop),
	       address_activation->query(loop),
	       data_activation->query(loop),
	       mem_type->query(loop),
	       mem_id->query(loop));
    }


  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////////////////
// Calculate the update sequence.  i.e., the deltas between successive 
// addresses
///////////////////////////////////////////////////////////////////////////////
int Port_Timing::calc_updseq(void)
{
  if (update_sequence != NULL)
    {
      delete update_sequence;
      update_sequence=new ACSIntArray(count);
    }

  int delta=0;
  for (int loop=0;loop<count-1;loop++)
    {
      delta=address_start->query(loop+1)-address_start->query(loop);
      update_sequence->set(loop,delta);
    }
  delta=address_start->query(0)-address_start->query(count-1)+1;
  update_sequence->set(count-1,delta);

  if (DEBUG_TIMING)
    {
      address_start->print("address start");
      update_sequence->print("update sequence");
    }

  // Return happy condition
  return(1);
}

int Port_Timing::get_sa(const int index)
{
  return(address_start->query(index));
}

int Port_Timing::get_addrate(const int index)
{
  return(address_rate->query(index));
}

int Port_Timing::get_vectorlength(const int index)
{
  return(vector_length->query(index));
}

int Port_Timing::get_memtype(const int index)
{
  return(mem_type->query(index));
}

int Port_Timing::get_majorbit(const int index)
{
  return(major_bit->query(index));
}

int Port_Timing::get_bitlen(const int index)
{
  return(bitlen->query(index));
}

int Port_Timing::get_memid(const int index)
{
  return(mem_id->query(index));
}

int Port_Timing::get_addresstime(const int index)
{
  return(address_activation->query(index));
}

//FIX:Should revisit naming convention used for these vs. the discrete
//    accessor methods...plz!!
ACSIntArray* Port_Timing::get_startingaddress(void)
{
  return(address_start);
}

ACSIntArray* Port_Timing::get_stepaddress(void)
{
  return(address_step);
}

ACSIntArray* Port_Timing::get_addressrate(void)
{
  return(address_rate);
}

ACSIntArray* Port_Timing::get_veclen(void)
{
  return(vector_length);
}

/////////////////////////////////////////////////////////////////////////////////
// Search through entire address space looking for the maximum address value that
// will be used
/////////////////////////////////////////////////////////////////////////////////
int Port_Timing::max_address(const int vec_len)
{
  int max_addr=0;
  for (int loop=0;loop<count;loop++)
    {
      int this_span=address_start->query(loop)+(address_step->query(loop)*vec_len);
      if (this_span > max_addr)
	max_addr=this_span;
    }
  return(max_addr);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Return all of the data activation times for either SOURCE or SINK accesses on this port
//////////////////////////////////////////////////////////////////////////////////////////
ACSIntArray* Port_Timing::access_times(const int data_dir)
{
  ACSIntArray* times=new ACSIntArray;
  for (int loop=0;loop<count;loop++)
    if (mem_type->query(loop)==data_dir)
      times->add(data_activation->query(loop));

  return(times);
}

//////////////////////////////////////////////////////////
// Return all of the address activation times on this port
//////////////////////////////////////////////////////////
ACSIntArray* Port_Timing::address_times(void)
{
  return(address_activation);
}

void Port_Timing::dump(void)
{
  for (int loop=0;loop<count;loop++)
    printf("astrt=%d, astep=%d, aa=%d, ar=%d, mb=%d, blen=%d, da=%d, mtype=%d, mid=%d, mux=%d\n",
	   address_start->query(loop),
	   address_step->query(loop),
	   address_activation->query(loop),
	   address_rate->query(loop),
	   major_bit->query(loop),
	   bitlen->query(loop),
	   data_activation->query(loop),
	   mem_type->query(loop),
	   mem_id->query(loop),
	   mux_line->query(loop));
}
