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
 Version: @(#)Constants.h	1.6 08/02/01
***********************************************************************/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __GNUG__
#pragma interface
#endif

#include <stdlib.h>
#include <stdio.h>
#include <strstream.h>
#include <math.h>
#include "ACSIntArray.h"
#include "acs_starconsts.h"

static const int DEBUG_CONST=0;

// Constant (class) constants
static const int CINT=0;
static const int CLONG=1;
static const int CFLOAT=2;
static const int CDOUBLE=3;

// Sign convention
// ASSUMPTION:In sync with acs_starconsts.h conventions!!!!

class Constants 
{
public:
  int count;
  int sign_convention;
  void** storage;
  ACSIntArray* types;

public:
 Constants::Constants(void);
 Constants::Constants(const int);
 Constants::~Constants(void);
 void Constants::add_double(double);
 void Constants::add_float(float);
 void Constants::add_long(long);
 void Constants::add_int(int);
 void Constants::add(void*,
		     const int);
 void Constants::add_storage(void*);
 int Constants::get_int(int);
 long Constants::get_long(int);
 float Constants::get_float(int);
 double Constants::get_double(int);

 char* Constants::query_bitstr(int,
			       int,
			       int);
 char* Constants::query_str(int,
			    int,
			    int);
 int Constants::query_bitsize(const int, const int);
 int Constants::query_bitsize_float(const int, const int);
};

#endif
