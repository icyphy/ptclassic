/**********************************************************************
Copyright (c) 1999-%Q% Sanders, a Lockheed Martin Company
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
 Version: $Id$
***********************************************************************/
#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <stdio.h>
#include <assert.h>
#include "StringList.h"
#include "acs_starconsts.h"

static const int DEBUG_CAPABILITY=0;

class Capability {

 public:
  StringList* domains;
  StringList* architectures;
  SequentialList* languages;

 public:
  Capability::Capability();
  Capability::~Capability();
  Capability& Capability::operator=(const Capability&);
  char* Capability::retrieve_string(StringList*,int);
  int Capability::add_domain(char*);
  int Capability::add_architecture(char*);
  int Capability::add_language(int);
  int Capability::get_language(int);
};

#endif