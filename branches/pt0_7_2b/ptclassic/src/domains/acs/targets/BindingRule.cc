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
 Version: @(#)BindingRule.cc      1.0     6/20/00

 -- Needed for make-depend
***********************************************************************/

#include "BindingRule.h"

BindingRule::BindingRule()
: origin_type(-1)
{
  matching_types=new ACSIntArray;
}

BindingRule::~BindingRule()
{
  origin_type=-1;
  delete matching_types;
}

void BindingRule::set_origin(const int new_origin)
{
  origin_type=new_origin;
}

void BindingRule::add_match(const int new_match)
{
  matching_types->add(new_match);
}

int BindingRule::match_origin(const int ref)
{
  if (origin_type==ref)
    return(1);
  else
    return(0);
}

int BindingRule::match(const int ref)
{
  for (int loop=0;loop<matching_types->population();loop++)
    if (ref==matching_types->query(loop))
      return(1);
  return(0);
}
