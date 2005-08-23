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
 Version: @(#)StringArray.cc      1.0     06/16/99
***********************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "StringArray.h"

StringArray::StringArray(void) : total(0), strings(NULL) {}

StringArray::~StringArray(void)
{
  for (int loop=0;loop<total;loop++)
    delete strings[loop];
  delete strings;
  total=0;
}

int StringArray::population(void)
{
  return(total);
}

int StringArray::add(const char* new_string)
{
  char** new_strings=new char *[total+1];
  for (int loop=0;loop<total;loop++)
    new_strings[loop]=strings[loop];
  
  new_strings[total]=new char[strlen(new_string)+1];
  strcpy(new_strings[total],new_string);

  delete strings;
  strings=new_strings;
  return(total++);
}

void StringArray::set(const int index, const char* new_string)
{
  strcpy(strings[index],new_string);
}

char* StringArray::get(int index)
{
  if ((index >= total) || (index < 0))
    {
      printf("StringArray::get:Error, index exceeds population\n");
      abort();
      return("NULL");
    }

  return(strings[index]);
}

int StringArray::prefix(const char* prefix_str)
{
  char* tmp_str=NULL;
  for (int loop=0;loop<total;loop++)
    {
      tmp_str=new char[strlen(strings[loop])+strlen(prefix_str)+1];
      strcpy(tmp_str,prefix_str);
      strcat(tmp_str,strings[loop]);
      delete strings[loop];
      strings[loop]=tmp_str;
    }

  // Return happy condition
  return(1);
}
