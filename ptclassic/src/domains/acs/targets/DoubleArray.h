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
 Version: @(#)DoubleArray.h      1.0     06/16/99
***********************************************************************/
#ifndef DOUBLEARRAY_H
#define DOUBLEARRAY_H

class DoubleArray
{
 private:
  int total;
  double* doubles;

 public:
 DoubleArray::DoubleArray(void);
 DoubleArray::DoubleArray(int);
 DoubleArray::DoubleArray(int,double);
 DoubleArray::~DoubleArray(void);
  DoubleArray& operator=(DoubleArray&);
  void DoubleArray::copy(DoubleArray*);
  void DoubleArray::fill(double);
  void DoubleArray::add(void);
  void DoubleArray::add(double);
  void DoubleArray::remove(int);
  int DoubleArray::population(void);
  double DoubleArray::query(int);
  int DoubleArray::set(int,double);
  void DoubleArray::print(const char*);
};

#endif

