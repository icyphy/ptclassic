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
 Version: $Id$
***********************************************************************/
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
  void IntArray::incr(int);
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
  int IntArray::find(int);
};

#endif

