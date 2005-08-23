static const char file_id[] = "Capability.cc";

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
 Version: @(#)Capability.cc	1.5 08/02/01
***********************************************************************/
#include "Capability.h"

#ifdef __GNUG__
#pragma implementation
#endif

Capability::Capability()
{
  domains=new StringArray;
  architectures=new StringArray;
  languages=new ACSIntArray;
}

Capability::~Capability()
{
  delete domains;
  delete architectures;
  delete languages;
}

Capability& Capability::operator=(const Capability& rh_cap)
{
  if ((rh_cap.domains)->population()!=0)
    for (int loop=0;loop<(rh_cap.domains)->population();loop++)
      {
	domains->add((rh_cap.domains)->get(loop));
	architectures->add((rh_cap.architectures)->get(loop));
	
	for (int copy_loop=0;copy_loop<languages->population();copy_loop++)
	  languages->add((rh_cap.languages)->get(copy_loop));
      }
  return *this;
}

int Capability::add_domain(const char* domain_name)
{
  domains->add(domain_name);

  // Return happy condition
  return(1);
}
int Capability::add_architecture(const char* arch_name)
{
  architectures->add(arch_name);

  // Return happy condition
  return(1);
}
int Capability::add_language(const int language)
{
  languages->add(language);

  // Return happy condition
  return(1);
}
int Capability::get_language(const int index)
{
  return(languages->query(index));
}





