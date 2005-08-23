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
 Version: @(#)Connectivity.h	1.5 08/02/01
***********************************************************************/
#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACSIntArray.h"
#include "acs_starconsts.h"

static const int DEBUG_CONNECT=0;

// Scope settings
static const int INY=0;
static const int OUTY=1;

class Connectivity
{
 public:
   int node_count;         // Total number of nodes
   ACSIntArray* acs_id;    // Unique identifier of connected pin
   ACSIntArray* pin_id;    // Connected to pin id on destination sg
   ACSIntArray* pin_type;  // Connection type, either DATA_NODE, CTRL_NODE,
                           // or EXT_NODE
   ACSIntArray* pin_signal;// Pointer to the signal associated with the pin
   ACSIntArray* data_dir;  // Which direction does data flow over this connection?
   ACSIntArray* major_bit; // Majorbit representation for this connection
   ACSIntArray* bitlength; // Bitlength representation for this connection from the pin's superset

public:
   Connectivity::Connectivity();
   Connectivity::~Connectivity();
   Connectivity& operator=(Connectivity&);
   int Connectivity::initialize(void);
   int Connectivity::assign_node(const int,
				 const int,
				 const int);
   int Connectivity::assign_node(const int,
				 const int,
				 const int,
				 const int);
   int Connectivity::assign_node(const int,
				 const int,
				 const int,
				 const int,
				 const int);
   int Connectivity::add_node(const int,
			      const int,
			      const int);
   int Connectivity::find_acsnode(const int);
   int Connectivity::find_node(const int);
   int Connectivity::find_node(const int,
			       const int);
   int Connectivity::remove_node(const int);
   int Connectivity::remove_allnodes(void);
   int Connectivity::query_pinsignal(const int);
   int Connectivity::set_pinsignal(const int, 
				    const int);
   int Connectivity::query_acsid(const int);
   int Connectivity::query_pintype(const int);
   int Connectivity::query_pinid(const int);
   int Connectivity::connected(void);

   int Connectivity::dump(void);
};

#endif
