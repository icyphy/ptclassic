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
 Date of creation: 5/8/00
 Version: @(#)Connection_List.h      1.0     5/8/00

 -- Needed for make-depend
***********************************************************************/
#ifndef CONNECTION_LIST_H
#define CONNECTION_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "ACSIntArray.h"

class Connection_List {
 private:
  int total_connections;
  ACSIntArray* ext_src_pins;
  ACSIntArray* src_cores;
  ACSIntArray* src_pins;
  ACSIntArray* ext_dest_pins;
  ACSIntArray* dest_cores;
  ACSIntArray* dest_pins;
 public:
 Connection_List::Connection_List();
 Connection_List::~Connection_List();

  int Connection_List::count(void);
  void Connection_List::add(const int, const int, const int, 
			    const int, const int, const int);
  void Connection_List::get(const int, 
			    int&, int&, int&,
			    int&, int&, int&);
  void Connection_List::print(const char*);
};

#endif
