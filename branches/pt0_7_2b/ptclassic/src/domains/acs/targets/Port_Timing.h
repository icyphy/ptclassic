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
 Version: @(#)Port_Timing.h     1.0     06/16/99
***********************************************************************/
#ifndef PORT_TIMING_H
#define PORT_TIMING_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACSIntArray.h"
#include "acs_starconsts.h"


class Port_Timing
{
  static const int DEBUG_TIMING=0;

 public:
  int count;
  int free_muxline;
  ACSIntArray* address_start;
  ACSIntArray* address_step;
  ACSIntArray* address_activation;
  ACSIntArray* address_rate;
  ACSIntArray* vector_length;
  ACSIntArray* major_bit;
  ACSIntArray* bitlen;
  ACSIntArray* data_activation;
  ACSIntArray* update_sequence;        // Delta's from address sequence
  ACSIntArray* mem_type;
  ACSIntArray* mem_id;
  ACSIntArray* mux_line;

  // Local copy of memport timings
  int read_skew;
  int write_skew;

public:
 Port_Timing::Port_Timing();
 Port_Timing::Port_Timing(const int,const int);
 Port_Timing::~Port_Timing();
 int Port_Timing::add_read(const int,const int,const int,const int,const int,const int,const int,const int,const int);
 int Port_Timing::add_lutread(const int,const int,const int,const int,const int,const int,const int,const int,const int);
 int Port_Timing::add_write(const int,const int,const int,const int,const int,const int,const int,const int,const int);
 int Port_Timing::adjust_times(const int,const int,int&,int&,int);
 int Port_Timing::add_entry(const int,const int,const int,const int, const int,const int,const int,const int,const int,const int);
 int Port_Timing::sort_times(void);
 int Port_Timing::calc_updseq(void);
 int Port_Timing::add_wordcount(const int);
 int Port_Timing::get_sa(const int);
 int Port_Timing::get_addrate(const int);
 int Port_Timing::get_vectorlength(const int);
 int Port_Timing::get_memtype(const int);
 int Port_Timing::get_majorbit(const int);
 int Port_Timing::get_bitlen(const int);
 int Port_Timing::get_memid(const int);
 int Port_Timing::get_addresstime(const int);
 ACSIntArray* Port_Timing::get_startingaddress(void);
 ACSIntArray* Port_Timing::get_stepaddress(void);
 ACSIntArray* Port_Timing::get_addressrate(void);
 ACSIntArray* Port_Timing::get_veclen(void);

 int Port_Timing::max_address(const int);
 ACSIntArray* Port_Timing::access_times(const int);
 ACSIntArray* Port_Timing::address_times(void);
 void Port_Timing::dump(void);
};

#endif
