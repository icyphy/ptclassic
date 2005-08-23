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
 Version: @(#)import.h	1.5 07/30/01
***********************************************************************/
#ifndef HWSCHED
#define HWSCHED

#ifdef __GNUG__
#pragma interface
#endif

#include <strstream.h>
#include <limits.h>

#include "Target.h"
#include "IntState.h"
#include "StringState.h"
#include "SymbolList.h"
#include "SimControl.h"
#include "CodeStreamList.h"
#include "StringList.h"
#include "DataFlowStar.h"
#include "ImplementationCost.h"
#include "ACSCGFPGATarget.h"
#include "ConversionTable.h"

#include "IntArray.h"
#include "DoubleArray.h"
#include "StringArray.h"
#include "Sg_Constructs.h"
#include "Port_Timing.h"
#include "Connectivity.h"
#include "Delay.h"

class ACSCGFPGATarget;
class ACSStar;
class SDFSchedule;
class SDFScheduler;
class ACSCGCStar;
class ACSCGFPGACore;
class Port_Timing;
class Sg_Constructs;

static const int DEBUG=0;
static const int DEBUG_VHDL=0;
static const int DEBUG_MEMPORT=0;
static const int DEBUG_PIN=0;
static const int DEBUG_CONST=0;


class Capability {
  friend class ACSCGFPGATarget;
  friend class HWSchedule;

 public:
  StringList* domains;
  StringList* architectures;
  SequentialList* languages;

 public:
  Capability::Capability();
  Capability::~Capability();
  Capability& Capability::operator=(const Capability&);
  char* Capability::retrieve_string(StringList*,int);
  int Capability::add_domain(char*);
  int Capability::add_architecture(char*);
  int Capability::add_language(int);
  int Capability::get_language(int);
};




class Constants 
{
public:
  int count;
  void** storage;
  IntArray* types;

public:
 Constants::Constants(void);
 Constants::~Constants(void);
 void Constants::add_double(double);
 void Constants::add_float(float);
 void Constants::add_long(long);
 void Constants::add_int(int);
 void Constants::add(void*,
		     const int);
 void Constants::add_storage(void*);
 int Constants::get_int(int);
 long Constants::get_long(int);
 float Constants::get_float(int);
 double Constants::get_double(int);

 char* Constants::query_bitstr(int,
			       int,
			       int);
 char* Constants::query_str(int,
			    int,
			    int);
 int Constants::query_bitsize(int);
};


class Sequencer
{
public:
  ACSCGFPGACore* seq_sg;
  SequentialList* wc_consts;
  ACSCGFPGACore* wc_mux;
  ACSCGFPGACore* wc_add;
  
  // Currently attached to Memport
       //SequentialList* addr_consts;
  //ACSCGFPGACore* addr_mux;
  //ACSCGFPGACore* addr_add;
  
public:
  Sequencer::Sequencer(void);
  Sequencer::~Sequencer(void);
};


class Resource
{
 public:
  // FIX: For now the size will simply represent clb size in a rectangular sense
  //      This should become a derivable class based on domain and technology type
  int row;
  int col;


public:
Resource::Resource(void);
Resource::~Resource(void);
 int Resource::set_occupancy(int,int);
 int Resource::get_occupancy(int,int);
};


#endif




















