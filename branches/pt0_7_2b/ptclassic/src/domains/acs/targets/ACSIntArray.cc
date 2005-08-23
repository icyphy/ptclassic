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
 Version: @(#)ACSIntArray.cc	1.4 08/15/00
***********************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACSIntArray.h"

ACSIntArray::ACSIntArray(void)
: total(0), ints(NULL) {}

ACSIntArray::ACSIntArray(int start_count)
: total(start_count)
{
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=666;
}

ACSIntArray::ACSIntArray(int start_count,int def_val)
: total(start_count)
{
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=def_val;
}

ACSIntArray::~ACSIntArray(void)
{
  if (ints != NULL)
    for (int loop=0;loop<total;loop++)
      ints[loop]=666;
  delete []ints;
  total=0;
}

ACSIntArray& ACSIntArray::operator=(ACSIntArray& rh_array)
{
  total=rh_array.total;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=rh_array.ints[loop];
  return *this;
}

inline void ACSIntArray::incr(int index)
{
  ints[index]++;
}

void ACSIntArray::copy(ACSIntArray* rh_array)
{
  total=rh_array->total;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=rh_array->ints[loop];
}

void ACSIntArray::fill(int default_val)
{
  for (int loop=0;loop<total;loop++)
    ints[loop]=default_val;
}

static int intcompare(const void *i, const void *j)
{

  if (*((int *)i) > *((int *)j))
    return(1);
  if (*((int *)i) <= *((int *)j))
    return(-1);
  return(0);
    
}
static int intcompare2(const void *i, const void *j)
{
  if (*((int *)i) < *((int *)j))
    return(1);
  if (*((int *)i) >= *((int *)j))
    return(-1);
  return(0);
    
}

ACSIntArray* ACSIntArray::sort_lh(void)
{
  ACSIntArray* results=new ACSIntArray(total);

  int *tmp_ints=new int[total];
  int *moved_already=new int[total];
  for (int loop=0;loop<total;loop++)
    {
      tmp_ints[loop]=ints[loop];
      moved_already[loop]=0;
    }

  qsort(tmp_ints,total,sizeof(int),intcompare);

  for (int loop=0;loop<total;loop++)
    for (int search_loop=0;search_loop<total;search_loop++)
      if ((moved_already[search_loop]==0) &&
	  (tmp_ints[loop]==ints[search_loop]))
	{
	  results->set(search_loop,loop);
	  moved_already[search_loop]=1;
	  break;
	}
  return(results);
}
ACSIntArray* ACSIntArray::sort_hl(void)
{
  ACSIntArray* results=new ACSIntArray(total);

  int *tmp_ints=new int[total];
  int *moved_already=new int[total];
  for (int loop=0;loop<total;loop++)
    {
      tmp_ints[loop]=ints[loop];
      moved_already[loop]=0;
    }

  qsort(tmp_ints,total,sizeof(int),intcompare2);

  for (int loop=0;loop<total;loop++)
    for (int search_loop=0;search_loop<total;search_loop++)
      if ((moved_already[search_loop]==0) &&
	  (tmp_ints[loop]==ints[search_loop]))
	{
	  results->set(search_loop,loop);
	  moved_already[search_loop]=1;
	  break;
	}
  return(results);
}

void ACSIntArray::reorder(ACSIntArray* new_order)
{
  int* new_ints=new int[total];
  
  for (int loop=0;loop<new_order->population();loop++)
    new_ints[new_order->query(loop)]=ints[loop];

  delete []ints;
  ints=new_ints;
}


void ACSIntArray::add(void)
{
  add(-1);
}

void ACSIntArray::add(int new_int)
{
  int* new_ints=new int[total+1];
  for (int loop=0;loop<total;loop++)
    new_ints[loop]=ints[loop];
  new_ints[total]=new_int;
  
  delete []ints;
  ints=new_ints;

  total++;
}

// Only add the new_int value iff the value has not already been stored in the array
int ACSIntArray::add_unique(int new_int)
{
  for (int loop=0;loop<total;loop++)
    if (ints[loop]==new_int)
      return(0);

  add(new_int);
  return(1);
}

void ACSIntArray::remove(int index)
{
  int* new_ints=new int[total-1];
  for (int low_loop=0;low_loop<index;low_loop++)
    {
//      printf("preserving lower entry %d\n",low_loop);
      new_ints[low_loop]=ints[low_loop];
    }
  for (int high_loop=index+1;high_loop<total;high_loop++)
    {
//      printf("preserving higher entry %d\n",high_loop);
      new_ints[high_loop-1]=ints[high_loop];
    }
  delete []ints;
  ints=new_ints;

  total--;
}

void ACSIntArray::remove_all(void)
{
  total=0;
  delete []ints;
}

inline int ACSIntArray::population(void)
{
  return(total);
}


int ACSIntArray::get(int index)
{
  return(query(index));
}
inline int ACSIntArray::query(int index)
{
  if ((index >= total) || (index < 0))
    {
      printf("ACSIntArray::query:Error, index exceeds population %d of %d\n",
	   index,total);
      abort();
      return(-1);
    }
  else
    return(ints[index]);
}

inline int ACSIntArray::set(int index,int val)
{
  if ((index >= total) || (index < 0))
    {
      printf("ACSIntArray::query:Error, index exceeds population %d of %d\n",
	   index,total);
      abort();
      return(-1);
    }
  else
    ints[index]=val;

  // Return happy condition
  return(1);
}


     
void ACSIntArray::print(const char* header)
{
  printf("%s:\n",header);
  for (int loop=0;loop<total;loop++)
    printf("%d ",ints[loop]);
  printf("\n");
}


int ACSIntArray::find(const int look_val)
{
  for (int loop=0;loop<total;loop++)
    if (ints[loop]==look_val)
      return(1);

  // Return failure of finding a matching value
  return(0);
}

int ACSIntArray::find_index(const int look_val)
{
  for (int loop=0;loop<total;loop++)
    if (ints[loop]==look_val)
      return(loop);

  // Return invalid index
  return(-1);
}

int ACSIntArray::full(const int valid_val)
{
  for (int loop=0;loop<total;loop++)
    if (ints[loop]!=valid_val)
      return(0);

  // Return full condition
  return(1);
}

int ACSIntArray::pop(void)
{
  int val=get(total-1);
  remove(total-1);
  return(val);
}

int ACSIntArray::tail(void)
{
  int val=get(total-1);
  return(val);
}
