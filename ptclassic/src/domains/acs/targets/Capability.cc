static const char file_id[] = "Capability.cc";

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
#include "Capability.h"

#ifdef __GNUG__
#pragma implementation
#endif

Capability::Capability()
{
  domains=new StringList;
  architectures=new StringList;
  languages=new SequentialList;
  assert(domains);
  assert(architectures);
  assert(languages);
}

Capability::~Capability()
{
  delete domains;
  delete architectures;
  delete languages;
}

Capability& Capability::operator=(const Capability& rh_cap)
{
  char* tmp_name=new char[MAX_STR];
  
  if ((rh_cap.domains)->numPieces()!=0)
    for (int loop=1;loop<=(rh_cap.domains)->numPieces();loop++)
      {
	strcpy(tmp_name,retrieve_string(rh_cap.domains,loop));
	*domains << tmp_name;
	strcpy(tmp_name,retrieve_string(rh_cap.architectures,loop));
	*architectures << tmp_name;
	
	for (int copy_loop=1;copy_loop<languages->size();copy_loop++)
	  languages->append((rh_cap.languages)->elem(copy_loop));
      }

  // Cleanup
  delete []tmp_name;

  return *this;
}

char* Capability::retrieve_string(StringList* the_list,int index)
{
  StringListIter stringIter(*the_list);
  char* entry_name;
  int count=0;

  while ((entry_name=(char*) stringIter++)!=NULL)
    {
      count++;
      if (count==index)
	return(entry_name);
    }

  if (DEBUG_CAPABILITY)
    printf("Capability::retrieve_string:Warning, unable to find matching string\n");
  return(NULL);
}

int Capability::add_domain(char* domain_name)
{
  *domains << domain_name;

  // Return happy condition
  return(1);
}
int Capability::add_architecture(char* arch_name)
{
  *architectures << arch_name;

  // Return happy condition
  return(1);
}
int Capability::add_language(int language)
{
  languages->append((Pointer) language);

  // Return happy condition
  return(1);
}
int Capability::get_language(int index)
{
  if (index < languages->size())
    return((int) languages->elem(index));
  else
    {
      if (DEBUG_CAPABILITY)
	printf("Capability::get_language:Error, invalid index provided\n");
      return(-1);
    }
}

