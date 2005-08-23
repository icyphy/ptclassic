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
 Version: @(#)acs_vhdl_lang.h      1.0     06/16/99
***********************************************************************/
#ifndef ACS_VHDL_H
#define ACS_VHDL_H

#ifdef __GNUG__
#pragma interface
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strstream.h>
#include <fstream.h>
#include <time.h>

//#include "StringList.h"
#include "CoreList.h"
#include "ACSIntArray.h"
#include "StringArray.h"
#include "ACSCGFPGACore.h"
#include "Pin.h"
#include "Connectivity.h"
#include "acs_starconsts.h"

class ACSCGFPGACore;
class CoreList;

// Debug flags
static const int DEBUG_LIB=0;
static const int DEBUG_BIND=0;
static const int DEBUG_SETPORT=0;
static const int DEBUG_SIGNALS=0;
static const int DEBUG_SETSIGNAL=0;
static const int DEBUG_INST=0;

// Data types
static const int STD_ULOGIC=0;
static const int STD_LOGIC=1;
static const int BIT=2;
static const int INTEGER=3;

// General definitions
static const int MAX_LEN=80;

// Architecture Definitions
static const int STRUCTURAL=0;
static const int BEHAVIORAL=1;
static const int BLANK=2;

class VHDL_LANG {
public:
  static const char begin_scope[]="begin ";
  static const char end_scope[]="end ";
  static const char end_statement[]=";";
  static const char if_statement[]="if ";
  static const char then_statement[]=" then";
  static const char else_statement[]="else";
  static const char elseif_statement[]="elsif ";
  static const char endif_statement[]="end if";
  static const char signal_declaration[]="signal ";
  static const char variable_declaration[]="variable ";
  static const char initial_value_assignment[]=":=";
  static const char vector_line[]=" downto 0";

  ACSIntArray* nc_sizes;

  VHDL_LANG::VHDL_LANG();
  VHDL_LANG::~VHDL_LANG();

  // Top level VHDL (glues intermediate statements) functions
  char* VHDL_LANG::gen_libraries(StringArray*,
				 StringArray*);
  char* VHDL_LANG::gen_entity(const char*,Pin*);
  char* VHDL_LANG::gen_architecture(const char*,
				    CoreList*,
				    const int,
				    Pin*, 
				    Pin*,
				    Pin*,
				    Pin*);
  char* VHDL_LANG::gen_components(const char*,CoreList*);
  char* VHDL_LANG::gen_configurations(CoreList*);
  char* VHDL_LANG::gen_instantiations(const int,
				      CoreList*,
				      Pin*,
				      Pin*,
				      Pin*,
				      Pin*);
  char* VHDL_LANG::const_definitions(Pin*);
  int VHDL_LANG::bind_ports(ACSCGFPGACore*,
			    StringArray*,
			    StringArray*,
			    Pin*,
			    Pin*,
			    Pin*,
			    Pin*);
  char* VHDL_LANG::set_nc(const int,Pin*);

  // Intermediate level VHDL (single line command) functions
  char* VHDL_LANG::set_entity(const char*);
  char* VHDL_LANG::start_architecture(const int,const char*);
  char* VHDL_LANG::start_architecture(const int,const strstreambuf*);
  char* VHDL_LANG::end_architecture(const int);
  char* VHDL_LANG::start_port(void);
  char* VHDL_LANG::set_port(Pin*);
  char* VHDL_LANG::pin_declaration(Pin*,const int);
  char* VHDL_LANG::end_port(void);
  char* VHDL_LANG::end_port(const char*);
  char* VHDL_LANG::end_port(const strstreambuf*);

  // FIX: signal and variable methods are similar in cc, should reduce the commonality!
  char* VHDL_LANG::signal(const char*,const int,const int);
  char* VHDL_LANG::signal(const strstreambuf*,const int,const int);
  char* VHDL_LANG::signal(const char*,const int,const int,const char*);
  char* VHDL_LANG::signal(const strstreambuf*,const int,const int,const strstreambuf*);
  char* VHDL_LANG::signal(const char*,const char*);
  char* VHDL_LANG::variable(const char*,const int,const int);
  char* VHDL_LANG::variable(const strstreambuf*,const int,const int);
  char* VHDL_LANG::variable(const char*,const int,const int,const char*);
  char* VHDL_LANG::variable(const strstreambuf*,const int,const int,const strstreambuf*);
  char* VHDL_LANG::variable(const char*,const char*);

  char* VHDL_LANG::start_component(const char*);
  char* VHDL_LANG::end_component(void);
  char* VHDL_LANG::signal_type(const int,const int);
  char* VHDL_LANG::for_use(const char*,const char*,const char*);
  char* VHDL_LANG::instantiate(const int,
			       const char*,
			       const char*,
			       StringArray*,
			       StringArray*);
  char* VHDL_LANG::type_def(const char*,
			    StringArray*);
  char* VHDL_LANG::process(const char*,
			   StringArray*);
  char* VHDL_LANG::begin_function_scope(const char*);
  char* VHDL_LANG::end_function_scope(const char*);
  char* VHDL_LANG::val(const char*);

  // Low level VHDL (glue) functions
  char* VHDL_LANG::equals(void);
  char* VHDL_LANG::equals(const char*,const char*);
  char* VHDL_LANG::equals(const strstreambuf*,const char*);
  char* VHDL_LANG::equals(const char*,const strstreambuf*);
  char* VHDL_LANG::equals(const strstreambuf*,const strstreambuf*);
  char* VHDL_LANG::expr(const char*,const char*);
  char* VHDL_LANG::expr(const strstreambuf*,const char*);
  char* VHDL_LANG::expr(const char*,const strstreambuf*);
  char* VHDL_LANG::expr(const strstreambuf*,const strstreambuf*);
  char* VHDL_LANG::test(const char*,const char*);
  char* VHDL_LANG::test(const strstreambuf*,const char*);
  char* VHDL_LANG::test(const strstreambuf*,const strstreambuf*);
  char* VHDL_LANG::l_if(const strstreambuf*,const strstreambuf*);
  char* VHDL_LANG::l_if(const char*,const strstreambuf*);
  char* VHDL_LANG::l_if(const char*,
			StringArray*);
  char* VHDL_LANG::l_if(const char*,const char*);
  char* VHDL_LANG::l_if(const strstreambuf*,
			StringArray*);
  char* VHDL_LANG::l_if(const strstreambuf*,
			StringArray&);
  char* VHDL_LANG::l_if(const char*,
			StringArray*,
			StringArray*);
  char* VHDL_LANG::l_if(const strstreambuf*,
			StringArray*,
			StringArray*);
  char* VHDL_LANG::l_case(const char*,
			  StringArray*,
			  StringArray*);
  char* VHDL_LANG::and(void);
  char* VHDL_LANG::and(const char*);
  char* VHDL_LANG::and(const strstreambuf*);
  char* VHDL_LANG::not(void);
  char* VHDL_LANG::not(const char*);
  char* VHDL_LANG::not(const strstreambuf*);
  char* VHDL_LANG::or(void);
  char* VHDL_LANG::or(const char*);
  char* VHDL_LANG::or(const strstreambuf*);
  char* VHDL_LANG::xor(const char*);
  char* VHDL_LANG::xor(const strstreambuf*);
  char* VHDL_LANG::xnor(const char*);
  char* VHDL_LANG::xnor(const strstreambuf*);
  char* VHDL_LANG::slice(const char*,const int,const int);
  char* VHDL_LANG::slice(const strstreambuf*,const int,const int);
  char* VHDL_LANG::slice(const char*,const int);
  char* VHDL_LANG::slice(const strstreambuf*,const int);
  char* VHDL_LANG::tie_it(const char*,const int,const int,const char*);

//  ostrstream VHDL_LANG::xor(const char*);
//  ostrstream VHDL_LANG::nand(const char*);
//  ostrstream VHDL_LANG::nor(const char*);
//  ostrstream VHDL_LANG::xnor(const char*);
  
  // Utility methods
  int dup_string(const char*,
		 StringArray*);

};

#endif



