/**********************************************************************
Copyright (c) 1999-%Q% Sanders, a Lockheed Martin Company
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
#ifndef SG_CONSTRUCTS_H
#define SG_CONSTRUCTS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "KnownBlock.h"
#include "SymbolList.h"
#include "ACSCGFPGACore.h"
#include "Fpga.h"
#include "Pin.h"
#include "acs_starconsts.h"

class ACSCGFPGACore;
class Fpga;

static const int DEBUG_CONSTRUCTS=0;

class Sg_Constructs
{
public:
  int valid;
  Fpga* target_fpga;
  int target_device;
  int* free_id;
  char* design_directory;
public:
  Sg_Constructs::Sg_Constructs(int*,char*);
  Sg_Constructs::Sg_Constructs(int*,
			       char*,
			       Fpga*,
			       const int);
  Sg_Constructs::~Sg_Constructs();

  void Sg_Constructs::validate(void);

  void Sg_Constructs::set_targetdevice(Fpga*,
				       const int);

  ACSCGFPGACore* Sg_Constructs::add_buffer(const int,
					   SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_ioport(const int,
					   SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_mux(const int,
					const int,
					SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_mux(const int,
					const int,
					const int,
					SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_const(int,
					  const int,
					  SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_const(long,
					  const int,
					  SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_const(double,
					  const int,
					  SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_adder(const int,
					  SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_subtracter(const int,
					       SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_delay(const int,
					  SequentialList*);


  ACSCGFPGACore* Sg_Constructs::add_sg(char*,
				       char*,
				       char*,
				       int,
				       int,
				       SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*,
				       char*,
				       char*,
				       int,
				       SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*, 
				       char*, 
				       char*,
				       int,
				       int,
				       int,
				       SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*, 
				       char*, 
				       char*,
				       int,
				       int,
				       int,
				       int,
				       SequentialList*);
  ACSCGFPGACore* Sg_Constructs::add_sg_core1(char*,
					     char*,
					     char*,
					     int,
					     int);

  ACSCGFPGACore* Sg_Constructs::add_sg_core2(ACSCGFPGACore*,
					     SequentialList*);

  int Sg_Constructs::insert_sg(ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  int Sg_Constructs::insert_sg(ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       Pin*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  int Sg_Constructs::insert_sg(Pin*,
			       ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  
  int Sg_Constructs::connect_sg(Pin*,int,
				ACSCGFPGACore*,int,
				int);
  int Sg_Constructs::connect_sg(Pin*,int,int,int,
				ACSCGFPGACore*,int,int,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,
				Pin*,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,int,int,
				Pin*,int,int,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,
				ACSCGFPGACore*,int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,int,
				ACSCGFPGACore*,int,int,
				int);
  int Sg_Constructs::connect_sg(SequentialList*,
				int*,
				int*,
				ACSCGFPGACore*,
				int,
				int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,
				int,
				int,
				int,
				ACSCGFPGACore*,
				int,
				int,
				int,
				int);
  int Sg_Constructs::connect_sg(SequentialList*,
				int*,
				int*,
				int,
				ACSCGFPGACore*,
				int,
				int,
				int,
				int);

};

#endif
