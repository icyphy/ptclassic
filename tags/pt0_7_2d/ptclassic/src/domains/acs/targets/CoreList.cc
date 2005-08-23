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
 Date of creation: 5/5/00
 Version: @(#)CoreList.cc      1.0     5/5/00

 -- Needed for make-depend
***********************************************************************/

#include "CoreList.h"

CoreList::CoreList()
: total_cores(0), core_list(NULL)
{
}

CoreList::~CoreList()
{
/*
  for (int loop=0;loop<total_cores;loop++)
    delete core_list[loop];
    */

  delete []core_list;
}

int CoreList::size(void)
{
  return(total_cores);
}

void CoreList::append(ACSCGFPGACore* new_core)
{
  ACSCGFPGACore** new_list = new ACSCGFPGACore*[total_cores+1];
  for (int loop=0;loop<total_cores;loop++)
    new_list[loop]=core_list[loop];
  new_list[total_cores]=new_core;
  total_cores++;

//  delete []core_list;
  core_list=new_list;
}

ACSCGFPGACore* CoreList::elem(const int index)
{
  if (index >= total_cores)
    abort();
  else
    return(core_list[index]);
}

void CoreList::remove(const int acs_id)
{
  ACSCGFPGACore** new_list = new ACSCGFPGACore*[total_cores-1];

  ACSCGFPGACore* core=find_core(acs_id);
  if (core==NULL)
    return;

  if (DEBUG_CORELIST)
    printf("ignoring core %s (acsid=%d)\n",core->comment_name(),acs_id);

  int index=find_coreid(acs_id);
  for (int loop=0;loop<index;loop++)
    new_list[loop]=elem(loop);

  for (int loop=index+1;loop<total_cores;loop++)
    new_list[loop-1]=elem(loop);
  total_cores--;
  
//  delete []core_list;
  core_list=new_list;
}


ACSCGFPGACore* CoreList::find_core(const int acs_id)
{
  for (int loop=0;loop<total_cores;loop++)
    if (core_list[loop]->acs_id==acs_id)
      return(core_list[loop]);

  return(NULL);
}

int CoreList::find_coreid(const int acs_id)
{
  for (int loop=0;loop<total_cores;loop++)
    if (core_list[loop]->acs_id==acs_id)
      return(loop);

  return(-1);
}

void CoreList::contents(const char* title)
{
  printf("contents of %s are:",title);
  for (int loop=0;loop<total_cores;loop++)
    {
      ACSCGFPGACore* fpga_core=core_list[loop];
      printf("core %d is %s\n",loop,fpga_core->comment_name());
    }
}
