static const char file_id[] = "Constants.cc";

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
 Version: @(#)Constants.cc	1.5 08/02/01
***********************************************************************/
#include "Constants.h"

#ifdef __GNUG__
#pragma implementation
#endif

Constants::Constants(void)
: count(0), sign_convention(UNSIGNED), storage(NULL)
{
  types=new ACSIntArray;
}
Constants::Constants(const int sign_type) 
: count(0), sign_convention(sign_type), storage(NULL)
{
  types=new ACSIntArray;
}
Constants::~Constants(void)
{
  for (int loop=0;loop<count;loop++)
    delete storage[loop];
  delete storage;
  delete types;
  count=0;
}

void Constants::add_double(double value)
{
  add(&value,CDOUBLE);
}

void Constants::add_float(float value)
{
  add(&value,CFLOAT);
}

void Constants::add_long(long value)
{
  add(&value,CLONG);
}

void Constants::add_int(int value)
{
  add(&value,CINT);
}

void Constants::add(void* value,const int type)
{
  void* new_ptr=NULL;

  switch(type)
    {
    case CINT:
      new_ptr=(void *) new int;
      memcpy(new_ptr,value,sizeof(int));
      break;
    case CLONG:
      new_ptr=(void *) new long;
      memcpy(new_ptr,value,sizeof(long));      
      break;
    case CFLOAT:
      new_ptr=(void *) new float;
      memcpy(new_ptr,value,sizeof(float));      
      break;
    case CDOUBLE:
      new_ptr=(void *) new double;
      memcpy(new_ptr,value,sizeof(double));      
      break;
    }

  add_storage(new_ptr);
  types->add(type);

  if (DEBUG_CONST)
    {
      printf("Constants: count=%d\n",count);
      printf("query_bitstr %s converted\n",query_bitstr(count-1,0,16));
    }
}

void Constants::add_storage(void* add_ptr)
{
  void** new_ptr=new void *[count+1];
  
  for (int loop=0;loop<count;loop++)
    new_ptr[loop]=storage[loop];

  new_ptr[count]=add_ptr;
  storage=new_ptr;

  count++;
}

int Constants::get_int(int index)
{
  return(*((int *) storage[index]));
}

long Constants::get_long(int index)
{
  return(*((long *) storage[index]));
}

float Constants::get_float(int index)
{
  return(*((float *) storage[index]));
}

double Constants::get_double(int index)
{
  return(*(double *) storage[index]);
}

// Return a constant value in either string format in '' or
// in numeric format.  Particular value is selected by an index
char* Constants::query_bitstr(int index,
			      int major_bit,
			      int bitlen)
{
  int indexed_type=types->query(index);

  // Breakup the indexed value into an integer and fractional component
  double value=0.0;
  switch (indexed_type)
    {
    case CINT:
      value=(double) get_int(index);
      break;
    case CLONG:
      value=(double) get_long(index);
      break;
    case CFLOAT:
      value=(double) get_float(index);
      break;
    case CDOUBLE:
      value=get_double(index);
      break;
    }
  if (DEBUG_CONST)
    printf("Extracted constant %d w/val %f, convert to (%d,%d)\n",
	   index,
	   value,
	   major_bit,
	   bitlen);

  // Convert magnitude to binary representation
  double abs_value=value;
  if (abs_value < 0)
    abs_value*=-1;
  int cur_bit=major_bit;
  int bit_ptr=bitlen-1;
  int* bit_array=new int[bitlen];
  while (bit_ptr>=0)
    {
      double power=pow(2.0,(double) cur_bit);
      if (DEBUG_CONST)
	printf("abs_value=%f, power=%f\n",abs_value,power);
      if (abs_value >= power)
	{
	  bit_array[bit_ptr]=1;
	  abs_value-=power;
	}
      else
	bit_array[bit_ptr]=0;
      cur_bit--;
      bit_ptr--;
    }

  // Convert to 2's complement if negative value
  if (sign_convention==SIGNED)
    if (value<0)
      {
	if (DEBUG_CONST)
	  printf("Constants::query_str:Converting negative value\n");
	
	// Invert bits
	for (int loop=0;loop<bitlen;loop++)
	  if (bit_array[loop]==1)
	    bit_array[loop]=0;
	  else
	    bit_array[loop]=1;
	
	// Add 1
	int carry=1;
	int bit=0;
	while (carry)
	  if (bit_array[bit]==1)
	    {
	      bit_array[bit]=0;
	      bit++;
	      
	      //FIX: This should set an error for insufficient bits
	      if (bit==bitlen)
		carry=0;
	    }
	  else
	    {
	      bit_array[bit]=1;
	      carry=0;
	    }
      }

  // Generate character version
  ostrstream tmp_str;
  for (int loop=bitlen-1;loop>=0;loop--)
    if (bit_array[loop]==1)
      tmp_str << "1";
    else
      tmp_str << "0";

  tmp_str << ends;
  return(tmp_str.str());
}

// Return a constant value in string format
// Particular value is selected by an index
char* Constants::query_str(int index,
			   int majorbits,
			   int bitlen)
{
  ostrstream tmp_str;
  char* tmp_valstr=new char[MAX_STR];
  char* format_str=new char[MAX_STR];
  
  int indexed_type=types->query(index);
  int queried_int=0;
  long queried_long=0;
  float queried_float=0.0;
  double queried_double=0.0;

  // If applicable
  if ((indexed_type==CFLOAT) || (indexed_type==CDOUBLE))
    {
      int msd=1;
      int lsd=2;
      if (majorbits > 1)
	msd=majorbits;

      int delta_bits=majorbits+1-bitlen;
      if (delta_bits<-2)
	lsd=-1*delta_bits;

      // FIX:There is probably a more clever way of doing this:
      ostrstream format;
      format << "%" << msd << "." << lsd << "f" << ends;
      strcpy(format_str,format.str());
    }

  switch (indexed_type)
    {
    case CINT:
      queried_int=get_int(index);
      sprintf(tmp_valstr,"%d",queried_int);
      break;
    case CLONG:
      queried_long=get_long(index);
      sprintf(tmp_valstr,"%ld",queried_long);
      break;
    case CFLOAT:
      queried_float=get_float(index);
      sprintf(tmp_valstr,"%g",queried_float);
//      sprintf(tmp_valstr,format_str,queried_float);
      break;
    case CDOUBLE:
      queried_double=get_double(index);
      sprintf(tmp_valstr,"%g",queried_double);
//      sprintf(tmp_valstr,format_str,queried_double);
      break;
    }

  tmp_str << tmp_valstr << ends;
  delete []tmp_valstr;
  delete []format_str;
  return(tmp_str.str());
}


int Constants::query_bitsize(const int index,
			     const int limit)
{
  int indexed_type=types->query(index);
  long value=0;
  switch (indexed_type)
    {
    case CINT:
      value=get_int(index);
      break;
    case CLONG:
      value=get_long(index);
      break;
    case CFLOAT:
      return(query_bitsize_float(index,limit));
    case CDOUBLE:
      value=0;
      break;
    }
  
  // log2(x)=log(x)/log(2)
  long abs_value=(long) abs(value);
  int bit_count=0;
  if (abs_value==0)
    bit_count=1;
  else
    bit_count=(int) ceil(log(abs_value+1.0)/log(2.0));

  if (value<0)
    bit_count++;

  if (sign_convention==SIGNED)
    bit_count++;

  if (DEBUG_CONST)
    printf("Converted value %ld to bit count %d\n",
	   value,
	   bit_count);
  return(bit_count);
}

int Constants::query_bitsize_float(const int index,
				   const int limit)
{
  float value=get_float(index);
  int power_test=1;
  while (power_test < limit)
    {
      float curr_value=value*pow(2.0,power_test);
      float trunc_value=curr_value-(int) curr_value;
      if (trunc_value < 0)
	trunc_value*=-1.0;
      if ((curr_value-trunc_value) <= 0)
	return(power_test);
      else
	power_test++;
    }
  if (DEBUG_CONST)
    printf("limit hit of %d\n",limit);
  return(limit);
}

