static const char file_id[] = "IntArray.cc";
/**************************************************************************
Version identification:
$Id$
 
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
 
Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
 
                                                PT_COPYRIGHT_VERSION_2
                                                COPYRIGHTENDKEY
Programmer:  Ken Smith
Date of creation:  6/10/99
 
This class is used by the SDFWF4 star.
 
*************************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "IntArray.h"

IntArray::IntArray(void)
{
  total=0;
  ints=NULL;
}

IntArray::IntArray(int start_count)
{
  total=start_count;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=666;
}

IntArray::IntArray(int start_count,int def_val)
{
  total=start_count;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=def_val;
}

IntArray::~IntArray(void)
{
  if (ints != NULL)
    for (int loop=0;loop<total;loop++)
      ints[loop]=666;
  delete []ints;
  total=0;
}

IntArray& IntArray::operator=(IntArray& rh_array)
{
  total=rh_array.total;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=rh_array.ints[loop];
  return *this;
}

void IntArray::copy(IntArray* rh_array)
{
  total=rh_array->total;
  ints=new int[total];
  for (int loop=0;loop<total;loop++)
    ints[loop]=rh_array->ints[loop];
}

void IntArray::fill(int default_val)
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

IntArray* IntArray::sort_lh(void)
{
  IntArray* results=new IntArray(total);

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
IntArray* IntArray::sort_hl(void)
{
  IntArray* results=new IntArray(total);

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

void IntArray::reorder(IntArray* new_order)
{
  int* new_ints=new int[total];
  
  for (int loop=0;loop<new_order->population();loop++)
    new_ints[new_order->query(loop)]=ints[loop];

  delete []ints;
  ints=new_ints;
}


void IntArray::add(void)
{
  int* new_ints=new int[total+1];
  for (int loop=0;loop<total;loop++)
    new_ints[loop]=ints[loop];
  new_ints[total]=-1;
  
  delete []ints;
  ints=new_ints;

  total++;
}

void IntArray::add(int new_int)
{
  int* new_ints=new int[total+1];
  for (int loop=0;loop<total;loop++)
    new_ints[loop]=ints[loop];
  new_ints[total]=new_int;
  
  delete []ints;
  ints=new_ints;

  total++;
}

void IntArray::remove(int index)
{
  int* new_ints=new int[total-1];
  for (int low_loop=0;low_loop<index;low_loop++)
    new_ints[low_loop]=ints[low_loop];
  for (int high_loop=index+1;high_loop<total;high_loop++)
    new_ints[high_loop-1]=ints[high_loop];
  delete []ints;
  ints=new_ints;

  total--;
}

int IntArray::population(void)
{
  return(total);
}

int IntArray::query(int index)
{
  if (index >= total)
    {
      printf("IntArray::query:Error, index exceeds population\n");
      return(-1);
    }
  else
    return(ints[index]);
}

int IntArray::set(int index,int val)
{
  if (index >= total)
    {
      printf("IntArray::query:Error, index exceeds population\n");
      return(-1);
    }
  else
    ints[index]=val;

  // Return happy condition
  return(1);
}


     
void IntArray::print(const char* header)
{
  printf("%s:\n",header);
  for (int loop=0;loop<total;loop++)
    printf("%d ",ints[loop]);
  printf("\n");
}
