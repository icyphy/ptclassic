#ifndef  _IntArray_h
#define  _IntArray_h 1
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

#ifndef INTARRAY_H
#define INTARRAY_H

class IntArray
{
 private:
  int total;
  int* ints;

 public:
 IntArray::IntArray(void);
 IntArray::IntArray(int);
 IntArray::IntArray(int,int);
 IntArray::~IntArray(void);
  IntArray& operator=(IntArray&);
  void IntArray::copy(IntArray*);
  void IntArray::fill(int);
  IntArray* IntArray::sort_lh(void);
  IntArray* IntArray::sort_hl(void);
  void IntArray::reorder(IntArray*);
  void IntArray::add(void);
  void IntArray::add(int);
  void IntArray::remove(int);
  int IntArray::population(void);
  int IntArray::query(int);
  int IntArray::set(int,int);
  void IntArray::print(const char*);
};

#endif

#endif
