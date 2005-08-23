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
 Version: @(#)Connectivity.cc	1.5 08/02/01
***********************************************************************/
#include "Connectivity.h"

Connectivity::Connectivity() 
: node_count(0)
{
  if (DEBUG_CONNECT)
    printf("Connectivity constructor invoked\n");
  acs_id=new ACSIntArray;
  pin_id=new ACSIntArray;
  pin_type=new ACSIntArray;
  pin_signal=new ACSIntArray;
  data_dir=new ACSIntArray;
  major_bit=new ACSIntArray;
  bitlength=new ACSIntArray;
}

Connectivity::~Connectivity()
{
  if (DEBUG_CONNECT)
    printf("Connectivity destructor invoked\n");
  node_count=0;
  delete acs_id;
  delete pin_id;
  delete pin_type;
  delete pin_signal;
  delete data_dir;
  delete major_bit;
  delete bitlength;
}

Connectivity& Connectivity::operator=(Connectivity& rh_con)
{
  if (DEBUG_CONNECT)
    printf("Connectivity operator= invoked\n");
  node_count=rh_con.node_count;
  acs_id->copy(rh_con.acs_id);
  pin_id->copy(rh_con.pin_id);
  pin_type->copy(rh_con.pin_type);
  pin_signal->copy(rh_con.pin_signal);
  data_dir->copy(rh_con.data_dir);
  major_bit->copy(rh_con.major_bit);
  bitlength->copy(rh_con.bitlength);

  return *this;
}

int Connectivity::initialize(void)
{
  pin_id->fill(UNASSIGNED);
  pin_type->fill(UNKNOWN);
  pin_signal->fill(UNASSIGNED);

  // Return happy condition
  return(1);
}

// Assign node node_no to a type
int Connectivity::assign_node(const int node_no,
			      const int acsid,
			      const int pintype)
{
  if (DEBUG_CONNECT)
    printf("assigning connectivity node %d to acsid %d, of type %d\n",
	   node_no,acsid,pintype);

  acs_id->set(node_no,acsid);
  pin_id->set(node_no,pintype);

  // Return happy condition
  return(1);
}

// Assign a node (id) to a type of (DATA_NODE, CTRL_NODE, or EXT_NODE)
int Connectivity::assign_node(const int node_no,
			      const int acsid,
			      const int pinid,
			      const int pintype)
{
  if (DEBUG_CONNECT)
    printf("assigning connectivity node %d to acsid %d, pin %d of type %d\n",
	   node_no,acsid,pinid,pintype);
  
  acs_id->set(node_no,acsid);
  pin_id->set(node_no,pinid);
  pin_type->set(node_no,pintype);

  // Since the connection has changed, the signal will need to be
  // remapped by the resolver.
  pin_signal->set(node_no,UNASSIGNED);

  // Return happy condition
  return(1);
}

// Assign a node (id) to a type of (DATA_NODE, CTRL_NODE, or EXT_NODE)
int Connectivity::assign_node(const int node_no,
			      const int acsid,
			      const int pinid,
			      const int pintype,
			      const int signal_id)
{
  if (DEBUG_CONNECT)
    {
      printf("assigning connectivity node %d to acsid %d, ",node_no,acsid);
      printf("pin %d of type %d and signal %d\n",pinid,pintype,signal_id);
    }

  acs_id->set(node_no,acsid);
  pin_id->set(node_no,pinid);
  pin_type->set(node_no,pintype);
  pin_signal->set(node_no,signal_id);

  // Return happy condition
  return(1);
}

// Add another node with an destination ID of id 
// and type of (DATA_NODE, CTRL_NODE, or EXT_NODE)
int Connectivity::add_node(const int acsid,
			   const int id,
			   const int type)
{
  if (DEBUG_CONNECT)
    {
      printf("Connectivity::add_node, node count=%d\n",node_count);
      printf("will drive acsid(%d), id(%d), type(%d)\n",acsid,id,type);
    }

  node_count++;

  acs_id->add(acsid);
  pin_id->add(id);
  pin_type->add(type);
  pin_signal->add(UNASSIGNED);

  // Return new node number (offset for arrays)
  return(node_count-1);
}

// Return the node pointer that matches a given id
int Connectivity::find_acsnode(const int id)
{
  for (int loop=0;loop<node_count;loop++)
    if (acs_id->query(loop)==id)
      return(loop);

  // Return unhappy condition
  if (DEBUG_CONNECT)
    printf("Connectivity::find_acsnode:Warning, unable to find match\n");
  return(-1);
}

int Connectivity::find_node(const int id)
{
  for (int loop=0;loop<node_count;loop++)
    if (pin_id->query(loop)==id)
      return(loop);

  // Return unhappy condition
  if (DEBUG_CONNECT)
    printf("Connectivity::find_node:Warning, unable to find match\n");
  return(-1);
}

// Return the node pointer that matches a given type and id
int Connectivity::find_node(const int id,const int type)
{
  for (int loop=0;loop<node_count;loop++)
    if ((pin_id->query(loop)==id) && (pin_type->query(loop)==type))
      return(loop);

  // Return unhappy condition
  if (DEBUG_CONNECT)
    printf("Connectivity::find_node(2 args):Warning, unable to find match\n");
  return(-1);
}

int Connectivity::remove_node(const int node_no)
{
  acs_id->remove(node_no);
  pin_id->remove(node_no);
  pin_type->remove(node_no);
  pin_signal->remove(node_no);

  node_count--;

  // Return happy condition
  return(1);
}


// Could be neater, but want to make sure;)
int Connectivity::remove_allnodes()
{
  node_count=0;
  delete acs_id;
  delete pin_id;
  delete pin_type;
  delete pin_signal;
  acs_id=new ACSIntArray;
  pin_id=new ACSIntArray;
  pin_type=new ACSIntArray;
  pin_signal=new ACSIntArray;

  // Return happy condition
  return(1);
}

// Given a specific signal index return its value
int Connectivity::query_pinsignal(const int index)
{
  return(pin_signal->query(index));
}


// Set a specific node_signal to the signal identifier provided
int Connectivity::set_pinsignal(const int index,
				const int sig_val)
{
  pin_signal->set(index,sig_val);
  
  // Return happy condition
  return(1);
}

// Given a specific acs_id index return its value
int Connectivity::query_acsid(const int index)
{
  return(acs_id->query(index));
}

// Given a specific node_type index return its value
int Connectivity::query_pintype(const int index)
{
  return(pin_type->query(index));
}
// Given a specific node_id index return its value
int Connectivity::query_pinid(const int index)
{
  return(pin_id->query(index));
}

// Essentially return the number of node_counts
int Connectivity::connected(void)
{
  return(node_count);
}



int Connectivity::dump(void)
{
  if (node_count==0)
    printf("No connections defined yet\n");

  for (int loop=0;loop < node_count;loop++)
    printf("index %d, acs_id=%d, pin_id=%d, pin_type=%d, pin_signal=%d\n",
	   loop,
	   acs_id->query(loop),
	   pin_id->query(loop),
	   pin_type->query(loop),
	   pin_signal->query(loop));

  // Return happy condition
  return(1);
}
