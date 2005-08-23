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
 Version: @(#)Pipealign.cc      1.0     06/16/99
***********************************************************************/
#include "Pipealign.h"

Pipealign::Pipealign(void)
{
  delays_needed=0;
  max_nodeact=-1;
  snk_pins=new ACSIntArray;
  src_acts=new ACSIntArray;
  src_acsids=new ACSIntArray;
  new_delays=new ACSIntArray;
}

Pipealign::~Pipealign(void)
{
  delays_needed=0;
  max_nodeact=-1;
  delete snk_pins;
  delete src_acts;
  delete src_acsids;
  delete new_delays;
}

int Pipealign::start_over(void)
{
  delays_needed=0;
  max_nodeact=-1;
  delete snk_pins;
  delete src_acts;
  delete new_delays;
  delete src_acsids;
  snk_pins=new ACSIntArray;
  src_acts=new ACSIntArray;
  src_acsids=new ACSIntArray;
  new_delays=new ACSIntArray;

  // Return happy condition
  return(1);
}

int Pipealign::add_src(const int snk_pin,
		       const int src_act,
		       const int src_id)
{
  if (DEBUG_PIPEALIGN)
    printf("add src id %d, time %d for snk_pin %d\n",src_id,src_act,snk_pin);

  snk_pins->add(snk_pin);
  src_acts->add(src_act);
  src_acsids->add(src_id);

  if (DEBUG_PIPEALIGN)
    printf("total snk_pins identified %d\n",snk_pins->population());

  // Return happy condition
  return(1);
}

int Pipealign::calc_netdelays(void)
{
  // Calculate relative differences in node activation times
  for (int loop=0;loop<src_acts->population();loop++)
    if (src_acts->query(loop) > max_nodeact)
      max_nodeact=src_acts->query(loop);

  if (DEBUG_PIPEALIGN)
    printf("max_nodeact across all pins(%d)=%d\n",
	   src_acts->population(),
	   max_nodeact);

  update_delays();

  // Return happy condition
  return(1);
}  

// If the activation time of the core is fixed, then pipe alignment delays are implicit
// and are not derived from its connections.  Set this activation time and determine what
// these delays truely are.
int Pipealign::calc_netdelays(const int new_act)
{
  max_nodeact=new_act;

  if (DEBUG_PIPEALIGN)
    printf("max_nodeact across all pins(%d)=%d\n",
	   src_acts->population(),
	   max_nodeact);

  if (src_acts->population() != new_delays->population())
    update_delays();
  else
    revise_delays();

  // Return happy condition
  return(1);
}  

int Pipealign::update_delays(void)
{
  // Determine amounts of delay needed, including zero
  for (int loop=0;loop<src_acts->population();loop++)
    {
      int src_act=src_acts->query(loop);
      if ((src_act!=NONSCHEDULED) && (src_act!=UNASSIGNED))
	{
	  new_delays->add(max_nodeact - src_acts->query(loop));
	  if (DEBUG_PIPEALIGN)
	    printf("update_delays:delay needed for pin %d, src_acts=%d, total new_delays=%d\n",
		   loop,
		   src_acts->query(loop),
		   new_delays->query(loop));
	  if (new_delays->query(loop) > 0)
	    delays_needed=1;
	}
      else
	new_delays->add(0);
    }

  // Return happy condition
  return(1);
}


int Pipealign::revise_delays(void)
{
  // Determine amounts of delay needed, including zero
  for (int loop=0;loop<src_acts->population();loop++)
    {
      new_delays->set(loop,max_nodeact - src_acts->query(loop));
      if (DEBUG_PIPEALIGN)
	printf("revise_delays:delay needed for pin %d, src_acts=%d, total new_delays=%d\n",
	       loop,
	       src_acts->query(loop),
	       new_delays->query(loop));
      if (new_delays->query(loop) > 0)
	delays_needed=1;
    }

  // Return happy condition
  return(1);
}

int Pipealign::find_pin(const int ref_pin)
{
  for (int loop=0;loop<snk_pins->population();loop++)
    if (snk_pins->query(loop)==ref_pin)
      return(loop);

  // Return unhappy condition
  printf("Error:Pipealign::find_pin, ref_pin %d not found. pop=%d\n",
	 ref_pin,
	 snk_pins->population());
  return(-1);
}

void Pipealign::print_delays(void)
{
  for (int loop=0;loop<snk_pins->population();loop++)
    printf("pin %d is connected to acsid %d at time %d, needs %d delays\n",
	   snk_pins->query(loop),
	   src_acsids->query(loop),
	   src_acts->query(loop),
	   new_delays->query(loop));
}
