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
 Version: @(#)Delay.cc      1.0     06/16/99
***********************************************************************/
#include "Delay.h"

Delay::Delay(void)
{
  delay_count=0;
  delays_needed=0;
  dest_ids=new ACSIntArray;
  dest_pins=new ACSIntArray;
  delay_requirements=new ACSIntArray;
  delay_ids=new ACSIntArray;
}

Delay::~Delay(void)
{
  delay_count=0;
  delays_needed=0;
  delete dest_ids;
  delete dest_pins;
  delete delay_requirements;
  delete delay_ids;
}

Delay& Delay::operator=(Delay& rh_delay)
{
  delay_count=rh_delay.delay_count;
  delays_needed=rh_delay.delays_needed;
  dest_ids->copy(rh_delay.dest_ids);
  dest_pins->copy(rh_delay.dest_pins);
  delay_requirements->copy(rh_delay.delay_requirements);
  delay_ids->copy(rh_delay.delay_ids);

  return *this;
}

int Delay::add_dest(int acs_id, 
		    int dest_pin,
		    int delay_reqs)
{
  delay_count++;
  dest_ids->add(acs_id);
  dest_pins->add(dest_pin);
  delay_requirements->add(delay_reqs);

  if (delay_reqs != 0)
    delays_needed=1;

  // Return happy condition
  return(1);
}

int Delay::sort_delays(void)
{
  if (DEBUG_DELAY)
    {
      printf("Delay requirements before:\n");
      for (int loop=0;loop<delay_requirements->population();loop++)
	printf("index=%d, dest_id=%d, dest_pin=%d, delay_req=%d\n",
	       loop,
	       dest_ids->query(loop),
	       dest_pins->query(loop),
	       delay_requirements->query(loop));
    }

  ACSIntArray* new_order=delay_requirements->sort_hl();
  dest_ids->reorder(new_order);
  dest_pins->reorder(new_order);
  delay_requirements->reorder(new_order);

  if (DEBUG_DELAY)
    {
      printf("Delay requirements after:\n");
      for (int loop=0;loop<delay_requirements->population();loop++)
	printf("index=%d, dest_id=%d, dest_pin=%d, delay_req=%d\n",
	       loop,
	       dest_ids->query(loop),
	       dest_pins->query(loop),
	       delay_requirements->query(loop));
    }

  // Return happy condition
  return(1);
}
