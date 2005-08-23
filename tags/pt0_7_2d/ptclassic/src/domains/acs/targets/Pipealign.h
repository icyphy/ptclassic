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
 Version: @(#)Pipealign.h      1.0     06/16/99
***********************************************************************/
#ifndef PIPEALIGN_H
#define PIPEALIGN_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACSIntArray.h"
#include "acs_starconsts.h"


class Pipealign
{
  static const int DEBUG_PIPEALIGN=0;
 
 public:
  int delays_needed;   // Boolean, should this smart generator require some
                       // delays on it's input chain
  int max_nodeact;     // Closest smart generator activation time 
		       // (to parent smart generator)

  ACSIntArray* snk_pins;  // The relevant pin on the parent smart generator.
                       // Order is in sync with the following arrays/lists
  ACSIntArray* src_acts;  // Activation times for all sourcing smart generators
  ACSIntArray* src_acsids;// List of ACSCGFPGACores acs_ids
  ACSIntArray* new_delays;// The actual number of delays that need to be added
 public:
  Pipealign::Pipealign(void);
  Pipealign::~Pipealign(void);
  int Pipealign::start_over(void);
  int Pipealign::add_src(const int,
                         const int,
                         const int);
  int Pipealign::calc_netdelays(void);
  int Pipealign::calc_netdelays(const int);
  int Pipealign::update_delays(void);
  int Pipealign::revise_delays(void);
  int Pipealign::find_pin(const int);
  void Pipealign::print_delays(void);
};

#endif
