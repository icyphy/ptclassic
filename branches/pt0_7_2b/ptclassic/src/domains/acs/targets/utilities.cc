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
 Date of creation: 6/26/00
 Version: @(#)utilities.cc      1.0     06/26/00
***********************************************************************/
//#include "Port_Timing.h"
#include "utilities.h"
////////////////////////////////////////////////////////////////////
// Find out if a bit (bit_pos) in the binary sequence representation
// of (value) is set or not.  Return code dictates the result
// FIX:What it lacks in elegance, makes up for the tenancity it
//     evidences;)
////////////////////////////////////////////////////////////////////
int bselcode(const int value, const int bit_pos)
{
  int bit_power=(int) pow(2.0,bit_pos);
//  printf("value=%d, bit_pos=%d, bit_power=%d\n",value,bit_pos,bit_power);

  // Check for trivial answer
  if (value < bit_power)
    return(0);
  if (value == bit_power)
    return(1);

  // Constrain shaving loop
  // log2(x)=log(x)/log(2)
  double precise_bit=log(value)/log(2.0);
  int max_bit=(int) ceil(precise_bit);

  int local_val=value;
  int curr_bit=max_bit;
  for (int bit_loop=max_bit;bit_loop>bit_pos;bit_loop--)
    {
      curr_bit=bit_loop;
      int power=(int) pow(2.0,bit_loop);
      if ((local_val-power) >= 0)
	local_val-=power;
      if (local_val<=0)
	break;
    }
  
  // Now examine bit position in question
  if (curr_bit==bit_pos+1)
    {
      curr_bit--;
      int power=(int) pow(2.0,curr_bit);
      if (((local_val-power)>=0) && (curr_bit==bit_pos))
	return(1);
      else
	return(0);
    }
  return(0);
}
