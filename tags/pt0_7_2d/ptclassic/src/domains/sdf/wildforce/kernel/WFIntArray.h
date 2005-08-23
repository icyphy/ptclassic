#ifndef  _WFIntArray_h
#define  _WFIntArray_h 1
/**************************************************************************
Version identification:
@(#)WFIntArray.h	1.1 10/06/99
 
Copyright (c) 1999 The Regents of the University of California.
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

#ifndef INTARRAY_H
#define INTARRAY_H

class WFIntArray
{
 private:
  int total;
  int* ints;

 public:
 WFIntArray::WFIntArray(void);
 WFIntArray::WFIntArray(int);
 WFIntArray::WFIntArray(int,int);
 WFIntArray::~WFIntArray(void);
  WFIntArray& operator=(WFIntArray&);
  void WFIntArray::copy(WFIntArray*);
  void WFIntArray::fill(int);
  WFIntArray* WFIntArray::sort_lh(void);
  WFIntArray* WFIntArray::sort_hl(void);
  void WFIntArray::reorder(WFIntArray*);
  void WFIntArray::add(void);
  void WFIntArray::add(int);
  void WFIntArray::remove(int);
  int WFIntArray::population(void);
  int WFIntArray::query(int);
  int WFIntArray::set(int,int);
  void WFIntArray::print(const char*);
};

#endif

#endif
