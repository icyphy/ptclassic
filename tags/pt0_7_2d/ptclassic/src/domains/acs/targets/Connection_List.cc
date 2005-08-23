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
 Date of creation: 5/8/00
 Version: @(#)Connection_List.cc      1.0     5/8/00

 -- Needed for make-depend
***********************************************************************/

#include "Connection_List.h"

Connection_List::Connection_List()
: total_connections(0)
{
  ext_src_pins=new ACSIntArray;
  src_cores=new ACSIntArray;
  src_pins=new ACSIntArray;
  ext_dest_pins=new ACSIntArray;
  dest_cores=new ACSIntArray;
  dest_pins=new ACSIntArray;
}

Connection_List::~Connection_List()
{
  delete ext_src_pins;
  delete src_cores;
  delete src_pins;
  delete ext_dest_pins;
  delete dest_cores;
  delete dest_pins;
}

int Connection_List::count(void)
{
  return(total_connections);
}

void Connection_List::add(const int ext_src_pin,
			  const int src_core,
			  const int src_pin,
			  const int ext_dest_pin,
			  const int dest_core,
			  const int dest_pin)
{
  ext_src_pins->add(ext_src_pin);
  src_cores->add(src_core);
  src_pins->add(src_pin);
  ext_dest_pins->add(ext_dest_pin);
  dest_cores->add(dest_core);
  dest_pins->add(dest_pin);

  total_connections++;
}

void Connection_List::get(const int index,
			  int& ext_src_pin,
			  int& src_core,
			  int& src_pin,
			  int& ext_dest_pin,
			  int& dest_core,
			  int& dest_pin)
{
  if (index >= total_connections)
    abort();

  ext_src_pin=ext_src_pins->query(index);
  src_core=src_cores->query(index);
  src_pin=src_pins->query(index);
  ext_dest_pin=ext_dest_pins->query(index);
  dest_core=dest_cores->query(index);
  dest_pin=dest_pins->query(index);
}

void Connection_List::print(const char* title)
{
  printf("%s:\n",title);
  for (int loop=0;loop<total_connections;loop++)
    printf("%d:(%d,%d,%d) (%d,%d,%d)\n",
	   loop,
	   ext_src_pins->query(loop),src_cores->query(loop),src_pins->query(loop),
	   ext_dest_pins->query(loop),dest_cores->query(loop),dest_pins->query(loop));
}

