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
 Version: @(#)Address.h      1.0     4/13/00
***********************************************************************/
#ifndef ADDRESS_H
#define ADDRESS_H

#include <stdio.h>
#include "ACSCGFPGACore.h"
#include "CoreList.h"
#include "ACSIntArray.h"
#include "Pin.h"
#include "Sg_Constructs.h"
#include "utilities.h"

// Class constants

class Address
{
 public:
  // Switches
  static const int DEBUG_ADDRESS=0;
  static const int DEBUG_GENADDRESS=0;

  // Class attributes
  ACSCGFPGACore* addrmux_star;
  CoreList* const_stars;
  ACSCGFPGACore* addrgen_star;
  ACSCGFPGACore* addrbuf_star;
  int addr_size;
  int addr_lo;
  int addr_hi;

public:
 Address::Address(void);
 Address::~Address(void);
  int Address::set_address(const int, const int, const int);
  void Address::genhw_address(Sg_Constructs*, const int, const int, Pin*, const int, Port_Timing*,CoreList*);
  void Address::genhw_lutaddress(Sg_Constructs*, const int, const int, const int, const int, Pin*, const int, Port_Timing*,CoreList*);
};

#endif
