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
 Version: @(#)DoubleArray.cc      1.0     06/16/99
***********************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "DoubleArray.h"

inline DoubleArray::DoubleArray(void) 
: total(0), doubles(NULL) {}

DoubleArray::DoubleArray(int start_count) 
: total(start_count)
{
  doubles=new double[total];
  for (int loop=0;loop<total;loop++)
    doubles[loop]=-1.0;
}

DoubleArray::DoubleArray(int start_count,double def_val) 
: total(start_count)
{
  doubles=new double[total];
  for (int loop=0;loop<total;loop++)
    doubles[loop]=def_val;
}

DoubleArray::~DoubleArray(void)
{
  if (doubles != NULL)
    for (int loop=0;loop<total;loop++)
      doubles[loop]=-1.0;
  delete []doubles;
  total=0;
}

DoubleArray& DoubleArray::operator=(DoubleArray& rh_array)
{
  total=rh_array.total;
  doubles=new double[total];
  for (int loop=0;loop<total;loop++)
    doubles[loop]=rh_array.doubles[loop];
  return *this;
}

void DoubleArray::copy(DoubleArray* rh_array)
{
  total=rh_array->total;
  doubles=new double[total];
  for (int loop=0;loop<total;loop++)
    doubles[loop]=rh_array->doubles[loop];
}

void DoubleArray::fill(double default_val)
{
  for (int loop=0;loop<total;loop++)
    doubles[loop]=default_val;
}

void DoubleArray::add(void)
{
  double* new_doubles=new double[total+1];
  for (int loop=0;loop<total;loop++)
    new_doubles[loop]=doubles[loop];
  
  delete []doubles;
  doubles=new_doubles;

  total++;
}

void DoubleArray::add(double new_double)
{
  double* new_doubles=new double[total+1];
  for (int loop=0;loop<total;loop++)
    new_doubles[loop]=doubles[loop];
  new_doubles[total]=new_double;
  
  delete []doubles;
  doubles=new_doubles;

  total++;
}

void DoubleArray::remove(int index)
{
  double* new_doubles=new double[total-1];
  for (int low_loop=0;low_loop<index;low_loop++)
    new_doubles[low_loop]=doubles[low_loop];
  for (int high_loop=index+1;high_loop<total;high_loop++)
    new_doubles[high_loop-1]=doubles[high_loop];
  delete []doubles;
  doubles=new_doubles;
}

inline int DoubleArray::population(void)
{
  return(total);
}

inline double DoubleArray::query(int index)
{
  if (index >= total)
    {
      printf("DoubleArray::query:Error, index exceeds population\n");
      return(-1);
    }
  else
    return(doubles[index]);
}

inline int DoubleArray::set(int index,double val)
{
  if (index >= total)
    {
      printf("DoubleArray::query:Error, index exceeds population\n");
      return(-1);
    }
  else
    doubles[index]=val;

  // Return happy condition
  return(1);
}


     
void DoubleArray::print(const char* header)
{
  printf("%s:\n",header);
  for (int loop=0;loop<total;loop++)
    printf("%f ",doubles[loop]);
  printf("\n");
}
