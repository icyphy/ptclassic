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
 Date of creation: 5/5/00
 Version: @(#)CoreList.h      1.0     5/5/00

 -- Needed for make-depend
***********************************************************************/
#ifndef CORELIST_H
#define CORELIST_H

#include <stdlib.h>
#include "ACSCGFPGACore.h"

class ACSCGFPGACore;

class CoreList {
  static const int DEBUG_CORELIST=0;

 private:
  int total_cores;
  ACSCGFPGACore** core_list;

 public:
 CoreList::CoreList();
 CoreList::~CoreList();
  int CoreList::size(void);
  void CoreList::append(ACSCGFPGACore*);
  ACSCGFPGACore* CoreList::elem(const int);
  void CoreList::remove(const int);
  ACSCGFPGACore* CoreList::find_core(const int);
  int CoreList::find_coreid(const int);
  void CoreList::contents(const char*);
};

#endif
