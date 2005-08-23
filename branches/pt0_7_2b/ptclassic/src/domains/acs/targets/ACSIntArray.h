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
 Version: @(#)ACSIntArray.h	1.5 08/02/01
***********************************************************************/
#ifndef ACSINTARRAY_H
#define ACSINTARRAY_H

class ACSIntArray
{
 private:
  int total;
  int* ints;

 public:
 ACSIntArray::ACSIntArray(void);
 ACSIntArray::ACSIntArray(int);
 ACSIntArray::ACSIntArray(int,int);
 ACSIntArray::~ACSIntArray(void);
  ACSIntArray& operator=(ACSIntArray&);
  void ACSIntArray::incr(int);
  void ACSIntArray::copy(ACSIntArray*);
  void ACSIntArray::cat(ACSIntArray*);
  void ACSIntArray::fill(const int);
  void ACSIntArray::fill(const int, const int);
  ACSIntArray* ACSIntArray::sort_lh(void);
  ACSIntArray* ACSIntArray::sort_hl(void);
  void ACSIntArray::reorder(ACSIntArray*);
  void ACSIntArray::add(void);
  void ACSIntArray::add(int);
  int ACSIntArray::add_unique(int);
  void ACSIntArray::remove(int);
  void ACSIntArray::remove_all(void);
  int ACSIntArray::population(void);
  int ACSIntArray::query(int);
  int ACSIntArray::get(int);
  int ACSIntArray::set(int,int);
  void ACSIntArray::print(const char*);
  int ACSIntArray::find(const int);
  int ACSIntArray::find_index(const int);
  int ACSIntArray::full(const int);
  int ACSIntArray::pop(void);
  int ACSIntArray::tail(void);
};

#endif

