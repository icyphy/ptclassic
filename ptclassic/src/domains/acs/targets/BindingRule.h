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
 Date of creation: 6/20/00
 Version: @(#)BindingRule.h      1.0     6/20/00

 -- Needed for make-depend
***********************************************************************/
#ifndef BINDINGRULE_H
#define BINDINGRULE_H

#include <stdlib.h>
#include "ACSIntArray.h"

class BindingRule {
  static const int DEBUG_BINDINGRULE=0;

 private:
  int origin_type;
  ACSIntArray* matching_types;

 public:
 BindingRule::BindingRule();
 BindingRule::~BindingRule();
  void BindingRule::set_origin(const int);
  void BindingRule::add_match(const int);
  int BindingRule::match_origin(const int);
  int BindingRule::match(const int);
};

#endif
